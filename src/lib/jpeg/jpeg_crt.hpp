// What the game's libjpeg compressor uses of its surroundings: the game CRT (malloc / free of jmemnobs, getenv +
// sscanf of jinit_memory_mgr, fwrite / fflush / ferror of jdatadst, sprintf / exit of jerror) and D3DX8's message
// table. Defined in jpeg_crt.cpp for the game (the CRT ports of lib/crt through their facades, the table in the data
// image); a test harness links its own definitions instead (re/p3/jpeg).
#pragma once

#include "jpeglib.hpp"

namespace joan::jpeg::crt {

void* malloc(jsize size);  // 0x007b6c54
void free(void* p);        // 0x007b4e1d
const char* getenv(const char* name);  // 0x007b7eb3
// sscanf(text, "%ld%c", value, ch) (0x007b3ec2); the format is jinit_memory_mgr's (passed as given).
int sscanf_long_char(const char* text, const char* format, jlong* value, char* ch);
jsize fwrite(const void* buffer, jsize size, jsize count, void* file);  // 0x007b7c14
int fflush(void* file);                                                // 0x007b7d4d
bool ferror(const void* file);  // the VC6 ferror macro: FILE::_flag & _IOERR
// sprintf (0x007b2952) as jerror's format_message calls it: one string argument, or the eight int parameters.
int sprintf_s(char* buffer, const char* format, const char* s);
int sprintf_i8(char* buffer, const char* format, const int (&i)[8]);
void exit_failure();  // exit(1) 0x007b47fc (does not return)
// jpeg_std_message_table (D3DX8 .rdata 0x009e1c68, last code 0x77).
const char* const* message_table();

}  // namespace joan::jpeg::crt
