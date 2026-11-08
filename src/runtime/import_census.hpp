// [Debug] ImportCensus=1: call counts of joan.exe's imports (import_census.cpp).
#pragma once

/// [Debug] ImportCensus: call counts of the imports of the original executable.
namespace joan::import_census {

/// Puts a counting thunk in front of every filled IAT slot (host::start, after the replay layer's wrappers).
void install();
/// openjoan_imports.txt next to openjoan.log (host::shutdown).
void write_report();

}  // namespace joan::import_census
