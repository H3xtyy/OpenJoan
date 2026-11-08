// SPDX-License-Identifier: GPL-3.0-or-later
// Double-byte code pages for the localised editions (Chinese, Japanese, Korean): the character a lead / trail byte
// pair stands for, and a CJK font file of the system to draw it with (src/en2d/cjk_glyphs.cpp, ADR-0004).
#pragma once

#include <cstdint>
#include <string>

namespace joan::platform {

/// Whether `cp` is a double-byte code page this module converts (932 Japanese, 936 Simplified Chinese, 949 Korean,
/// 950 Traditional Chinese).
bool dbcs_code_page(unsigned cp);

/// The Unicode code point of the double-byte character `code` (lead byte << 8 | trail byte) in code page `cp`, 0 when
/// the pair is not a character of that page or the system cannot convert it (Linux: iconv; Windows:
/// MultiByteToWideChar).
char32_t dbcs_to_unicode(unsigned cp, std::uint16_t code);

/// A font file with the characters of code page `cp`: openjoan.ini [Platform] CjkFont when set (a path; "off"
/// disables CJK text), otherwise the first of the usual system fonts that exists (Noto Sans CJK / Source Han,
/// WenQuanYi, Droid Sans Fallback on Linux; Microsoft YaHei, SimSun, MingLiU, MS Gothic, Malgun Gothic on Windows).
/// Empty when none is found or CJK text is off.
std::string cjk_font_file(unsigned cp);

/// The script tag a font collection names the face of code page `cp` with ("SC", "TC", "JP", "KR"), for picking the
/// right face of a .ttc that holds several (Noto Sans CJK holds all four).
const char* cjk_face_tag(unsigned cp);

}  // namespace joan::platform
