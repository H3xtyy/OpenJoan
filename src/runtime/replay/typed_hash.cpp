// The typed table of the replay's image hash: this build's (hash_types.inc), its form in a recording. See
// typed_hash.hpp.
#include "typed_hash.hpp"

#include <cstring>
#include <iterator>

namespace joan::replay::typed {

namespace {

struct PtrAt {
    std::uint32_t va;
};
struct UntypedAt {
    std::uint32_t va, size;
};

constexpr PtrAt kPointers[] = {
#define JOAN_HASH_PTR(va, path) {va},
#include "hash_types.inc"
#undef JOAN_HASH_PTR
};
constexpr UntypedAt kUntypedRanges[] = {
#define JOAN_HASH_UNTYPED(va, size, path) {va, size},
#include "hash_types.inc"
#undef JOAN_HASH_UNTYPED
};

constexpr std::uint32_t kSerialVersion = 1;

void put(std::string& s, std::uint32_t v) { s.append(reinterpret_cast<const char*>(&v), 4); }

}  // namespace

Table builtin(std::uint32_t va, std::uint32_t size) {
    Table t;
    t.pointers.reserve(std::size(kPointers));
    for (const PtrAt& p : kPointers)
        if (p.va >= va && p.va - va + 4 <= size) t.pointers.push_back(p.va - va);
    for (const UntypedAt& u : kUntypedRanges)
        if (u.va >= va && u.va - va + u.size <= size) t.untyped.push_back({u.va - va, u.va - va + u.size});
    return t;
}

std::string serialise(const Table& t) {
    std::string s;
    s.reserve(12 + 4 * t.pointers.size() + 8 * t.untyped.size());
    put(s, kSerialVersion);
    put(s, static_cast<std::uint32_t>(t.pointers.size()));
    for (const std::uint32_t o : t.pointers) put(s, o);
    put(s, static_cast<std::uint32_t>(t.untyped.size()));
    for (const Span& u : t.untyped) {
        put(s, u.begin);
        put(s, u.end);
    }
    return s;
}

bool deserialise(const std::uint8_t* p, std::size_t n, Table& out) {
    out = Table{};
    auto get = [&](std::uint32_t& v) {
        if (n < 4) return false;
        std::memcpy(&v, p, 4);
        p += 4;
        n -= 4;
        return true;
    };
    std::uint32_t version = 0, count = 0;
    if (!get(version) || version != kSerialVersion || !get(count) || count > n / 4) return false;
    out.pointers.resize(count);
    for (std::uint32_t& o : out.pointers) get(o);
    if (!get(count) || count > n / 8) return false;
    out.untyped.resize(count);
    for (Span& u : out.untyped) {
        get(u.begin);
        get(u.end);
    }
    return n == 0;
}

std::uint64_t fingerprint(const Table& t) {
    const std::string s = serialise(t);
    std::uint64_t h = 0xcbf29ce484222325ull;
    for (const char c : s) {
        h ^= static_cast<std::uint8_t>(c);
        h *= 0x100000001b3ull;
    }
    return h;
}

bool classes(const Table& t, std::uint32_t size, std::vector<std::uint8_t>& out) {
    out.assign(size, kTyped);
    bool ok = true;
    for (const std::uint32_t o : t.pointers) {
        if (o > size || size - o < 4) {
            ok = false;
            continue;
        }
        for (std::uint32_t k = 0; k < 4; ++k) {
            ok = ok && out[o + k] == kTyped;
            out[o + k] = kPointer;
        }
    }
    for (const Span& u : t.untyped) {
        if (u.begin > u.end || u.end > size) {
            ok = false;
            continue;
        }
        for (std::uint32_t o = u.begin; o < u.end; ++o) {
            ok = ok && out[o] == kTyped;
            out[o] = kUntyped;
        }
    }
    return ok;
}

}  // namespace joan::replay::typed
