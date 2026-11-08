// The game's File (src/ilib/file.cpp) as a save::Port, and the thin wrappers of io.hpp. Separate from io.cpp so the
// serialiser itself builds without the game (re/save/test).
#include "io.hpp"

#include <cstdint>

#include "../original/file.hpp"
#include "../original/file_mem.hpp"

namespace joan::save {
namespace {

std::int32_t file_write(void* f, const void* buf, std::uint32_t n) { return static_cast<orig::File*>(f)->file_write(buf, n); }
std::int32_t file_read(void* f, void* buf, std::uint32_t n) { return static_cast<orig::File*>(f)->file_read(buf, n); }
const char* file_name(void* f) { return static_cast<orig::File*>(f)->file_name; }
std::int32_t mem_write(void*, const void*, std::uint32_t) { return 0; }  // FileMem only reads
std::int32_t mem_read(void* f, void* buf, std::uint32_t n) { return static_cast<orig::FileMem*>(f)->file_read(buf, n); }
const char* mem_name(void* f) { return static_cast<orig::FileMem*>(f)->file_name; }

}  // namespace

Port port(orig::File& f) noexcept { return Port{&f, &file_write, &file_read, &file_name}; }
Port port(orig::FileMem& f) noexcept { return Port{&f, &mem_write, &mem_read, &mem_name}; }

Out::Out(orig::File& f, const Record& r, const void* legacy) noexcept : Out(port(f), r, legacy) {}
In::In(orig::File& f, const Record& r, void* legacy) noexcept : In(port(f), r, legacy) {}

std::int32_t put_i16(orig::File& f, std::int16_t v) { return f.file_put_short(v); }
std::int32_t put_u16(orig::File& f, std::uint16_t v) { return f.file_put_unsigned_short(v); }
std::int32_t put_i32(orig::File& f, std::int32_t v) { return f.file_put_long(v); }
std::int16_t get_i16(orig::File& f) { return f.file_get_short(); }
std::uint16_t get_u16(orig::File& f) { return f.file_get_unsigned_short(); }
std::int32_t get_i32(orig::File& f) { return f.file_get_long(); }
std::int32_t write_array(orig::File& f, const void* elems, std::uint32_t count, std::uint32_t elem_size) {
    return f.file_write_array(elems, count, elem_size);
}
std::int32_t read_array(orig::File& f, void* elems, std::uint32_t count, std::uint32_t elem_size) {
    return f.file_read_array(elems, count, elem_size);
}
std::int32_t read_array(orig::FileMem& f, void* elems, std::uint32_t count, std::uint32_t elem_size) {
    return f.file_read_array(elems, count, elem_size);
}

}  // namespace joan::save
