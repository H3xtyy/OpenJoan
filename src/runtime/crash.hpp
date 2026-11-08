// Unhandled-exception logging: when joan.exe crashes, the exception, the
// registers and a call stack (EBP chain plus code addresses found on the
// stack, each as module+offset) are written to openjoan.log.
#pragma once

#include <cstdint>
#include <string>

namespace joan::hook {

/// Installs the unhandled-exception handler that writes the exception, registers and a call stack to openjoan.log.
void install_crash_handler();

/// "openjoan.exe+0x1f6b85" for an address inside a loaded module, "" otherwise.
std::string describe_address(std::uintptr_t address);

}  // namespace joan::hook
