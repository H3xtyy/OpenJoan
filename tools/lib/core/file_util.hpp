#pragma once

#include <cstddef>
#include <filesystem>
#include <vector>

namespace joan {

/// Reads a whole file into memory. Throws std::runtime_error on failure.
std::vector<std::byte> read_file(const std::filesystem::path& path);

/// Writes a buffer to a file, creating parent directories. Throws on failure.
void write_file(const std::filesystem::path& path, const void* data, std::size_t size);

}  // namespace joan
