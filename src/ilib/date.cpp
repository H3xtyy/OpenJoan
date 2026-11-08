// Reimplementation of DateInfo (layout and tables in original/date.hpp).
//
// The year of a Julian day is estimated as (day - base) / 365.2425 on the x87,
// and times of day are float minutes, so those functions emulate the x87
// rounding (../x87.hpp): 24-bit precision control (the game's) rounds every step
// to float, any other state to double.
#include "../callconv.hpp"
#include "../platform/win32_types.hpp"

#include "../platform/files.hpp"

#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../original/date.hpp"
#include "../original/engine.hpp"
#include "../original/gameset.hpp"
#include "../registry.hpp"
#include "../x87.hpp"

namespace {
using joan::hook::global;
using joan::hook::original;
using joan::hook::x87::current_precision;
using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;
using joan::hook::x87::Precision;
using joan::orig::String;

JOAN_COUNTER(c_julian, "DateInfo::julian(y, m, d)");
JOAN_COUNTER(c_julian_str, "DateInfo::julian(yyyymmdd)");
JOAN_COUNTER(c_get_date, "DateInfo::get_date");
JOAN_COUNTER(c_day_of_week, "DateInfo::day_of_week");
JOAN_COUNTER(c_date_str, "DateInfo::date_str");
JOAN_COUNTER(c_day_year, "DateInfo::day_year");
JOAN_COUNTER(c_year_to_julian, "DateInfo::year_to_julian");
JOAN_COUNTER(c_month_day, "DateInfo::month_day");
JOAN_COUNTER(c_time_str_int, "DateInfo::time_str(hhmm)");
JOAN_COUNTER(c_days_in_month, "DateInfo::days_in_month");
JOAN_COUNTER(c_add_months, "DateInfo::add_months");
JOAN_COUNTER(c_ft_julian, "DateInfo::file_time_to_julian");
JOAN_COUNTER(c_ft_minutes, "DateInfo::file_time_to_minutes");
JOAN_COUNTER(c_today, "DateInfo::today");
JOAN_COUNTER(c_minutes_now, "DateInfo::minutes_now");
JOAN_COUNTER(c_time_str, "DateInfo::time_str(minutes)");
JOAN_COUNTER(c_time_str_hhmm, "DateInfo::time_str_hhmm");

// Read from the game's own table, so that out-of-range months read the same
// neighbouring memory the original does.
std::int32_t days_before_month(std::int32_t month) {
    return (&img::global<const std::int32_t>(d::g_a15198))[month];
}
char* time_buf() { return &global<char>(d::s_00_00); }
char* empty_str() { return &global<char>(d::g_a6432c); }
String& date_buf() { return global<String>(d::g_date_buf); }
std::uint8_t& date_buf_guard() { return global<std::uint8_t>(d::g_date_buf_guard); }

constexpr double kDaysPerYear = 365.2425;  // 0x009d8818
constexpr float k60 = 60.0f;               // 0x009cf9cc

bool is_leap(std::int32_t y) { return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0; }

// Wrapping 32-bit arithmetic, like the original's imul / add.
std::int32_t wrap(std::int64_t v) { return static_cast<std::int32_t>(static_cast<std::uint32_t>(v)); }

// --- statically linked MSVC 6 CRT ---
std::int32_t crt_time(std::int32_t* t) { return original<std::int32_t(JOAN_CDECL*)(std::int32_t*)>(fn::crt_time)(t); }
struct Tm {  // struct tm
    std::int32_t sec, min, hour, mday, mon, year, wday, yday, isdst;
};
Tm* crt_localtime(const std::int32_t* t) { return original<Tm*(JOAN_CDECL*)(const std::int32_t*)>(fn::crt_localtime)(t); }
using SprintfFn = int(JOAN_CDECL*)(char*, const char*, ...);
SprintfFn crt_sprintf() { return original<SprintfFn>(fn::crt_sprintf); }
void crt_atexit(std::uintptr_t fn) { original<int(JOAN_CDECL*)(std::uintptr_t)>(fn::crt_atexit)(fn); }

// --- text resources ("DATE" section of the game's TxtRes at 0x00a53f68) ---
struct TxtRes {
    int unused;
};
TxtRes& txt() { return global<TxtRes>(d::g_txt); }
char* txt_date_long(std::int32_t d, std::int32_t m, std::int32_t y) {  // 0x004d9d20 via thunk
    return (txt().*original<char* (TxtRes::*)(std::int32_t, std::int32_t, std::int32_t)>(fn::Menu_date_1))(d, m, y);
}
char* txt_date_short(std::int32_t d, std::int32_t m, std::int32_t y) {  // 0x004d9d70 via thunk
    return (txt().*original<char* (TxtRes::*)(std::int32_t, std::int32_t, std::int32_t)>(fn::Date_open_2))(d, m, y);
}
char* txt_weekday(std::int32_t dow, char abbreviated) {  // 0x004d9e30 via thunk
    return (txt().*original<char* (TxtRes::*)(std::int32_t, char)>(fn::TxtRes_weekday))(dow, abbreviated);
}
char* txt_am() { return (txt().*original<char* (TxtRes::*)()>(fn::Menu_ampm))(); }  // 0x004d9ea0
char* txt_pm() { return (txt().*original<char* (TxtRes::*)()>(fn::AmPm_open_2))(); }  // 0x004d9ed0

// --- String members, via their thunks ---
void str_construct(String& s) { (s.*original<String* (String::*)()>(fn::String))(); }
void str_assign(String& s, const char* t) { (s.*original<String* (String::*)(const char*)>(fn::op_assign_char_ptr))(t); }
void str_append(String& s, const char* t) { (s.*original<String* (String::*)(const char*)>(fn::op_add_assign_char_ptr))(t); }

// The emulated FPU at the precision control in force.
Fpu current_fpu() { return Fpu{current_precision() == Precision::Single}; }
}  // namespace

namespace joan::orig {

// ---------------------------------------------------------------------------
// Integer functions
// ---------------------------------------------------------------------------

// 0x0057fef0 - day of the year, or -1 for an invalid date.
std::int32_t DateInfo::day_year(std::int32_t year, std::int32_t month, std::int32_t day) {
    std::int32_t leap = is_leap(year) ? 1 : 0;
    std::int32_t dim = days_before_month(month + 1) - days_before_month(month);  // read before validation, as the original
    if (month == 2) dim += leap;
    if (year < 0 || month < 1 || month > 12 || day < 1 || day > dim) return -1;
    if (month <= 2) leap = 0;
    return days_before_month(month) + day + leap;
}

// 0x0057fff0 - days from 1 Jan 1 AD to 1 Jan `year`.
std::int32_t DateInfo::year_to_julian(std::int32_t year) {
    const std::int32_t y = year - 1;
    return wrap(static_cast<std::int64_t>(y) * 365 + y / 4 - y / 100 + y / 400);
}

// 0x00580060 - month and day of a day of the year; -1 (and 0 / 0) if out of range.
std::int32_t DateInfo::month_day(std::int32_t year, std::int32_t day_of_year, std::int32_t* month, std::int32_t* day) {
    std::int32_t leap = is_leap(year) ? 1 : 0;
    if (day_of_year <= 59) leap = 0;
    for (std::int32_t m = 2; m <= 13; ++m) {
        if (day_of_year <= days_before_month(m) + leap) {
            --m;
            *month = m;
            if (m <= 2) leap = 0;
            *day = day_of_year - days_before_month(m) - leap;
            return 0;
        }
    }
    *day = 0;
    *month = 0;
    return -1;
}

// 0x0057f990
std::int32_t DateInfo::julian(std::int32_t year, std::int32_t month, std::int32_t day) {
    const std::int32_t d = day_year(year, month, day);
    if (d < 1) return -1;
    return wrap(static_cast<std::int64_t>(year_to_julian(year)) + d + kJulianBase);
}

// 0x0057fa10 - "YYYYMMDD" (fields parsed day first, like the original: Misc's
// scratch buffer is left holding the year field).
std::int32_t DateInfo::julian(const char* s) {
    Misc& m = misc();
    using AtoiFn = std::int32_t (Misc::*)(const char*, std::uint32_t);
    const auto atoi_field = original<AtoiFn>(fn::atoi_field);
    const std::int32_t d = (m.*atoi_field)(s + 6, 2);
    const std::int32_t mo = (m.*atoi_field)(s + 4, 2);
    const std::int32_t y = (m.*atoi_field)(s, 4);
    return julian(y, mo, d);
}

// 0x0057fc00
std::int32_t DateInfo::day_of_week(std::int32_t j) { return j % 7 + 1; }

// 0x00580180 - "hh:mm" from an integer hhmm, in a static buffer.
char* DateInfo::time_str(std::int32_t hhmm) {
    char* buf = time_buf();
    buf[4] = static_cast<char>(hhmm % 10 + '0');
    buf[3] = static_cast<char>(hhmm / 10 % 10 + '0');
    buf[1] = static_cast<char>(hhmm / 100 % 10 + '0');
    buf[0] = static_cast<char>(hhmm / 1000 % 10 + '0');
    return buf;
}

// 0x00580230 - days in a month of a non-leap year.
std::int32_t DateInfo::days_in_month(std::int32_t month) {
    return days_before_month(month + 1) - days_before_month(month);
}

// 0x00580270
std::int32_t DateInfo::add_months(std::int32_t j, std::int32_t months) {
    std::int32_t y = get_date(j, 'Y');
    std::int32_t m = get_date(j, 'M');
    std::int32_t d = get_date(j, 'D');
    m += months;
    while (m > 12) {
        m -= 12;
        ++y;
    }
    while (m <= 0) {
        m += 12;
        --y;
    }
    if (d > days_in_month(m)) d = days_in_month(m);
    return julian(y, m, d);
}

// 0x00580350
std::int32_t DateInfo::file_time_to_julian(const _FILETIME* ft) {
    FILETIME local;
    SYSTEMTIME st;
    joan::platform::files::FileTimeToLocalFileTime(ft, &local);
    joan::platform::files::FileTimeToSystemTime(&local, &st);
    return julian(st.wYear, st.wMonth, st.wDay);
}

// 0x00580430
std::int32_t DateInfo::today() {
    std::int32_t t;
    crt_time(&t);
    const Tm* tm = crt_localtime(&t);
    return julian(tm->year + 1900, tm->mon + 1, tm->mday);
}

}  // namespace joan::orig

// ---------------------------------------------------------------------------
// Floating-point functions, evaluated at the emulated x87 precision
// ---------------------------------------------------------------------------
namespace joan::orig {

struct DateFp : DateInfo {
    // Year and day of the year of a Julian day in range (shared by get_date and date_str).
    void split(const Fpu& fpu, std::int32_t j, std::int32_t& year, std::int32_t& day_of_year) {
        const std::int32_t j0 = j - kJulianBase;
        year = ftol(fpu.div(j0, kDaysPerYear)) + 1;
        day_of_year = j0 - year_to_julian(year);
        if (day_of_year <= 0) {
            --year;
            day_of_year = j0 - year_to_julian(year);
        }
        const std::int32_t len = is_leap(year) ? 366 : 365;
        if (day_of_year > len) {
            ++year;
            day_of_year -= len;
        }
    }

    // 0x0057fa80
    std::int32_t get_date(const Fpu& fpu, std::int32_t j, char part) {
        if (j > kJulianMax || j < kJulianBase) return -1;
        std::int32_t y, doy, m, d;
        split(fpu, j, y, doy);
        if (month_day(y, doy, &m, &d) < 0) return -1;
        switch (part) {
            case 'Y': return y;
            case 'M': return m;
            case 'D': return d;
            default: return 0;
        }
    }

    // 0x0057fc30 - e.g. "January 5, 1429 (Thursday)", in a function-static String.
    char* date_str(const Fpu& fpu, std::int32_t j, std::int32_t short_month, std::int32_t weekday) {
        if (j > kJulianMax || j < kJulianBase) {
            *empty_str() = '\0';
            return empty_str();
        }
        std::int32_t y, doy, m, d;
        split(fpu, j, y, doy);
        if (month_day(y, doy, &m, &d) < 0) {
            *empty_str() = '\0';
            return empty_str();
        }
        if (!(date_buf_guard() & 1)) {  // first use: construct the static, destructor via atexit
            date_buf_guard() |= 1;
            str_construct(date_buf());
            crt_atexit(fn::code_value(fn::StaticInit_atexit_empty_57fed0));
        }
        str_assign(date_buf(), short_month ? txt_date_short(d, m, y) : txt_date_long(d, m, y));
        if (weekday) {
            str_append(date_buf(), " (");
            str_append(date_buf(), txt_weekday(day_of_week(j), weekday == 1 ? 0 : 1));
            str_append(date_buf(), ")");
        }
        return date_buf().str_buf;
    }

    // 0x005803c0 - UTC time of day of a FILETIME, in minutes.
    double file_time_to_minutes(const Fpu& fpu, const _FILETIME* ft) {
        SYSTEMTIME st;
        joan::platform::files::FileTimeToSystemTime(ft, &st);
        const std::int32_t hm = st.wHour * 60 + (st.wMinute & 0x3f);
        return fpu.add(hm, fpu.div(st.wSecond, k60));
    }

    // 0x005804a0 - local time of day, in minutes.
    double minutes_now(const Fpu& fpu) {
        std::int32_t t;
        crt_time(&t);
        const Tm* tm = crt_localtime(&t);
        const std::int32_t hm = tm->hour * 60 + tm->min;
        return fpu.add(hm, fpu.div(tm->sec, k60));
    }

    // 0x00580510 - "h:mm", "h:mm:ss", with " am" / " pm" on a 12-hour clock if asked.
    char* time_str(const Fpu& fpu, char* buf, float minutes, char seconds, char am_pm) {
        const std::int32_t h = ftol(fpu.div(minutes, k60));
        const std::int32_t m = ftol(fpu.sub(minutes, wrap(static_cast<std::int64_t>(h) * 60)));
        const std::int32_t s =
            ftol(fpu.mul(fpu.sub(fpu.sub(minutes, wrap(static_cast<std::int64_t>(h) * 60)), m), k60));
        if (am_pm) {
            std::int32_t h12 = h > 12 ? h - 12 : h;
            if (h12 == 0) h12 = 12;
            const char* suffix = h < 12 ? txt_am() : txt_pm();
            if (seconds) crt_sprintf()(buf, "%d:%.2d:%.2d %s", h12, m, s, suffix);
            else crt_sprintf()(buf, "%d:%.2d %s", h12, m, suffix);
        } else {
            if (seconds) crt_sprintf()(buf, "%d:%.2d:%.2d", h, m, s);
            else crt_sprintf()(buf, "%d:%.2d", h, m);
        }
        return buf;
    }

    // 0x005806c0 - "hhmm".
    char* time_str_hhmm(const Fpu& fpu, char* buf, float minutes) {
        const std::int32_t h = ftol(fpu.div(minutes, k60));
        const std::int32_t m = ftol(fpu.sub(minutes, wrap(static_cast<std::int64_t>(h) * 60)));
        crt_sprintf()(buf, "%.2d%.2d", h, m);
        return buf;
    }
};
static_assert(sizeof(DateFp) == sizeof(DateInfo));

// The plain members declared in date.hpp, at the current precision (used by
// add_months and by other reimplemented code).
std::int32_t DateInfo::get_date(std::int32_t j, char part) {
    return static_cast<DateFp*>(this)->get_date(current_fpu(), j, part);
}

}  // namespace joan::orig

// ---------------------------------------------------------------------------
// Entry points (thiscall, same signatures as the original)
// ---------------------------------------------------------------------------
namespace {
using joan::hook::Counter;
using joan::orig::DateFp;
using joan::orig::DateInfo;

struct Entry : DateFp {
    // Integer results.
    template <class Mine>
    std::int32_t counted(Counter& c, Mine mine) {
        c.hit();
        return mine();
    }

    std::int32_t julian_entry(std::int32_t y, std::int32_t m, std::int32_t d) {
        return counted(c_julian, [&] { return julian(y, m, d); });
    }
    std::int32_t julian_str_entry(const char* s) { return counted(c_julian_str, [&] { return julian(s); }); }
    std::int32_t day_of_week_entry(std::int32_t j) { return counted(c_day_of_week, [&] { return day_of_week(j); }); }
    std::int32_t day_year_entry(std::int32_t y, std::int32_t m, std::int32_t d) {
        return counted(c_day_year, [&] { return day_year(y, m, d); });
    }
    std::int32_t year_to_julian_entry(std::int32_t y) {
        return counted(c_year_to_julian, [&] { return year_to_julian(y); });
    }
    std::int32_t month_day_entry(std::int32_t y, std::int32_t doy, std::int32_t* month, std::int32_t* day) {
        c_month_day.hit();
        return month_day(y, doy, month, day);
    }
    std::int32_t days_in_month_entry(std::int32_t m) {
        return counted(c_days_in_month, [&] { return days_in_month(m); });
    }
    std::int32_t add_months_entry(std::int32_t j, std::int32_t n) {
        return counted(c_add_months, [&] { return add_months(j, n); });
    }
    std::int32_t file_time_to_julian_entry(const _FILETIME* ft) {
        return counted(c_ft_julian, [&] { return file_time_to_julian(ft); });
    }
    std::int32_t today_entry() { return counted(c_today, [&] { return today(); }); }
    char* time_str_int_entry(std::int32_t hhmm) {
        c_time_str_int.hit();
        return DateInfo::time_str(hhmm);
    }

    std::int32_t get_date_entry(std::int32_t j, char part) {
        c_get_date.hit();
        return DateFp::get_date(current_fpu(), j, part);
    }
    char* date_str_entry(std::int32_t j, std::int32_t short_month, std::int32_t weekday) {
        c_date_str.hit();
        return date_str(current_fpu(), j, short_month, weekday);
    }
    double file_time_to_minutes_entry(const _FILETIME* ft) {
        c_ft_minutes.hit();
        return file_time_to_minutes(current_fpu(), ft);
    }
    double minutes_now_entry() {
        c_minutes_now.hit();
        return minutes_now(current_fpu());
    }
    char* time_str_entry(char* buf, float minutes, char seconds, char am_pm) {
        c_time_str.hit();
        return time_str(current_fpu(), buf, minutes, seconds, am_pm);
    }
    char* time_str_hhmm_entry(char* buf, float minutes) {
        c_time_str_hhmm.hit();
        return time_str_hhmm(current_fpu(), buf, minutes);
    }
};
static_assert(sizeof(Entry) == sizeof(DateInfo));
}  // namespace

#define DATE(name, addr, fn) JOAN_REPLACE("ilib/date.cpp", "DateInfo::" name, addr, fn)
DATE("julian(y, m, d)", 0x0057f990, &Entry::julian_entry);
DATE("julian(yyyymmdd)", 0x0057fa10, &Entry::julian_str_entry);
DATE("get_date", 0x0057fa80, &Entry::get_date_entry);
DATE("day_of_week", 0x0057fc00, &Entry::day_of_week_entry);
DATE("date_str", 0x0057fc30, &Entry::date_str_entry);
DATE("day_year", 0x0057fef0, &Entry::day_year_entry);
DATE("year_to_julian", 0x0057fff0, &Entry::year_to_julian_entry);
DATE("month_day", 0x00580060, &Entry::month_day_entry);
DATE("time_str(hhmm)", 0x00580180, &Entry::time_str_int_entry);
DATE("days_in_month", 0x00580230, &Entry::days_in_month_entry);
DATE("add_months", 0x00580270, &Entry::add_months_entry);
DATE("file_time_to_julian", 0x00580350, &Entry::file_time_to_julian_entry);
DATE("file_time_to_minutes", 0x005803c0, &Entry::file_time_to_minutes_entry);
DATE("today", 0x00580430, &Entry::today_entry);
DATE("minutes_now", 0x005804a0, &Entry::minutes_now_entry);
DATE("time_str(minutes)", 0x00580510, &Entry::time_str_entry);
DATE("time_str_hhmm", 0x005806c0, &Entry::time_str_hhmm_entry);
#undef DATE

// ---------------------------------------- from modules/game_b105_15.cpp (game/b105_15)
namespace {
namespace game_b105_15 {

void JOAN_CDECL empty_cdecl() {}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("StaticInit::atexit_empty_57fed0", 0x0057fed0, &empty_cdecl);
#undef B

}  // namespace game_b105_15
}  // namespace
