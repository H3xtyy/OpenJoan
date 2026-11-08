// SPDX-License-Identifier: GPL-3.0-or-later
// Double-byte code pages: conversion to Unicode and the system's CJK font (dbcs.hpp, ADR-0004).
#include "dbcs.hpp"

#include <array>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <string_view>
#include <unordered_map>

#include "ini.hpp"
#include "window.hpp"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <iconv.h>
#endif

namespace joan::platform {
namespace {

#ifndef _WIN32
// iconv names of the code pages (glibc and musl know at least one of each list).
std::array<const char*, 3> iconv_names(unsigned cp) {
    switch (cp) {
        case 932: return {"CP932", "SHIFT_JIS", "SJIS"};
        case 936: return {"CP936", "GBK", "GB2312"};
        case 949: return {"CP949", "UHC", "EUC-KR"};
        case 950: return {"CP950", "BIG5", "BIG-5"};
        default: return {nullptr, nullptr, nullptr};
    }
}

struct Converter {
    iconv_t cd = reinterpret_cast<iconv_t>(-1);
};

std::mutex g_lock;
std::unordered_map<unsigned, Converter> g_converters;

iconv_t converter(unsigned cp) {
    auto [it, added] = g_converters.try_emplace(cp);
    if (added) {
        for (const char* name : iconv_names(cp)) {
            if (!name) continue;
            it->second.cd = iconv_open("UTF-32LE", name);
            if (it->second.cd != reinterpret_cast<iconv_t>(-1)) break;
        }
    }
    return it->second.cd;
}
#endif

bool is_file(const std::filesystem::path& p) {
    std::error_code ec;
    return std::filesystem::is_regular_file(p, ec);
}

// A file whose name contains `stem` (case-insensitive) anywhere below `dir` (a few levels deep).
[[maybe_unused]] std::string find_below(const std::filesystem::path& dir, std::string_view stem) {
    std::error_code ec;
    if (!std::filesystem::is_directory(dir, ec)) return {};
    std::filesystem::recursive_directory_iterator it(dir, std::filesystem::directory_options::skip_permission_denied, ec);
    for (; !ec && it != std::filesystem::recursive_directory_iterator(); it.increment(ec)) {
        if (it.depth() > 4) {
            it.disable_recursion_pending();
            continue;
        }
        if (!it->is_regular_file(ec)) continue;
        std::string name = it->path().filename().string();
        for (char& c : name) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        if (name.find(stem) != std::string::npos) return it->path().string();
    }
    return {};
}

}  // namespace

bool dbcs_code_page(unsigned cp) { return cp == 932 || cp == 936 || cp == 949 || cp == 950; }

char32_t dbcs_to_unicode(unsigned cp, std::uint16_t code) {
    if (!dbcs_code_page(cp)) return 0;
    char in[2] = {static_cast<char>(code >> 8), static_cast<char>(code & 0xff)};
#ifdef _WIN32
    wchar_t out[2] = {};
    const int n = ::MultiByteToWideChar(cp, MB_ERR_INVALID_CHARS, in, 2, out, 2);
    if (n == 1) return static_cast<char32_t>(out[0]);
    if (n == 2 && out[0] >= 0xd800 && out[0] < 0xdc00)
        return 0x10000 + ((static_cast<char32_t>(out[0]) - 0xd800) << 10) + (static_cast<char32_t>(out[1]) - 0xdc00);
    return 0;
#else
    std::lock_guard<std::mutex> guard(g_lock);
    iconv_t cd = converter(cp);
    if (cd == reinterpret_cast<iconv_t>(-1)) return 0;
    iconv(cd, nullptr, nullptr, nullptr, nullptr);  // initial state
    char32_t out = 0;
    char* src = in;
    std::size_t src_left = 2;
    char* dst = reinterpret_cast<char*>(&out);
    std::size_t dst_left = sizeof out;
    if (iconv(cd, &src, &src_left, &dst, &dst_left) == static_cast<std::size_t>(-1) || src_left != 0) return 0;
    return out;
#endif
}

const char* cjk_face_tag(unsigned cp) {
    switch (cp) {
        case 932: return "JP";
        case 949: return "KR";
        case 950: return "TC";
        default: return "SC";
    }
}

std::string cjk_font_file(unsigned cp) {
    const std::string setting = ini::get_string("Platform", "CjkFont", "", settings_file());
    if (setting == "off" || setting == "OFF" || setting == "0") return {};
    if (!setting.empty() && setting != "auto") return is_file(setting) ? setting : std::string{};
#ifdef _WIN32
    char windir[MAX_PATH] = {};
    if (::GetWindowsDirectoryA(windir, MAX_PATH) == 0) return {};
    const std::filesystem::path fonts = std::filesystem::path(windir) / "Fonts";
    static const char* const kSc[] = {"msyh.ttc", "simsun.ttc", "simhei.ttf"};
    static const char* const kTc[] = {"msjh.ttc", "mingliu.ttc", "kaiu.ttf"};
    static const char* const kJp[] = {"msgothic.ttc", "meiryo.ttc", "YuGothR.ttc"};
    static const char* const kKr[] = {"malgun.ttf", "gulim.ttc", "batang.ttc"};
    const char* const* list = cp == 950 ? kTc : cp == 932 ? kJp : cp == 949 ? kKr : kSc;
    for (int i = 0; i < 3; ++i)
        if (is_file(fonts / list[i])) return (fonts / list[i]).string();
    for (const char* const* l : {kSc, kTc, kJp, kKr})
        for (int i = 0; i < 3; ++i)
            if (is_file(fonts / l[i])) return (fonts / l[i]).string();
    return {};
#else
    (void)cp;  // the Linux fonts below hold every CJK script (the face is picked by cjk_face_tag)
    // Fixed places of the common packages first, then a search of the font folders by name.
    static const char* const kFiles[] = {
        "/usr/share/fonts/google-noto-sans-cjk-vf-fonts/NotoSansCJK-VF.ttc",           // Fedora
        "/usr/share/fonts/google-noto-cjk/NotoSansCJK-Regular.ttc",                    // older Fedora
        "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",                      // Debian / Ubuntu
        "/usr/share/fonts/noto-cjk/NotoSansCJK-Regular.ttc",                           // Arch
        "/usr/share/fonts/adobe-source-han-sans/SourceHanSans.ttc",                    // Arch (adobe-source-han-sans)
        "/usr/share/fonts/wqy-microhei/wqy-microhei.ttc",                              // Fedora / Arch
        "/usr/share/fonts/truetype/wqy/wqy-microhei.ttc",                              // Debian / Ubuntu
        "/usr/share/fonts/wqy-zenhei/wqy-zenhei.ttc",
        "/usr/share/fonts/truetype/wqy/wqy-zenhei.ttc",
        "/usr/share/fonts/google-droid-sans-fonts/DroidSansFallbackFull.ttf",          // Fedora
        "/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf",                   // Debian / Ubuntu
    };
    for (const char* f : kFiles)
        if (is_file(f)) return f;
    std::filesystem::path dirs[3] = {"/usr/share/fonts", "/usr/local/share/fonts", {}};
    if (const char* home = std::getenv("HOME")) dirs[2] = std::filesystem::path(home) / ".local/share/fonts";
    for (std::string_view stem : {"notosanscjk", "sourcehansans", "notoserifcjk", "wqy-microhei", "wqy-zenhei",
                                  "droidsansfallback", "uming", "ukai"})
        for (const auto& d : dirs)
            if (!d.empty())
                if (std::string f = find_below(d, stem); !f.empty()) return f;
    return {};
#endif
}

}  // namespace joan::platform
