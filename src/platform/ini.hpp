// openjoan.ini without Windows' profile API (GetPrivateProfileInt / GetPrivateProfileString /
// WritePrivateProfileString), for builds on other systems: a small reader / writer with the same rules.
//   - Sections and keys match without regard to case; the first matching section and the first matching key win.
//   - A value is the text after the first '=', spaces and tabs trimmed; one pair of matching quotes around it is
//     removed. No inline comments (as Windows): "Key=1 ; note" reads "1 ; note".
//   - Lines without '=' (comments, blank lines) are skipped; a UTF-8 byte order mark is ignored.
// The file is read again only when its size or time changed (most settings are read once at start-up).
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

/// Reader and writer of the settings file with the rules of the Windows profile API.
namespace joan::platform::ini {

/// The key's value as a decimal number (leading spaces, an optional sign, digits; anything after them ignored), 0
/// when the value does not start with a number, `def` when there is no such key - as GetPrivateProfileIntA.
std::int32_t get_int(const char* section, const char* key, std::int32_t def, const std::filesystem::path& file);

/// The key's value, `def` when there is no such key.
std::string get_string(const char* section, const char* key, const char* def, const std::filesystem::path& file);

/// As GetPrivateProfileStringA: the value (or `def`) into out[0..size), cut to size - 1 characters and terminated;
/// returns the number of characters stored (size - 1 when it was cut).
std::uint32_t get_string(const char* section, const char* key, const char* def, char* out, std::uint32_t size,
                         const std::filesystem::path& file);

/// Sets the key's value, adding the key at the end of its section, or the section at the end of the file, when
/// missing. The rest of the file (comments, order, line ends) is kept. False when the file could not be written.
bool write_string(const char* section, const char* key, const char* value, const std::filesystem::path& file);

}  // namespace joan::platform::ini
