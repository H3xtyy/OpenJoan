// Character sets for the platform layer (phase C5 round F3b, src/platform/README.md; text.cpp): the game's ANSI code page
// and conversions between it, UTF-16 and UTF-8 - without Windows.
#pragma once

#include <string>

/// Character sets: the game's ANSI code page and conversions between it, UTF-16 and UTF-8.
namespace joan::platform::text {

/// The game's ANSI / OEM code pages ([Platform] CodePage: auto = Windows' on Windows, 1252 / 850 elsewhere).
unsigned ansi_code_page();
/// The OEM code page (see ansi_code_page for the setting).
unsigned oem_code_page();

/// ANSI text (the ANSI code page) as UTF-8 (SDL's message boxes and titles).
std::string ansi_to_utf8(const char* s);
/// UTF-8 text as ANSI (unmappable characters: '?'; SDL's file dialog results).
std::string utf8_to_ansi(const char* s);
/// UTF-16 text (Windows' wide strings, 0-terminated) as UTF-8.
std::string utf16_to_utf8(const char16_t* s);

}  // namespace joan::platform::text
