// game/unknown_rle.cpp: functions whose original source file is unknown (formerly game/_unplaced/rle.cpp;
// re/attribution.csv: between ww/ilib/resx.cpp and ww/ilib/se.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/dynarray.hpp"
#include "../x87.hpp"
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
namespace {
namespace game_b94_05 {

struct H {
    // 0x0059d190
    void* self_59d190() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("Obj::self_59d190", 0x0059d190, &H::self_59d190);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

struct H {
    void empty() {}  // 0x0058e720 / 0x0059d1b0
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("empty_59d1b0", 0x0059d1b0, &H::empty);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b101_10.cpp (game/b101_10)
namespace {
namespace game_b101_10 {

using joan::orig::helpers::crt_memset;

JOAN_COUNTER(c_rle_size, "Rle::encoded_size");
JOAN_COUNTER(c_rle_encode, "Rle::encode");
JOAN_COUNTER(c_rle_file, "Rle::encode_file");

struct H {
    // ------------------------------------------------------------------ 0x0059d1d0
    static i32 size_flush(u8 c, u8 esc, i32 run, i32 out) {
        if (c == esc || run > 2) {
            while (run > 0xff) {
                out += 3;
                run -= 0xff;
            }
            out += 3;
        } else {
            while (run > 0) {
                ++out;
                --run;
            }
        }
        return out;
    }
    i32 encoded_size(const u8* src, i32 n, i32 esc_arg) {
        c_rle_size.hit();
        const u8 esc = static_cast<u8>(esc_arg);
        i32 out = 0;
        if (n == 0) return 2;
        u8 c = *src;
        i32 run = 1;
        ++src;
        --n;
        for (; n > 0; --n, ++src) {
            if (*src != c) {
                out = size_flush(c, esc, run, out);
                c = *src;
                run = 0;
            }
            ++run;
        }
        out = size_flush(c, esc, run, out);
        out += 2;
        return out;
    }

    // ------------------------------------------------------------------ 0x0059d3b0
    static i32 encode_flush(u8* dst, u8 c, u8 esc, i32 run, i32 o) {
        if (c == esc || run > 2) {
            while (run > 0xff) {
                dst[o] = esc;
                dst[o + 1] = 0xff;
                dst[o + 2] = c;
                o += 3;
                run -= 0xff;
            }
            dst[o] = esc;
            dst[o + 1] = static_cast<u8>(run);
            dst[o + 2] = c;
            o += 3;
        } else {
            while (run > 0) {
                dst[o] = c;
                ++o;
                --run;
            }
        }
        return o;
    }
    i32 encode(const u8* src, i32 n, u8* dst, i32 esc_arg) {
        c_rle_encode.hit();
        const u8 esc = static_cast<u8>(esc_arg);
        i32 o = 0;
        if (n == 0) {
            dst[0] = esc;
            dst[1] = 0;
            return 2;
        }
        u8 c = *src;
        i32 run = 1;
        ++src;
        --n;
        for (; n > 0; --n, ++src) {
            if (*src != c) {
                o = encode_flush(dst, c, esc, run, o);
                c = *src;
                run = 0;
            }
            ++run;
        }
        o = encode_flush(dst, c, esc, run, o);
        dst[o] = esc;
        dst[o + 1] = 0;
        o += 2;
        return o;
    }

    // ------------------------------------------------------------------ 0x0059d780
    static bool file_write(void* file, u8* buf, i32 n) {
        return joan::orig::as_file(file)->file_write(static_cast<void*>(buf), joan::orig::arg::u32(n)) != 0;  // 0x005872e0
    }
    // Returns false on a failed write; out is updated as the original's counter.
    static bool file_flush(void* file, u8* buf, u8 c, u8 esc, i32 run, i32& out) {
        if (c == esc || run > 2) {
            while (run > 0xff) {
                buf[0] = esc;
                buf[1] = 0xff;
                buf[2] = c;
                if (!file_write(file, buf, 3)) return false;
                out += 3;
                run -= 0xff;
            }
            buf[0] = esc;
            buf[1] = static_cast<u8>(run);
            buf[2] = c;
            if (!file_write(file, buf, 3)) return false;
            out += 3;
        } else {
            crt_memset(buf, c, static_cast<u32>(run));
            if (!file_write(file, buf, run)) return false;
            out += run;
        }
        return true;
    }
    i32 encode_file(const u8* src, i32 n, void* file, i32 esc_arg) {
        c_rle_file.hit();
        const u8 esc = static_cast<u8>(esc_arg);
        u8 buf[4];
        i32 out = 0;
        if (n == 0) {
            buf[0] = esc;
            buf[1] = 0;
            if (!file_write(file, buf, 2)) return -1;
            return 2;
        }
        u8 c = *src;
        i32 run = 1;
        ++src;
        --n;
        for (; n > 0; --n, ++src) {
            if (*src != c) {
                if (!file_flush(file, buf, c, esc, run, out)) return -1;
                c = *src;
                run = 0;
            }
            ++run;
        }
        if (!file_flush(file, buf, c, esc, run, out)) return -1;
        buf[0] = esc;
        buf[1] = 0;
        if (!file_write(file, buf, 2)) return -1;
        out += 2;
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_10", name, addr, fn)
B("Rle::encoded_size", 0x0059d1d0, &H::encoded_size);
B("Rle::encode", 0x0059d3b0, &H::encode);
B("Rle::encode_file", 0x0059d780, &H::encode_file);
#undef B

}  // namespace game_b101_10
}  // namespace

// ---------------------------------------- from modules/game_b102_09.cpp (game/b102_09)
namespace {
namespace game_b102_09 {

JOAN_COUNTER(c_rle_mem, "Rle::decode_mem");
JOAN_COUNTER(c_rle_file, "Rle::decode_file");

struct H {
    // ------------------------------------------------------------------------ 0x0059d690
    u32 rle_mem(const u8* src, u8* dst, u32 marker) {
        c_rle_mem.hit();
        u32 n = 0;
        for (;;) {
            if (static_cast<u32>(src[0]) == (marker & 0xffu)) {
                if (src[1] == 0) break;
                memset_(dst + n, static_cast<i32>(src[2]), static_cast<u32>(src[1]));
                n += src[1];
                src += 3;
            } else {
                dst[n] = src[0];
                ++n;
                ++src;
            }
        }
        return n;
    }

    // ------------------------------------------------------------------------ 0x0059daf0
    i32 rle_file(void* file, u8* dst, u32 marker) {
        c_rle_file.hit();
        u8 buf[4] = {0, 0, 0, 0};
        i32 n = 0;
        for (;;) {
            if (joan::orig::as_file(file)->file_read(static_cast<void*>(&buf[0]), 1u) == 0) return -1;                         // 0x005873a0
            if (static_cast<u32>(buf[0]) == (marker & 0xffu)) {
                if (joan::orig::as_file(file)->file_read(static_cast<void*>(&buf[1]), 1u) == 0) return -1;
                if (buf[1] == 0) break;
                if (joan::orig::as_file(file)->file_read(static_cast<void*>(&buf[2]), 1u) == 0) return -1;
                memset_(dst + n, static_cast<i32>(buf[2]), static_cast<u32>(buf[1]));
                n = static_cast<i32>(static_cast<u32>(n) + buf[1]);
            } else {
                dst[n] = buf[0];
                n = static_cast<i32>(static_cast<u32>(n) + 1u);
            }
        }
        return n;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_09", name, addr, fn)
B("Rle::decode_mem", 0x0059d690, &H::rle_mem);
B("Rle::decode_file", 0x0059daf0, &H::rle_file);
#undef B

}  // namespace game_b102_09
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
namespace {
namespace game_b92_04 {

JOAN_COUNTER(c_rle_decode, "Rle::decode");

struct H {
    // ------------------------------------------------------------------------ 0x0059dc10
    i32 rle_decode(void* stream, u8* out, i32 marker) {
        c_rle_decode.hit();
        i32 n = 0;
        u8 b[4] = {0, 0, 0, 0};  // ebp-0xc, -0xb, -0xa
        for (;;) {
            if (joan::orig::as_file_mem(stream)->file_read(static_cast<void*>(&b[0]), joan::orig::arg::u32(i32{1})) == 0) return -1;                         // 0x00588a80
            if (static_cast<u32>(b[0]) == (static_cast<u32>(marker) & 0xffu)) {
                if (joan::orig::as_file_mem(stream)->file_read(static_cast<void*>(&b[1]), joan::orig::arg::u32(i32{1})) == 0) return -1;
                if (b[1] == 0) return n;
                if (joan::orig::as_file_mem(stream)->file_read(static_cast<void*>(&b[2]), joan::orig::arg::u32(i32{1})) == 0) return -1;
                memset_(out + n, static_cast<i32>(b[2]), static_cast<u32>(b[1]));
                n += static_cast<i32>(b[1]);
            } else {
                out[n] = b[0];
                n += 1;
            }
        }
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Rle::decode", 0x0059dc10, &H::rle_decode);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Rle::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_05::H::self_59d190>(this)); }  // 0x59d190
auto joan::orig::Rle::destruct() -> void { joan::bind::member<&::game_b91_03::H::empty>(this); }  // 0x59d1b0
auto joan::orig::Rle::decode(void* a0, void* a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_04::H::rle_decode>(this, a0, a1, a2)); }  // 0x59dc10
