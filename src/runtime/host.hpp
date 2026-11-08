#pragma once

#include <string>

/// Start-up and shutdown of the reimplementation inside the process.
namespace joan::host {

/// Load openjoan.ini from `dir` (with trailing separator), install every replacement, fixes, display and renderer hooks,
/// then seal the code table (src/code_table.hpp): from here on every code slot is final.
void start(const std::string& dir);
/// Write the end-of-run reports (call counters, traces). Safe to call more than once.
void shutdown();

}  // namespace joan::host
