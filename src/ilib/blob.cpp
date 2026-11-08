// Reimplementation of Blob (layout in original/blob.hpp). The members are thin
// wrappers around Mem (mem.cpp); here they are only counted.
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../original/blob.hpp"
#include "../original/engine.hpp"
#include "../registry.hpp"

namespace {
using joan::hook::global;
using joan::orig::mem;

JOAN_COUNTER(c_ctor, "Blob::Blob(int)");
JOAN_COUNTER(c_copy, "Blob::Blob(Blob&)");
JOAN_COUNTER(c_dtor, "Blob::~Blob");
JOAN_COUNTER(c_resize, "Blob::resize");
JOAN_COUNTER(c_assign, "Blob::operator=");

constexpr img::Addr kSrc(d::s_d_ww_ilib_blob_cpp);  // "d:\ww\ilib\blob.cpp"
int line_at(img::Addr address, int offset = 0) { return global<std::int16_t>(address) + offset; }
}  // namespace

namespace joan::orig {

// 0x0057f770
Blob* Blob::construct(std::int32_t n) {
    c_ctor.hit();
    data = static_cast<char*>(mem().add(static_cast<std::uint32_t>(n), img::str(kSrc), line_at(d::g_a1510c)));
    size = n;
    return this;
}

// 0x0057f7d0
Blob* Blob::construct_copy(const Blob* other) {
    c_copy.hit();
    data = static_cast<char*>(mem().add(static_cast<std::uint32_t>(other->size), img::str(kSrc), line_at(d::g_a15110)));
    size = other->size;
    std::memcpy(data, other->data, static_cast<std::size_t>(other->size));
    return this;
}

// 0x0057f850
void Blob::destruct() {
    c_dtor.hit();
    if (data) mem().del(data, img::str(kSrc), line_at(d::g_a15114, 2));
}

// 0x0057f8a0
void Blob::resize(std::int32_t n) {
    c_resize.hit();
    data = static_cast<char*>(mem().resize(data, static_cast<std::uint32_t>(n), img::str(kSrc), line_at(d::g_a15118, 1)));
    size = n;
}

// 0x0057f900
Blob* Blob::assign(const Blob* other) {
    c_assign.hit();
    data = static_cast<char*>(mem().resize(data, static_cast<std::uint32_t>(other->size), img::str(kSrc), line_at(d::g_a1511c, 1)));
    size = other->size;
    std::memcpy(data, other->data, static_cast<std::size_t>(other->size));
    return this;
}

}  // namespace joan::orig

#define BLOB(name, addr, fn) JOAN_REPLACE("ilib/blob.cpp", "Blob::" name, addr, fn)
BLOB("Blob(int)", 0x0057f770, &joan::orig::Blob::construct);
BLOB("Blob(Blob&)", 0x0057f7d0, &joan::orig::Blob::construct_copy);
BLOB("~Blob", 0x0057f850, &joan::orig::Blob::destruct);
BLOB("resize", 0x0057f8a0, &joan::orig::Blob::resize);
BLOB("operator=", 0x0057f900, &joan::orig::Blob::assign);
#undef BLOB
