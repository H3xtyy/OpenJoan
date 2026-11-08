// joan.exe's code-page and locale imports on portable code (phase C5 round F3b, src/platform/README.md).
//
// The C runtime (lib/crt: mbstowcs / wcstombs, setlocale, _strupr / _strlwr / toupper of the non-"C" locales, strcoll,
// strftime's names) and a few of the game's own calls convert text between its ANSI code page and UTF-16, map case,
// classify characters and ask for locale strings. Census of a session: only MultiByteToWideChar / WideCharToMultiByte
// are called (the game keeps the "C" locale); the rest is the C runtime's locale code, implemented here for the day it
// runs.
//   - code pages: the single-byte Windows / DOS pages of the game's releases (1250, 1251, 1252, 1253, 1254, 1257, 437,
//     850, 866: text_tables.inc) and UTF-8; CP_ACP / CP_OEMCP / CP_THREAD_ACP = [Platform] CodePage (auto: the code
//     pages Windows uses, so the game reads its text files as before; 1252 / 850 on other systems). Undefined bytes map
//     to U+0080..U+00FF and back, unmappable characters become the default character ('?'), as Windows does without its
//     "best fit" tables;
//   - LCMapString (upper / lower case, sort keys), CompareString (ordinal, NORM_IGNORECASE), GetStringType (CT_CTYPE1):
//     Unicode's simple case mappings and classes for Latin, Greek and Cyrillic (text_case.inc); comparisons are by code
//     point, not Windows' linguistic order (only strcoll of a non-"C" locale would notice);
//   - GetLocaleInfo / GetUserDefaultLCID / IsValidLocale / EnumSystemLocales: the English (United States) locale, with
//     the ANSI / OEM code pages above.
#include "text.hpp"

#include "win32_types.hpp"
#include "../build_flavor.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#include "../runtime/loader/import_overrides.hpp"
#include "ini.hpp"
#include "threads.hpp"
#include "window.hpp"
#include "text_case.inc"

namespace {

namespace tt = joan::platform::text_tables;

// ---------------------------------------------------------------- settings

struct Pages {
    unsigned ansi = 1252;
    unsigned oem = 850;
};

#define JOAN_CODE_PAGE(cp, ...) {cp, {__VA_ARGS__}},
struct Table {
    unsigned cp;
    std::uint16_t hi[128];
};
const Table kTables[] = {
#include "text_tables.inc"
};
#undef JOAN_CODE_PAGE

const Table* table_of(unsigned cp) {
    for (const Table& t : kTables)
        if (t.cp == cp) return &t;
    return nullptr;
}

const Pages& pages() {
    static const Pages p = [] {
        Pages r;
        const std::filesystem::path& ini = joan::platform::settings_file();
        char value[32] = {};
        if constexpr (joan::kReleaseBuild) std::strcpy(value, "auto");  // release: always from Windows (build_flavor.hpp)
        else joan::platform::ini::get_string("Platform", "CodePage", "auto", value, sizeof(value), ini.c_str());
        if (_stricmp(value, "auto") == 0) {
#ifdef _WIN32
            if (table_of(::GetACP())) r.ansi = ::GetACP();
            if (table_of(::GetOEMCP())) r.oem = ::GetOEMCP();
#endif
        } else {
            const auto n = static_cast<unsigned>(std::strtoul(value, nullptr, 10));
            if (table_of(n)) r.ansi = n;
        }
        return r;
    }();
    return p;
}

unsigned resolve(unsigned cp) {
    switch (cp) {
        case CP_ACP:
        case CP_THREAD_ACP:
        case CP_MACCP:
            return pages().ansi;
        case CP_OEMCP:
            return pages().oem;
        default:
            return cp;
    }
}

bool known(unsigned cp) { return cp == CP_UTF8 || table_of(cp) != nullptr; }

void set_error(DWORD e) { joan::platform::set_last_error(e); }

// ---------------------------------------------------------------- conversions

// Bytes -> UTF-16 code units.
bool decode(unsigned cp, const char* s, std::size_t n, std::u16string& out) {
    out.clear();
    if (cp == CP_UTF8) {
        for (std::size_t i = 0; i < n;) {
            const auto c = static_cast<unsigned char>(s[i]);
            std::uint32_t v = 0xFFFD;
            std::size_t len = 1;
            if (c < 0x80) {
                v = c;
            } else if ((c >> 5) == 6 && i + 1 < n) {
                v = ((c & 0x1Fu) << 6) | (static_cast<unsigned char>(s[i + 1]) & 0x3Fu);
                len = 2;
            } else if ((c >> 4) == 14 && i + 2 < n) {
                v = ((c & 0x0Fu) << 12) | ((static_cast<unsigned char>(s[i + 1]) & 0x3Fu) << 6) |
                    (static_cast<unsigned char>(s[i + 2]) & 0x3Fu);
                len = 3;
            } else if ((c >> 3) == 30 && i + 3 < n) {
                v = ((c & 0x07u) << 18) | ((static_cast<unsigned char>(s[i + 1]) & 0x3Fu) << 12) |
                    ((static_cast<unsigned char>(s[i + 2]) & 0x3Fu) << 6) | (static_cast<unsigned char>(s[i + 3]) & 0x3Fu);
                len = 4;
            }
            i += len;
            if (v >= 0x10000) {
                v -= 0x10000;
                out.push_back(static_cast<char16_t>(0xD800 + (v >> 10)));
                out.push_back(static_cast<char16_t>(0xDC00 + (v & 0x3FF)));
            } else {
                out.push_back(static_cast<char16_t>(v));
            }
        }
        return true;
    }
    const Table* t = table_of(cp);
    if (!t) return false;
    for (std::size_t i = 0; i < n; ++i) {
        const auto c = static_cast<unsigned char>(s[i]);
        out.push_back(c < 0x80 ? static_cast<char16_t>(c) : static_cast<char16_t>(t->hi[c - 0x80]));
    }
    return true;
}

// UTF-16 -> bytes; `used_default` set when a character had no byte.
bool encode(unsigned cp, const char16_t* w, std::size_t n, char def, std::string& out, bool& used_default) {
    out.clear();
    used_default = false;
    if (cp == CP_UTF8) {
        for (std::size_t i = 0; i < n; ++i) {
            std::uint32_t v = w[i];
            if (v >= 0xD800 && v < 0xDC00 && i + 1 < n && w[i + 1] >= 0xDC00 && w[i + 1] < 0xE000) {
                v = 0x10000 + ((v - 0xD800) << 10) + (static_cast<std::uint32_t>(w[i + 1]) - 0xDC00);
                ++i;
            }
            if (v < 0x80) {
                out.push_back(static_cast<char>(v));
            } else if (v < 0x800) {
                out.push_back(static_cast<char>(0xC0 | (v >> 6)));
                out.push_back(static_cast<char>(0x80 | (v & 0x3F)));
            } else if (v < 0x10000) {
                out.push_back(static_cast<char>(0xE0 | (v >> 12)));
                out.push_back(static_cast<char>(0x80 | ((v >> 6) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | (v & 0x3F)));
            } else {
                out.push_back(static_cast<char>(0xF0 | (v >> 18)));
                out.push_back(static_cast<char>(0x80 | ((v >> 12) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | ((v >> 6) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | (v & 0x3F)));
            }
        }
        return true;
    }
    const Table* t = table_of(cp);
    if (!t) return false;
    static std::unordered_map<unsigned, std::unordered_map<char16_t, char>> reverse;  // built once per code page
    auto& rev = reverse[cp];
    if (rev.empty())
        for (int i = 0; i < 128; ++i) rev.emplace(static_cast<char16_t>(t->hi[i]), static_cast<char>(0x80 + i));
    for (std::size_t i = 0; i < n; ++i) {
        const char16_t c = w[i];
        if (c < 0x80) {
            out.push_back(static_cast<char>(c));
            continue;
        }
        auto it = rev.find(c);
        if (it != rev.end()) {
            out.push_back(it->second);
        } else {
            out.push_back(def);
            used_default = true;
        }
    }
    return true;
}

int WINAPI j_MultiByteToWideChar(UINT cp, DWORD, LPCCH s, int n, LPWSTR out, int cap) {
    if (!s || n == 0 || cap < 0) {
        set_error(ERROR_INVALID_PARAMETER);
        return 0;
    }
    const std::size_t len = n < 0 ? std::strlen(s) + 1 : static_cast<std::size_t>(n);
    std::u16string w;
    if (!decode(resolve(cp), s, len, w)) {
        set_error(ERROR_INVALID_PARAMETER);
        return 0;
    }
    const auto need = static_cast<int>(w.size());
    if (cap == 0) return need;
    if (cap < need) {
        set_error(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    std::memcpy(out, w.data(), w.size() * sizeof(char16_t));
    return need;
}

int WINAPI j_WideCharToMultiByte(UINT cp, DWORD, LPCWCH w, int n, LPSTR out, int cap, LPCCH def, LPBOOL used) {
    if (!w || n == 0 || cap < 0) {
        set_error(ERROR_INVALID_PARAMETER);
        return 0;
    }
    std::size_t len = 0;
    if (n < 0) {
        while (w[len]) ++len;
        ++len;
    } else {
        len = static_cast<std::size_t>(n);
    }
    std::string s;
    bool used_default = false;
    const unsigned page = resolve(cp);
    if (!encode(page, reinterpret_cast<const char16_t*>(w), len, def && page != CP_UTF8 ? *def : '?', s, used_default)) {
        set_error(ERROR_INVALID_PARAMETER);
        return 0;
    }
    if (used) *used = used_default ? TRUE : FALSE;
    const auto need = static_cast<int>(s.size());
    if (cap == 0) return need;
    if (cap < need) {
        set_error(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    std::memcpy(out, s.data(), s.size());
    return need;
}

UINT WINAPI j_GetACP() { return pages().ansi; }
UINT WINAPI j_GetOEMCP() { return pages().oem; }
BOOL WINAPI j_IsValidCodePage(UINT cp) { return known(cp) ? TRUE : FALSE; }

BOOL WINAPI j_GetCPInfo(UINT cp, LPCPINFO info) {
    const unsigned page = resolve(cp);
    if (!info || !known(page)) {
        set_error(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    std::memset(info, 0, sizeof(*info));
    info->MaxCharSize = page == CP_UTF8 ? 4 : 1;
    info->DefaultChar[0] = '?';
    return TRUE;  // single-byte pages: no lead-byte ranges
}

// ---------------------------------------------------------------- case, classes, comparisons

char16_t upper(char16_t c) { return c < tt::kCaseLimit ? static_cast<char16_t>(tt::kUpper[c]) : c; }
char16_t lower(char16_t c) { return c < tt::kCaseLimit ? static_cast<char16_t>(tt::kLower[c]) : c; }
WORD ctype1(char16_t c) {
    if (c < tt::kCaseLimit) return tt::kCtype1[c];
    if (c >= 0x2000 && c < 0x2070) return C1_DEFINED | C1_PUNCT;
    return C1_DEFINED | C1_ALPHA;
}

std::u16string widen_ansi(const char* s, int n) {
    const std::size_t len = n < 0 ? std::strlen(s) : static_cast<std::size_t>(n);
    std::u16string w;
    decode(pages().ansi, s, len, w);
    return w;
}
std::u16string widen(const WCHAR* s, int n) {
    std::size_t len = 0;
    if (n < 0) {
        while (s[len]) ++len;
    } else {
        len = static_cast<std::size_t>(n);
    }
    return std::u16string(reinterpret_cast<const char16_t*>(s), len);
}

// LCMapString on UTF-16 text; `sortkey` output is bytes.
bool map_string(DWORD flags, std::u16string& w, std::string* sortkey) {
    if (flags & LCMAP_SORTKEY) {
        // A key whose byte order is the ordinal order of the (case-folded with NORM_IGNORECASE) text.
        sortkey->clear();
        for (char16_t c : w) {
            const char16_t k = (flags & NORM_IGNORECASE) ? upper(c) : c;
            sortkey->push_back(static_cast<char>(k >> 8));
            sortkey->push_back(static_cast<char>(k & 0xFF));
        }
        sortkey->push_back('\0');
        return true;
    }
    for (char16_t& c : w) {
        if (flags & LCMAP_UPPERCASE) c = upper(c);
        else if (flags & LCMAP_LOWERCASE) c = lower(c);
        if (flags & LCMAP_BYTEREV) c = static_cast<char16_t>((c << 8) | (c >> 8));
    }
    return true;
}

int WINAPI j_LCMapStringW(LCID, DWORD flags, LPCWSTR src, int n, LPWSTR dst, int cap) {
    if (!src || n == 0) {
        set_error(ERROR_INVALID_PARAMETER);
        return 0;
    }
    std::u16string w = widen(src, n);
    if (n < 0) w.push_back(u'\0');
    if (flags & LCMAP_SORTKEY) {
        std::string key;
        map_string(flags, w, &key);
        const auto need = static_cast<int>(key.size());
        if (cap == 0) return need;
        if (cap < need) {
            set_error(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }
        std::memcpy(dst, key.data(), key.size());  // sort keys are bytes, the size counted in bytes
        return need;
    }
    map_string(flags, w, nullptr);
    const auto need = static_cast<int>(w.size());
    if (cap == 0) return need;
    if (cap < need) {
        set_error(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    std::memcpy(dst, w.data(), w.size() * sizeof(char16_t));
    return need;
}

int WINAPI j_LCMapStringA(LCID, DWORD flags, LPCSTR src, int n, LPSTR dst, int cap) {
    if (!src || n == 0) {
        set_error(ERROR_INVALID_PARAMETER);
        return 0;
    }
    std::u16string w = widen_ansi(src, n);
    if (n < 0) w.push_back(u'\0');
    std::string out;
    if (flags & LCMAP_SORTKEY) {
        map_string(flags, w, &out);
    } else {
        map_string(flags, w, nullptr);
        bool used = false;
        encode(pages().ansi, w.data(), w.size(), '?', out, used);
    }
    const auto need = static_cast<int>(out.size());
    if (cap == 0) return need;
    if (cap < need) {
        set_error(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    std::memcpy(dst, out.data(), out.size());
    return need;
}

int compare(const std::u16string& a, const std::u16string& b, DWORD flags) {
    const std::size_t n = std::min(a.size(), b.size());
    for (std::size_t i = 0; i < n; ++i) {
        char16_t x = a[i];
        char16_t y = b[i];
        if (flags & NORM_IGNORECASE) {
            x = upper(x);
            y = upper(y);
        }
        if (x != y) return x < y ? CSTR_LESS_THAN : CSTR_GREATER_THAN;
    }
    if (a.size() == b.size()) return CSTR_EQUAL;
    return a.size() < b.size() ? CSTR_LESS_THAN : CSTR_GREATER_THAN;
}

int WINAPI j_CompareStringA(LCID, DWORD flags, PCNZCH a, int na, PCNZCH b, int nb) {
    if (!a || !b) {
        set_error(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return compare(widen_ansi(a, na), widen_ansi(b, nb), flags);
}

int WINAPI j_CompareStringW(LCID, DWORD flags, PCNZWCH a, int na, PCNZWCH b, int nb) {
    if (!a || !b) {
        set_error(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return compare(widen(a, na), widen(b, nb), flags);
}

BOOL WINAPI j_GetStringTypeW(DWORD type, LPCWCH src, int n, LPWORD out) {
    if (!src || !out || type != CT_CTYPE1) {
        set_error(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    const std::u16string w = widen(src, n);
    for (std::size_t i = 0; i < w.size(); ++i) out[i] = ctype1(w[i]);
    if (n < 0) out[w.size()] = ctype1(0);
    return TRUE;
}

BOOL WINAPI j_GetStringTypeA(LCID, DWORD type, LPCSTR src, int n, LPWORD out) {
    if (!src || !out || type != CT_CTYPE1) {
        set_error(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    const std::u16string w = widen_ansi(src, n);
    for (std::size_t i = 0; i < w.size(); ++i) out[i] = ctype1(w[i]);
    if (n < 0) out[w.size()] = ctype1(0);
    return TRUE;
}

// ---------------------------------------------------------------- locale

constexpr LCID kLocale = 0x0409;  // English (United States)

const char* const kDays[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const char* const kMonths[12] = {"January", "February", "March",     "April",   "May",      "June",
                                 "July",    "August",   "September", "October", "November", "December"};

// The locale string of `type` (LOCALE_* without its flags); false if unknown.
bool locale_string(LCTYPE type, std::string& s) {
    static char buf[32];
    if (type >= LOCALE_SDAYNAME1 && type <= LOCALE_SDAYNAME7) {
        s = kDays[type - LOCALE_SDAYNAME1];
        return true;
    }
    if (type >= LOCALE_SABBREVDAYNAME1 && type <= LOCALE_SABBREVDAYNAME7) {
        s = std::string(kDays[type - LOCALE_SABBREVDAYNAME1]).substr(0, 3);
        return true;
    }
    if (type >= LOCALE_SMONTHNAME1 && type <= LOCALE_SMONTHNAME12) {
        s = kMonths[type - LOCALE_SMONTHNAME1];
        return true;
    }
    if (type >= LOCALE_SABBREVMONTHNAME1 && type <= LOCALE_SABBREVMONTHNAME12) {
        s = std::string(kMonths[type - LOCALE_SABBREVMONTHNAME1]).substr(0, 3);
        return true;
    }
    switch (type) {
        case LOCALE_ILANGUAGE: case LOCALE_IDEFAULTLANGUAGE: s = "0409"; return true;
        case LOCALE_SLANGUAGE: s = "English (United States)"; return true;
        case LOCALE_SENGLANGUAGE: s = "English"; return true;
        case LOCALE_SABBREVLANGNAME: s = "ENU"; return true;
        case LOCALE_SNATIVELANGNAME: s = "English"; return true;
        case LOCALE_ICOUNTRY: case LOCALE_IDEFAULTCOUNTRY: s = "1"; return true;
        case LOCALE_SCOUNTRY: case LOCALE_SENGCOUNTRY: case LOCALE_SNATIVECTRYNAME: s = "United States"; return true;
        case LOCALE_SABBREVCTRYNAME: s = "USA"; return true;
        case LOCALE_IDEFAULTCODEPAGE: std::snprintf(buf, sizeof buf, "%u", pages().oem); s = buf; return true;
        case LOCALE_IDEFAULTANSICODEPAGE: std::snprintf(buf, sizeof buf, "%u", pages().ansi); s = buf; return true;
        case LOCALE_SLIST: s = ","; return true;
        case LOCALE_IMEASURE: s = "1"; return true;
        case LOCALE_SDECIMAL: case LOCALE_SMONDECIMALSEP: s = "."; return true;
        case LOCALE_STHOUSAND: case LOCALE_SMONTHOUSANDSEP: s = ","; return true;
        case LOCALE_SGROUPING: case LOCALE_SMONGROUPING: s = "3;0"; return true;
        case LOCALE_IDIGITS: case LOCALE_ICURRDIGITS: case LOCALE_IINTLCURRDIGITS: s = "2"; return true;
        case LOCALE_ILZERO: s = "1"; return true;
        case LOCALE_SNATIVEDIGITS: s = "0123456789"; return true;
        case LOCALE_SCURRENCY: s = "$"; return true;
        case LOCALE_SINTLSYMBOL: s = "USD"; return true;
        case LOCALE_ICURRENCY: case LOCALE_INEGCURR: s = "0"; return true;
        case LOCALE_SDATE: s = "/"; return true;
        case LOCALE_STIME: s = ":"; return true;
        case LOCALE_SSHORTDATE: s = "M/d/yyyy"; return true;
        case LOCALE_SLONGDATE: s = "dddd, MMMM d, yyyy"; return true;
        case LOCALE_STIMEFORMAT: s = "h:mm:ss tt"; return true;
        case LOCALE_IDATE: case LOCALE_ILDATE: s = "0"; return true;
        case LOCALE_ITIME: s = "0"; return true;
        case LOCALE_ICENTURY: s = "1"; return true;
        case LOCALE_ITLZERO: s = "0"; return true;
        case LOCALE_IDAYLZERO: case LOCALE_IMONLZERO: s = "0"; return true;
        case LOCALE_S1159: s = "AM"; return true;
        case LOCALE_S2359: s = "PM"; return true;
        case LOCALE_SPOSITIVESIGN: s = ""; return true;
        case LOCALE_SNEGATIVESIGN: s = "-"; return true;
        case LOCALE_IPOSSIGNPOSN: s = "3"; return true;
        case LOCALE_INEGSIGNPOSN: s = "0"; return true;
        case LOCALE_IPOSSYMPRECEDES: case LOCALE_INEGSYMPRECEDES: s = "1"; return true;
        case LOCALE_IPOSSEPBYSPACE: case LOCALE_INEGSEPBYSPACE: s = "0"; return true;
        default: return false;
    }
}

template <class Ch>
int locale_info(LCTYPE type, Ch* out, int cap) {
    const bool number = (type & LOCALE_RETURN_NUMBER) != 0;
    type &= ~(LOCALE_NOUSEROVERRIDE | LOCALE_USE_CP_ACP | LOCALE_RETURN_NUMBER);
    std::string s;
    if (!locale_string(type, s)) {
        set_error(ERROR_INVALID_FLAGS);
        return 0;
    }
    if (number) {  // the value as a DWORD in the buffer (its size in characters)
        constexpr int need = static_cast<int>(sizeof(DWORD) / sizeof(Ch));
        if (cap == 0) return need;
        if (cap < need) {
            set_error(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }
        const auto v = static_cast<DWORD>(std::strtoul(s.c_str(), nullptr, 10));
        std::memcpy(out, &v, sizeof v);
        return need;
    }
    const auto need = static_cast<int>(s.size() + 1);
    if (cap == 0) return need;
    if (cap < need) {
        set_error(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    for (std::size_t i = 0; i <= s.size(); ++i) out[i] = static_cast<Ch>(static_cast<unsigned char>(s.c_str()[i]));
    return need;
}

int WINAPI j_GetLocaleInfoA(LCID, LCTYPE type, LPSTR out, int cap) { return locale_info(type, out, cap); }
int WINAPI j_GetLocaleInfoW(LCID, LCTYPE type, LPWSTR out, int cap) { return locale_info(type, out, cap); }
LCID WINAPI j_GetUserDefaultLCID() { return kLocale; }
BOOL WINAPI j_IsValidLocale(LCID lcid, DWORD) { return (lcid & 0x3FF) != 0 ? TRUE : FALSE; }
BOOL WINAPI j_EnumSystemLocalesA(LOCALE_ENUMPROCA proc, DWORD) {
    if (!proc) {
        set_error(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    char name[] = "00000409";
    proc(name);
    return TRUE;
}

}  // namespace

namespace joan::platform::text {

unsigned ansi_code_page() { return pages().ansi; }
unsigned oem_code_page() { return pages().oem; }

std::string ansi_to_utf8(const char* s) {
    if (!s) return {};
    std::u16string w;
    decode(pages().ansi, s, std::strlen(s), w);
    std::string u;
    bool used = false;
    encode(CP_UTF8, w.data(), w.size(), '?', u, used);
    return u;
}

std::string utf8_to_ansi(const char* s) {
    if (!s) return {};
    std::u16string w;
    decode(CP_UTF8, s, std::strlen(s), w);
    std::string a;
    bool used = false;
    encode(pages().ansi, w.data(), w.size(), '?', a, used);
    return a;
}

std::string utf16_to_utf8(const char16_t* s) {
    if (!s) return {};
    std::size_t n = 0;
    while (s[n]) ++n;
    std::string u;
    bool used = false;
    encode(CP_UTF8, s, n, '?', u, used);
    return u;
}

}  // namespace joan::platform::text

#define K(name) JOAN_IMPORT_OVERRIDE("KERNEL32.dll", #name, &j_##name, "portable " #name)
K(MultiByteToWideChar);
K(WideCharToMultiByte);
K(GetACP);
K(GetOEMCP);
K(IsValidCodePage);
K(GetCPInfo);
K(LCMapStringA);
K(LCMapStringW);
K(CompareStringA);
K(CompareStringW);
K(GetStringTypeA);
K(GetStringTypeW);
K(GetLocaleInfoA);
K(GetLocaleInfoW);
K(GetUserDefaultLCID);
K(IsValidLocale);
K(EnumSystemLocalesA);
#undef K
