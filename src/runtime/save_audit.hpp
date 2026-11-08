// [Debug] SaveAudit=1 (docs/RUNTIME.md, test points B0 / T1): do the game's save files contain absolute
// joan.exe image addresses? The File class replacement (ilib/file.cpp: file_write 0x005872e0 - the choke point of
// every save writer -, file_write_array, file_put_long) passes each written buffer here; every dword in
// [0x00400000, 0x00a96000) - or inside joan_image, the data image where it is now (S7b-main) - is logged with what lies
// at that (original) address (data object / vtable / string / thunk / code), and file_close logs one summary line per
// file. Logging only: what is written does not change.
#pragma once

#include <cstdint>

// The return address of the calling function (the code that writes the record).
#if defined(_MSC_VER) && !defined(__clang__)
#include "../compiler.hpp"
#define JOAN_SAVE_AUDIT_CALLER() JOAN_RETURN_ADDRESS()
#else
#define JOAN_SAVE_AUDIT_CALLER() __builtin_return_address(0)
#endif

/// [Debug] SaveAudit: logs absolute original image addresses found in written save files.
namespace joan::hook::save_audit {

/// settings().save_audit
bool enabled();
/// A buffer about to be written to `file_name` at file position `pos` by `what` (writer name); `caller` = the return
/// address of the File method (our code that writes the record).
void scan(const char* file_name, std::int32_t pos, const void* buf, std::uint32_t size, const char* what,
          const void* caller);
/// The file is closed: summary line (hits per kind), forget its counters.
void close(const char* file_name);

}  // namespace joan::hook::save_audit
