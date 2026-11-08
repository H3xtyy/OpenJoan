// DateInfo (d:\ww\ilib\date.cpp, the Seven Kingdoms ilib "ODATE" module),
// global `date` at 0x00a52928: Julian day numbers (day 1721426 = 1 Jan 1 AD,
// Gregorian), date / time strings, FILETIME conversions. It has no data
// members; its tables live at fixed addresses:
//
//   0x00a15198  int days_before_month[14]  (index 1..13: 0, 31, 59, .., 334, 365)
//   0x00a151d0  char time_buf[6]           "00:00", filled by time_str(int)
//   0x00a6432c  char empty_str[1]          returned for dates out of range
//   0x00a64340  String date_buf            function-static of date_str (guard bit 0 of 0x00a64328)
//
// Times of day are float minutes (hour * 60 + minute + second / 60).
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstdint>

#include "../data_names.hpp"
#include "../runtime/patch.hpp"

struct _FILETIME;

namespace joan::orig {

inline constexpr std::int32_t kJulianBase = 1721425;  ///< julian(1, 1, 1) - 1
inline constexpr std::int32_t kJulianMax = 5373484;  ///< 31 Dec 9999

/// The engine's calendar helper (global `date`, accessor date()); stateless apart from the fixed tables above.
/// Julian day numbers are int32; invalid input gives -1 (or the empty string for the string functions).
struct DateInfo {
    // --- reimplemented members (src/ilib/date.cpp) ---
    std::int32_t julian(std::int32_t year, std::int32_t month, std::int32_t day);  ///< 0x0057f990, -1 if invalid
    std::int32_t julian(const char* yyyymmdd);  ///< 0x0057fa10
    std::int32_t get_date(std::int32_t julian, char part);  ///< 0x0057fa80 'Y' / 'M' / 'D', -1 if out of range
    std::int32_t day_of_week(std::int32_t julian);  ///< 0x0057fc00 1..7
    std::int32_t day_year(std::int32_t year, std::int32_t month, std::int32_t day);  ///< 0x0057fef0 1..366, -1 if invalid
    std::int32_t year_to_julian(std::int32_t year);  ///< 0x0057fff0 days before 1 Jan
    std::int32_t month_day(std::int32_t year, std::int32_t day_of_year, std::int32_t* month, std::int32_t* day);  ///< 0x00580060
    char* time_str(std::int32_t hhmm);  ///< 0x00580180 "hh:mm"
    std::int32_t days_in_month(std::int32_t month);  ///< 0x00580230 (February: 28)
    std::int32_t add_months(std::int32_t julian, std::int32_t months);  ///< 0x00580270
    std::int32_t file_time_to_julian(const _FILETIME* ft);  ///< 0x00580350 (local time)
    std::int32_t today();  ///< 0x00580430

    // Floating-point members, with the x87 emulated (DateFp in date.cpp):
    //   char* date_str(int julian, int short_month, int weekday)   0x0057fc30 (weekday: 0 none, 1 full, else abbreviated)
    //   double file_time_to_minutes(const FILETIME*)               0x005803c0 (UTC; ST0)
    //   double minutes_now()                                       0x005804a0 (ST0)
    //   char* time_str(char* buf, float minutes, char seconds, char am_pm)  0x00580510
    //   char* time_str_hhmm(char* buf, float minutes)              0x005806c0 "hhmm"

    // The other DateInfo functions the game code calls (typed, through their slots).
#include "gen/date_info_methods.inc"
};

/// The global date helper `date` (d::g_date).
inline DateInfo& date() { return hook::global<DateInfo>(d::g_date); }

}  // namespace joan::orig
