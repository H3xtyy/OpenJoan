// game/unknown_vtxconv.cpp: functions whose original source file is unknown (formerly game/_unplaced/vtxconv.cpp;
// re/attribution.csv: between ww/en3d/vertex.cpp and ww/en3d/vga.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

struct H {
    // ------------------------------------------------------------------ 0x0060f020
    H* copy_24(const u8* src) {
        jo::self<jo::Vtx>(this)->pos_x = jo::as_vtx(src)->pos_x;
        jo::self<jo::Vtx>(this)->pos_y = jo::as_vtx(src)->pos_y;
        jo::self<jo::Vtx>(this)->pos_z = jo::as_vtx(src)->pos_z;
        jo::self<jo::Vtx>(this)->u32_c = jo::as_vtx(src)->u32_c;
        jo::self<jo::Vtx>(this)->u32_10 = jo::as_vtx(src)->u32_10;
        jo::self<jo::Vtx>(this)->u32_14 = jo::as_vtx(src)->u32_14;
        jo::self<jo::Vtx>(this)->u32_18 = jo::as_vtx(src)->u32_18;
        const u32 a = jo::as_vtx(src)->arr_1c;
        const u32 b = jo::as_vtx(src)->arr_20;
        jo::self<jo::Vtx>(this)->arr_1c = a;
        jo::self<jo::Vtx>(this)->arr_20 = b;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Obj::copy_24", 0x0060f020, &H::copy_24);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

struct H {
    // ------------------------------------------------------------------ 0x0060f0b0
    void* assign_24(const u8* src) {
        jo::self<jo::Vtx>(this)->pos_x = jo::as_vtx(src)->pos_x;
        jo::self<jo::Vtx>(this)->pos_y = jo::as_vtx(src)->pos_y;
        jo::self<jo::Vtx>(this)->pos_z = jo::as_vtx(src)->pos_z;
        jo::self<jo::Vtx>(this)->u32_c = jo::as_vtx(src)->u32_c;
        jo::self<jo::Vtx>(this)->u32_10 = jo::as_vtx(src)->u32_10;
        jo::self<jo::Vtx>(this)->u32_14 = jo::as_vtx(src)->u32_14;
        jo::self<jo::Vtx>(this)->u32_18 = jo::as_vtx(src)->u32_18;
        const u32 a = jo::as_vtx(src)->arr_1c;
        const u32 b = jo::as_vtx(src)->arr_20;
        jo::self<jo::Vtx>(this)->arr_1c = a;
        jo::self<jo::Vtx>(this)->arr_20 = b;
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Obj::assign_24", 0x0060f0b0, &H::assign_24);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

struct H {
    // ------------------------------------------------------------------------ 0x0060f140
    void* rec_assign(const u8* src) {
        jo::VertexF8* const from = jo::self<jo::VertexF8>(src);
        jo::self<jo::Vtx>(this)->pos_x = jo::view<u32>(from->pos[0]);
        jo::self<jo::Vtx>(this)->pos_y = jo::view<u32>(from->pos[1]);
        jo::self<jo::Vtx>(this)->pos_z = jo::view<u32>(from->pos[2]);
        jo::self<jo::Vtx>(this)->u32_c = jo::view<u32>(from->attr[1]);
        jo::self<jo::Vtx>(this)->u32_10 = jo::view<u32>(from->attr[2]);
        jo::self<jo::Vtx>(this)->u32_14 = jo::view<u32>(from->attr[3]);
        jo::self<jo::Vtx>(this)->u32_18 = jo::view<u32>(from->attr[4]);
        const u32 v20 = jo::view<u32>(from->attr[5]);
        const u32 v24 = jo::view<u32>(from->attr[6]);
        jo::self<jo::Vtx>(this)->arr_1c = v20;
        jo::self<jo::Vtx>(this)->arr_20 = v24;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Rec28::assign", 0x0060f140, &H::rec_assign);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

struct H {
    // ------------------------------------------------------------------ 0x0060f1d0
    H* copy_60f1d0(const u8* src) {
        jo::self<jo::Vtx>(this)->pos_x = jo::as_vtx(src)->pos_x;
        jo::self<jo::Vtx>(this)->pos_y = jo::as_vtx(src)->pos_y;
        jo::self<jo::Vtx>(this)->pos_z = jo::as_vtx(src)->pos_z;
        jo::self<jo::Vtx>(this)->u32_c = jo::as_vtx(src)->u32_c;
        jo::self<jo::Vtx>(this)->u32_10 = jo::as_vtx(src)->u32_10;
        jo::self<jo::Vtx>(this)->u32_14 = jo::as_vtx(src)->u32_14;
        jo::self<jo::Vtx>(this)->u32_18 = jo::as_vtx(src)->u32_18;
        for (i32 i = 0; i < 4; ++i) {
            const u32 a = jo::as_vtx(src)->arr_1c, b = jo::as_vtx(src)->arr_20;
            jo::view<u32>(jo::self<jo::VertexF2>(this)->attr[4 + i * 2]) = a;
            jo::view<u32>(jo::self<jo::VertexF2>(this)->attr[5 + i * 2]) = b;
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Obj::copy_60f1d0", 0x0060f1d0, &H::copy_60f1d0);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

struct H {
    // ------------------------------------------------------------------ 0x0060f280
    H* copy_60f280(const u8* src) {
        jo::self<jo::Vtx>(this)->pos_x = jo::as_vtx(src)->pos_x;
        jo::self<jo::Vtx>(this)->pos_y = jo::as_vtx(src)->pos_y;
        jo::self<jo::Vtx>(this)->pos_z = jo::as_vtx(src)->pos_z;
        jo::self<jo::Vtx>(this)->u32_c = jo::as_vtx(src)->u32_c;
        jo::self<jo::Vtx>(this)->u32_10 = jo::as_vtx(src)->u32_10;
        jo::self<jo::Vtx>(this)->u32_14 = jo::as_vtx(src)->u32_14;
        jo::self<jo::Vtx>(this)->u32_18 = jo::as_vtx(src)->u32_18;
        for (i32 i = 0; i < 4; ++i) {
            const u32 a = jo::as_vtx(src)->arr_1c;
            const u32 b = jo::as_vtx(src)->arr_20;
            jo::view<u32>(jo::self<jo::VertexF2>(this)->attr[i * 2 + 4]) = a;
            jo::view<u32>(jo::self<jo::VertexF2>(this)->attr[i * 2 + 5]) = b;
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Obj::copy_60f280", 0x0060f280, &H::copy_60f280);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
namespace {
namespace game_b93_03 {

struct H {
    void* vtx_from_24(const void* src) {  // 0x0060f330
        std::memcpy(this, src, 12);
        jo::self<jo::Vtx>(this)->u32_c = jo::view<u32>(jo::self<jo::VertexF1>(src)->attr[3]);
        jo::self<jo::Vtx>(this)->u32_10 = jo::view<u32>(jo::self<jo::VertexF1>(src)->attr[4]);
        jo::self<jo::Vtx>(this)->u32_14 = jo::view<u32>(jo::self<jo::VertexF1>(src)->attr[5]);
        return this;
    }
    void* vtx_from_28(const void* src) {  // 0x0060f4b0
        std::memcpy(this, src, 12);
        jo::self<jo::Vtx>(this)->u32_c = 0x3f800000u;
        jo::self<jo::Vtx>(this)->u32_10 = jo::view<u32>(jo::self<jo::VertexF8>(src)->attr[0]);
        jo::self<jo::Vtx>(this)->u32_14 = jo::view<u32>(jo::self<jo::VertexF8>(src)->attr[1]);
        jo::self<jo::Vtx>(this)->u32_18 = jo::view<u32>(jo::self<jo::VertexF8>(src)->attr[2]);
        jo::self<jo::Vtx>(this)->arr_1c = jo::view<u32>(jo::self<jo::VertexF8>(src)->attr[3]);
        jo::self<jo::Vtx>(this)->arr_20 = jo::view<u32>(jo::self<jo::VertexF8>(src)->attr[4]);
        jo::self<jo::Vtx>(this)->u32_24 = jo::view<u32>(jo::self<jo::VertexF8>(src)->attr[5]);
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Vtx::from_24", 0x0060f330, &H::vtx_from_24);
B93("Vtx::from_28", 0x0060f4b0, &H::vtx_from_28);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

struct H {
    // 0x0060f390
    void* assign_24(const std::uint32_t* src) {
        for (std::int32_t i = 0; i < 9; ++i) jo::view<std::uint32_t[9]>(*jo::self<jo::VertexF1>(this))[i] = src[i];
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Obj::assign_24", 0x0060f390, &H::assign_24);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

struct H {
    // 0x0060f420
    void* copy_24(const void* src) {
        for (int i = 0; i < 9; ++i) jo::view<std::uint32_t[9]>(*jo::self<jo::VertexF1>(this))[i] = jo::view<std::uint32_t[9]>(*jo::self<jo::VertexF1>(src))[i];
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Obj::copy_24", 0x0060f420, &H::copy_24);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::VertexFormat::from_24(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_03::H::vtx_from_24>(this, a0)); }  // 0x60f330
auto joan::orig::VertexFormat::from_28(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_03::H::vtx_from_28>(this, a0)); }  // 0x60f4b0
