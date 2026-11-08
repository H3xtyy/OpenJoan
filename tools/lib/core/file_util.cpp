#include "core/file_util.hpp"

#include <fstream>
#include <stdexcept>

namespace joan {

std::vector<std::byte> read_file(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in) throw std::runtime_error("cannot open file: " + path.string());
    const auto size = static_cast<std::size_t>(in.tellg());
    std::vector<std::byte> buf(size);
    in.seekg(0);
    if (size && !in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(size)))
        throw std::runtime_error("cannot read file: " + path.string());
    return buf;
}

void write_file(const std::filesystem::path& path, const void* data, std::size_t size) {
    if (path.has_parent_path()) std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) throw std::runtime_error("cannot create file: " + path.string());
    out.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
    if (!out) throw std::runtime_error("cannot write file: " + path.string());
}

}  // namespace joan
