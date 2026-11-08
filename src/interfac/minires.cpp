// interfac/minires.cpp: functions of the original d:\ww\interfac\minires.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cmath>
#include <limits>
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

}  // namespace

// ---------------------------------------- from modules/game_ui8.cpp (game/ui8)
//
//      - MiniRes::free 0x0071b080: frees +5, the 0xae-byte records of +0xd (count +9) and
//        +0x15 (count +0x11) with their +0xaa objects (0x006292c0 + deleting dtor 0x0071b330).
//        ret 0.
//
//      - MiniRes::find_frame 0x0071b3b0 (a, b, c) -> 32-byte record or null. ret 0xc.
namespace {
namespace game_ui8 {

constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kMiniResFile(d::s_d_ww_interfac_minires_cpp);  // "d:\ww\interfac\minires.cpp"

JOAN_COUNTER(c_mr_free, "MiniRes::free");
JOAN_COUNTER(c_mr_find, "MiniRes::find_frame");

// The records `list` (count `count`): MiniRes::maps / map_count or markers / marker_count.
void minires_free_records(jo::MiniResMap*& list, const std::int32_t& count, std::int32_t line) {
    for (std::int32_t i = 0; i < count; ++i) {
        if (list[i].overlay == nullptr) continue;
        joan::orig::as_overlay2d(list[i].overlay)->free_actor();  // 0x006292c0
        void* const item = list[i].overlay;
        if (item != nullptr) joan::orig::as_overlay2d(item)->destroy(1u);  // 0x0071b330 (deleting dtor)
        list[i].overlay = nullptr;
    }
    joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(list), joan::orig::arg::str(img::str(kMiniResFile)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a28940)) + line));  // 0x0058ed70
    list = nullptr;
}

struct H {
    // 0x0071b080. ret 0.
    void minires_free() {
        c_mr_free.hit();
        if (jo::self<jo::MiniRes>(this)->frames != nullptr) {
            joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::MiniRes>(this)->frames)), joan::orig::arg::str(img::str(kMiniResFile)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a28940)) + 3));  // 0x0058ed70
            jo::self<jo::MiniRes>(this)->frames = nullptr;
        }
        if (jo::self<jo::MiniRes>(this)->maps != nullptr) minires_free_records(jo::self<jo::MiniRes>(this)->maps, jo::self<jo::MiniRes>(this)->map_count, 0x14);
        if (jo::self<jo::MiniRes>(this)->markers != nullptr) minires_free_records(jo::self<jo::MiniRes>(this)->markers, jo::self<jo::MiniRes>(this)->marker_count, 0x25);
        jo::self<jo::MiniRes>(this)->loaded = 0;
    }

    // 0x0071b3b0. ret 0xc.
    std::uint8_t* minires_find(std::int16_t a, std::int16_t b, std::int16_t c) {
        c_mr_find.hit();
        if (static_cast<std::int32_t>(b) >= 1 && static_cast<std::int32_t>(c) >= 1) {
            for (std::int32_t i = 0; i < jo::self<jo::MiniRes>(this)->frame_count; ++i) {
                std::uint8_t* const r = reinterpret_cast<std::uint8_t*>(&jo::self<jo::MiniRes>(this)->frames[i]);
                if (static_cast<std::int32_t>(jo::as_mini_res_frame(r)->kind) == static_cast<std::int32_t>(a) &&
                    static_cast<std::int32_t>(jo::as_mini_res_frame(r)->sub_a) == static_cast<std::int32_t>(b) &&
                    static_cast<std::int32_t>(jo::as_mini_res_frame(r)->sub_b) == static_cast<std::int32_t>(c) &&
                    static_cast<std::int8_t>(jo::as_mini_res_frame(r)->is_default) == 0)
                    return r;
            }
        }
        for (std::int32_t i = 0; i < jo::self<jo::MiniRes>(this)->frame_count; ++i) {
            std::uint8_t* const r = reinterpret_cast<std::uint8_t*>(&jo::self<jo::MiniRes>(this)->frames[i]);
            if (static_cast<std::int32_t>(jo::as_mini_res_frame(r)->kind) == static_cast<std::int32_t>(a) &&
                static_cast<std::int8_t>(jo::as_mini_res_frame(r)->is_default) != 0)
                return r;
        }
        return nullptr;
    }
};

#define U8(name, addr, fn) JOAN_REPLACE("game/ui8", name, addr, fn)
U8("MiniRes::free", 0x0071b080, &H::minires_free);
U8("MiniRes::find_frame", 0x0071b3b0, &H::minires_find);
#undef U8

}  // namespace game_ui8
}  // namespace

// ---------------------------------------- from modules/game_b82_15.cpp (game/b82_15)
//
//      - MiniRes::read_frames 0x0071b6a0 (bmpres, id; ret 8): skipped when 0x00561320(id) says
//        so; switches the archive 0x00a56b18 to 'INTERFAC', copies the bitmap records except
//        TITLE / OBJ01 / OBJ02 into 32-byte frames +5 (count +1): name, 'z' kind +9, 'b' flag
//        +0x1f, 'r' row +0xb / 'c' column +0xd; maximum rows / columns per kind +0x19..+0x1f;
//        cell sizes +0x21..+0x2d from the mission record 0x00420240(id) +0x54 / +0x58 (_ftol,
//        idiv); per frame pixel rectangle +0xf..+0x1b; restores the archive.
//
//      - MiniRes::read_maps 0x0071bf10 (id; ret 4, SEH frame, normal path only): reads the
//        'MAP\<dir><name>.MAP' files of the mission record into 0xae-byte records +0xd (count
//        +9, +0x11), adds the 'Cursor' record and builds a textured quad mesh + controller per
//        record (+0xa6 / +0xaa).
//
//      - MiniRes::MiniRes 0x0071aff0, MiniRes::load 0x0071b020 (ret 8), MiniRes::free 0x0071b080,
//        MiniRes::find_frame 0x0071b3b0 (ret 0xc), MiniResItem::`scalar deleting dtor'
//        0x0071b330 (ret 4).
namespace {
namespace game_b82_15 {

using joan::call::cdecl_call;
using joan::img::str;
using u32 = std::uint32_t;
std::int32_t mul32(std::int32_t a, std::int32_t b) {  // imul (wrapping)
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b));
}

constexpr std::uint32_t kOne = 0x3f800000u;   // 1.0f

// CRT
constexpr fn::Code kMemset = fn::crt_memset;
constexpr fn::Code kMemcpy = fn::crt_memcpy_7b2a20;
constexpr fn::Code kNew = fn::op_new;
constexpr fn::Code kDelete = fn::op_delete;
constexpr fn::Code kStrncpy = fn::crt_strncpy;
constexpr fn::Code kStrcpy = fn::crt_strcpy;
constexpr fn::Code kStrrchr = fn::crt_strrchr;
constexpr fn::Code kStrlen = fn::crt_strlen;
constexpr fn::Code kStrcmp = fn::crt_stricmp;
constexpr fn::Code kSqrt = fn::crt_sqrt_7b2344;

void* const kMemory = obj(d::g_mem);
void* const kTimer = obj(d::g_misc);
void* const kArchive = obj(d::g_game_set);
void* const kMissions = obj(d::g_a45f08);
void* const kSound = obj(d::g_texture_array);
const char* const kMiniResFile = str(d::s_d_ww_interfac_minires_cpp);  // "d:\ww\interfac\minires.cpp"
const char* const kInterfac = str(d::s_INTERFAC);     // "INTERFAC"

JOAN_COUNTER(c_frames, "MiniRes::read_frames");
JOAN_COUNTER(c_maps, "MiniRes::read_maps");

void archive_restore(std::int16_t previous) {
    joan::orig::as_game_set(kArchive)->close_set_58b140();                                       // 0x0058b140
    joan::orig::as_game_set(kArchive)->open_set_58b080(static_cast<std::int32_t>(previous));  // 0x0058b080
}

std::uint8_t* mission(std::int16_t id) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(kMissions)->get_420240(static_cast<std::int32_t>(id)));  // 0x00420240
}

// Local objects of 0x0071bf10: two 0x191-byte strings and the 0x11c-byte file reader.
struct alignas(4) Str {
    char b[0x194];
};
struct alignas(alignof(void*)) FileObj {  // a FileMem: 0x11c bytes on x86, 0x12c on x64
    std::uint8_t b[((sizeof(joan::orig::FileMem) > 0x11c ? sizeof(joan::orig::FileMem) : 0x11c) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};
struct Vertex {
    std::uint32_t x, y, z, a, b, w, c, u, v;
};
static_assert(sizeof(Vertex) == 0x24);

struct H {
    // 0x0071b330. ret 4.
    void* item_delete(std::uint32_t flags) {
        joan::orig::as_overlay2d(this)->release();  // 0x00628ea0
        if ((flags & 1u) != 0) cdecl_call(kDelete, static_cast<void*>(this));
        return this;
    }

    // strncpy(local, strrchr(rec, ch) + 1, n) and 0x00590270(local, n).
    static std::int32_t field(std::uint8_t* rec, std::int32_t ch, std::uint32_t n, char* buf) {
        char* p = cdecl_call<char*>(kStrrchr, static_cast<void*>(rec), ch);
        p = p + 1;
        cdecl_call<char*>(kStrncpy, buf, static_cast<const char*>(p), n);
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(kTimer)->atoi_field_590270(joan::orig::arg::str(static_cast<void*>(buf)), joan::orig::arg::u32(n)));  // 0x00590270
    }

    // 0x0071b6a0. ret 8.
    void minires_read_frames(std::uint8_t* res, std::int16_t id) {
        c_frames.hit();
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(kMissions)->empty_at_561320(static_cast<std::int32_t>(id))) != 0) return;  // 0x00561320
        std::int16_t& arch = global<std::int16_t>(d::g_a6a5c4);
        if (static_cast<std::int32_t>(arch) == 0) {
            arch = joan::orig::arg::as<std::int16_t>(joan::orig::as_game_set(kArchive)->find_set_58b250(joan::orig::arg::str(kInterfac)));  // 0x0058b250
        }
        const std::int16_t previous = global<std::int16_t>(d::g_a56b19);
        if (static_cast<std::int32_t>(arch) != static_cast<std::int32_t>(previous)) {
            joan::orig::as_game_set(kArchive)->close_set_58b140();                                    // 0x0058b140
            joan::orig::as_game_set(kArchive)->open_set_58b080(static_cast<std::int32_t>(arch));  // 0x0058b080
        }
        jo::view<std::uint32_t>(jo::self<jo::MiniRes>(this)->frame_count) = jo::view<std::uint32_t>(jo::as_bmp_res(res)->count);
        for (std::int32_t i = 0; i < jo::as_bmp_res(res)->count; ++i) {
            if (cdecl_call<std::int32_t>(kStrcmp, static_cast<void*>(jo::as_bmp_res(res)->entries[i].name), str(d::s_TITLE)) == 0 ||
                cdecl_call<std::int32_t>(kStrcmp, static_cast<void*>(jo::as_bmp_res(res)->entries[i].name), str(d::s_OBJ01)) == 0 ||
                cdecl_call<std::int32_t>(kStrcmp, static_cast<void*>(jo::as_bmp_res(res)->entries[i].name), str(d::s_OBJ02)) == 0) {
                jo::self<jo::MiniRes>(this)->frame_count = jo::self<jo::MiniRes>(this)->frame_count - 1;
            }
        }
        jo::view<void*>(jo::self<jo::MiniRes>(this)->frames) = joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::MiniRes>(this)->frame_count) * static_cast<std::uint32_t>(sizeof(jo::MiniResFrame))), joan::orig::arg::str(kMiniResFile), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a28944)) + 0x1e));  // 0x0058e7b0
        cdecl_call<void*>(kMemset, jo::view<void*>(jo::self<jo::MiniRes>(this)->frames), 0, jo::view<std::uint32_t>(jo::self<jo::MiniRes>(this)->frame_count) * static_cast<std::uint32_t>(sizeof(jo::MiniResFrame)));
        jo::self<jo::MiniRes>(this)->rect[0] = 0;
        jo::self<jo::MiniRes>(this)->rect[1] = 0;
        jo::self<jo::MiniRes>(this)->rect[2] = 0;
        jo::self<jo::MiniRes>(this)->rect[3] = 0;
        std::int16_t index = 0;
        for (std::int32_t i = 0; i < jo::as_bmp_res(res)->count; ++i) {
            std::uint8_t* const rec = reinterpret_cast<std::uint8_t*>(&jo::self<jo::MiniRes>(this)->frames[index]);
            std::uint8_t* const name = reinterpret_cast<std::uint8_t*>(jo::as_bmp_res(res)->entries[i].name);
            if (cdecl_call<std::int32_t>(kStrcmp, static_cast<void*>(name), str(d::s_TITLE)) == 0) continue;  // 'TITLE'
            if (cdecl_call<std::int32_t>(kStrcmp, static_cast<void*>(name), str(d::s_OBJ01)) == 0) continue;  // 'OBJ01'
            if (cdecl_call<std::int32_t>(kStrcmp, static_cast<void*>(name), str(d::s_OBJ02)) == 0) continue;  // 'OBJ02'
            index = static_cast<std::int16_t>(index + 1);
            joan::orig::as_misc(kTimer)->copy_field_590210(joan::orig::arg::str(static_cast<void*>(rec)), joan::orig::arg::str(static_cast<void*>(name)), 8);  // 0x00590210
            char buf1[4] = {};
            jo::as_mini_res_frame(rec)->kind = static_cast<std::int16_t>(field(rec, 0x7a, 1u, buf1));  // 'z'
            if (cdecl_call<char*>(kStrrchr, static_cast<void*>(rec), 0x62) != nullptr) {    // 'b'
                jo::as_mini_res_frame(rec)->is_default = 1;
                continue;
            }
            char buf2[4] = {};
            jo::as_mini_res_frame(rec)->sub_a = static_cast<std::int16_t>(field(rec, 0x72, 2u, buf2));  // 'r'
            if (static_cast<std::int32_t>(jo::as_mini_res_frame(rec)->kind) == 0 &&
                static_cast<std::int32_t>(jo::as_mini_res_frame(rec)->sub_a) > static_cast<std::int32_t>(jo::self<jo::MiniRes>(this)->rect[0])) {
                jo::self<jo::MiniRes>(this)->rect[0] = jo::as_mini_res_frame(rec)->sub_a;
            } else if (static_cast<std::int32_t>(jo::as_mini_res_frame(rec)->kind) == 1 &&
                       static_cast<std::int32_t>(jo::as_mini_res_frame(rec)->sub_a) > static_cast<std::int32_t>(jo::self<jo::MiniRes>(this)->rect[2])) {
                jo::self<jo::MiniRes>(this)->rect[2] = jo::as_mini_res_frame(rec)->sub_a;
            }
            jo::as_mini_res_frame(rec)->sub_b = static_cast<std::int16_t>(field(rec, 0x63, 2u, buf2));  // 'c'
            if (static_cast<std::int32_t>(jo::as_mini_res_frame(rec)->kind) == 0 &&
                static_cast<std::int32_t>(jo::as_mini_res_frame(rec)->sub_b) > static_cast<std::int32_t>(jo::self<jo::MiniRes>(this)->rect[1])) {
                jo::self<jo::MiniRes>(this)->rect[1] = jo::as_mini_res_frame(rec)->sub_b;
            } else if (static_cast<std::int32_t>(jo::as_mini_res_frame(rec)->kind) == 1 &&
                       static_cast<std::int32_t>(jo::as_mini_res_frame(rec)->sub_b) > static_cast<std::int32_t>(jo::self<jo::MiniRes>(this)->rect[3])) {
                jo::self<jo::MiniRes>(this)->rect[3] = jo::as_mini_res_frame(rec)->sub_b;
            }
            jo::as_mini_res_frame(rec)->is_default = 0;
        }
        {
            const std::int32_t v = joan::hook::x87::ftol(jo::as_world(mission(id))->bounds[3]);
            jo::self<jo::MiniRes>(this)->i32_21 = v / static_cast<std::int32_t>(jo::self<jo::MiniRes>(this)->rect[1]);
        }
        {
            const std::int32_t v = joan::hook::x87::ftol(jo::as_world(mission(id))->bounds[4]);
            jo::self<jo::MiniRes>(this)->i32_25 = v / static_cast<std::int32_t>(jo::self<jo::MiniRes>(this)->rect[0]);
        }
        {
            const std::int32_t v = joan::hook::x87::ftol(jo::as_world(mission(id))->bounds[3]);
            jo::self<jo::MiniRes>(this)->i32_29 = v / static_cast<std::int32_t>(jo::self<jo::MiniRes>(this)->rect[3]);
        }
        {
            const std::int32_t v = joan::hook::x87::ftol(jo::as_world(mission(id))->bounds[4]);
            jo::self<jo::MiniRes>(this)->i32_2d = v / static_cast<std::int32_t>(jo::self<jo::MiniRes>(this)->rect[2]);
        }
        for (std::int32_t i = 0; i < jo::self<jo::MiniRes>(this)->frame_count; ++i) {
            std::uint8_t* const rec = reinterpret_cast<std::uint8_t*>(&jo::self<jo::MiniRes>(this)->frames[i]);
            if (static_cast<std::int8_t>(jo::as_mini_res_frame(rec)->is_default) != 0) continue;
            const std::int32_t* cw;  // cell width / height and row count of the frame's kind
            const std::int32_t* ch;
            const std::int16_t* rows;
            if (static_cast<std::int32_t>(jo::as_mini_res_frame(rec)->kind) == 0) {
                cw = &jo::self<jo::MiniRes>(this)->i32_21;
                ch = &jo::self<jo::MiniRes>(this)->i32_25;
                rows = &jo::self<jo::MiniRes>(this)->rect[0];
            } else if (static_cast<std::int32_t>(jo::as_mini_res_frame(rec)->kind) == 1) {
                cw = &jo::self<jo::MiniRes>(this)->i32_29;
                ch = &jo::self<jo::MiniRes>(this)->i32_2d;
                rows = &jo::self<jo::MiniRes>(this)->rect[2];
            } else {
                continue;
            }
            const std::int32_t col = jo::as_mini_res_frame(rec)->sub_b;
            const std::int32_t row = jo::as_mini_res_frame(rec)->sub_a;
            const std::int32_t nrows = *rows;
            jo::as_mini_res_frame(rec)->x0 = mul32(*cw, col - 1);
            jo::as_mini_res_frame(rec)->y0 = mul32(*ch, nrows - (row - 1));
            const std::int32_t x2 = static_cast<std::int32_t>(static_cast<std::uint32_t>(mul32(*cw, col)) - 1u);
            jo::as_mini_res_frame(rec)->x1 = x2 < 0 ? 0 : x2;
            const std::int32_t y2 = static_cast<std::int32_t>(static_cast<std::uint32_t>(mul32(*ch, nrows - row)) - 1u);
            jo::as_mini_res_frame(rec)->y1 = y2 < 0 ? 0 : y2;
        }
        if (static_cast<std::int32_t>(arch) != static_cast<std::int32_t>(previous)) archive_restore(previous);
    }

    // Builds 'MAP\' + dir + name + '.MAP' in s and opens it with the reader.
    static void open_map(Str& s, Str& dir, FileObj& file, std::int16_t id, std::int32_t m) {
        joan::orig::as_string(&s)->assign(str(d::s_MAP_9cf818));     // 0x005a1440 'MAP\'
        joan::orig::as_string(&s)->append_str(joan::orig::as_string(&dir));  // 0x005a14f0
        std::uint8_t* const r = mission(id);
        joan::orig::as_string(&s)->append(joan::orig::arg::str((&jo::as_world(r)->cells->item)[m]->name));  // 0x005a1550 (the map name)
        joan::orig::as_string(&s)->append(str(d::s_MAP_9dee60));                                                    // '.MAP'
        char* const name = joan::orig::as_string(&s)->c_str();          // 0x00417d90
        joan::orig::as_file_mem(&file)->open_588000(joan::orig::arg::str(name), 1, static_cast<char>(joan::orig::arg::i32(1)));                                               // 0x00588000
    }

    // 0x0071bf10. ret 4.
    void minires_read_maps(std::int16_t id) {
        c_maps.hit();
        const joan::hook::x87::Fpu fpu{true};
        Str path;
        FileObj file;
        Str dir;
        joan::orig::as_string(&path)->construct();  // 0x005a1170
        joan::orig::as_file_mem(&file)->construct();  // 0x00587f80
        {
            char* const tail = cdecl_call<char*>(kStrrchr, static_cast<void*>(jo::as_world(mission(id))->name), 0x5c);
            joan::orig::as_string(&dir)->construct(joan::orig::arg::str(static_cast<const char*>(tail)));  // 0x005a11a0
        }
        {
            std::uint32_t n = cdecl_call<std::uint32_t>(kStrlen, static_cast<void*>(jo::as_world(mission(id))->name));
            n -= cdecl_call<std::uint32_t>(kStrlen, static_cast<void*>(joan::orig::as_string(&dir)->c_str()));
            n += 1;
            std::uint8_t* const r = mission(id);
            char* const d = joan::orig::as_string(&dir)->c_str();
            cdecl_call<char*>(kStrncpy, d, static_cast<const char*>(jo::as_world(r)->name), n);
        }
        for (std::int32_t m = 0; m < jo::as_world(mission(id))->cell_count; ++m) {
            open_map(path, dir, file, id, m);
            jo::self<jo::MiniRes>(this)->map_count = jo::self<jo::MiniRes>(this)->map_count + joan::orig::as_file_mem(&file)->file_get_long();  // 0x005893c0
            jo::self<jo::MiniRes>(this)->marker_count = jo::self<jo::MiniRes>(this)->map_count;
            joan::orig::as_file_mem(&file)->close();  // 0x005886d0
        }
        jo::self<jo::MiniRes>(this)->map_count = jo::self<jo::MiniRes>(this)->map_count + 1;
        {
            const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_a28948)) + 0x17;
            jo::view<void*>(jo::self<jo::MiniRes>(this)->maps) = joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(static_cast<std::uint32_t>(mul32(jo::self<jo::MiniRes>(this)->map_count, static_cast<std::int32_t>(sizeof(jo::MiniResMap))))), joan::orig::arg::str(kMiniResFile), joan::orig::arg::i32(line));  // 0x0058e7b0
        }
        cdecl_call<void*>(kMemset, jo::view<void*>(jo::self<jo::MiniRes>(this)->maps), 0, static_cast<std::uint32_t>(mul32(jo::self<jo::MiniRes>(this)->map_count, static_cast<std::int32_t>(sizeof(jo::MiniResMap)))));
        std::int16_t index = 0;
        for (std::int32_t m = 0; m < jo::as_world(mission(id))->cell_count; ++m) {
            open_map(path, dir, file, id, m);
            const std::int32_t count = joan::orig::as_file_mem(&file)->file_get_long();  // 0x005893c0
            for (std::int32_t j = 0; j < count; ++j) {
                auto rec = [&]() { return &jo::self<jo::MiniRes>(this)->maps[index]; };
                joan::orig::as_file_mem(&file)->file_read(joan::orig::arg::mptr(static_cast<void*>(rec()->name)), 0x40u);         // 0x00588a80
                joan::orig::as_file_mem(&file)->file_read(joan::orig::arg::mptr(static_cast<void*>(rec()->icon)), 8u);
                {
                    const std::int32_t v = joan::orig::as_file_mem(&file)->file_get_long();
                    rec()->kind = static_cast<std::int16_t>(v);
                }
                joan::orig::as_file_mem(&file)->file_read(joan::orig::arg::mptr(static_cast<void*>(rec()->unit_name)), 0x40u);
                {
                    const float a = static_cast<float>(joan::orig::as_file_mem(&file)->file_get_long());
                    const std::int32_t base = (&jo::as_world(mission(id))->cells->item)[m]->origin_x;
                    rec()->pos[0] = static_cast<float>(fpu.add(static_cast<double>(base), static_cast<double>(a)));
                }
                {
                    const float a = static_cast<float>(joan::orig::as_file_mem(&file)->file_get_long());
                    const std::int32_t base = (&jo::as_world(mission(id))->cells->item)[m]->origin_y;
                    rec()->pos[1] = static_cast<float>(fpu.add(static_cast<double>(base), static_cast<double>(a)));
                }
                {
                    const std::int32_t v = joan::orig::as_file_mem(&file)->file_get_long();
                    rec()->pos[2] = static_cast<float>(v);
                }
                {
                    const std::int32_t v = joan::orig::as_file_mem(&file)->file_get_long();
                    rec()->f32_97 = static_cast<float>(v);
                }
                joan::orig::as_file_mem(&file)->allow_vary_size = 0;  // +0x105
                joan::orig::as_file_mem(&file)->file_read(joan::orig::arg::mptr(static_cast<void*>(&rec()->no_mesh)), 1u);
                joan::orig::as_file_mem(&file)->file_read(joan::orig::arg::mptr(static_cast<void*>(&rec()->u8_a1)), 1u);
                joan::orig::as_file_mem(&file)->allow_vary_size = 1;
                index = static_cast<std::int16_t>(index + 1);
            }
            joan::orig::as_file_mem(&file)->close();  // 0x005886d0
        }
        {
            const std::int32_t last = jo::self<jo::MiniRes>(this)->map_count - 1;
            jo::MiniResMap* const rec = &jo::self<jo::MiniRes>(this)->maps[last];
            cdecl_call<char*>(kStrncpy, static_cast<void*>(rec->name), str(d::s_Cursor), 0x40u);        // 'Cursor'
            cdecl_call<char*>(kStrncpy, static_cast<void*>(rec->icon), str(d::s_CURSOR), 8u);   // 'CURSOR'
            rec->kind = 2;
            cdecl_call<char*>(kStrcpy, static_cast<void*>(rec->unit_name), str(d::s_Main_Character));        // 'Main Character'
            jo::view<std::uint32_t>(rec->pos[0]) = global<std::uint32_t>(d::g_pos);
            jo::view<std::uint32_t>(rec->pos[1]) = global<std::uint32_t>(d::g_a69cdc);
            jo::view<std::uint32_t>(rec->pos[2]) = global<std::uint32_t>(d::g_a69ce0);
            jo::view<std::uint32_t>(rec->f32_97) = 0;
        }
        for (std::int32_t i = 0; i < jo::self<jo::MiniRes>(this)->map_count; ++i) {
            jo::MiniResMap* const rec = &jo::self<jo::MiniRes>(this)->maps[i];
            rec->enabled = 1;
            rec->u32_a2 = 0;
            if (static_cast<std::int8_t>(rec->no_mesh) != 0) continue;
            std::uint8_t* const bmp = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_bmp_res(obj(d::g_heroIcons))->find(joan::orig::arg::str(static_cast<void*>(rec->icon))));  // 0x00620b40
            void* mesh = nullptr;
            {
                void* const mem = cdecl_call<void*>(kNew, static_cast<u32>(sizeof(jo::Plane)));  // 0x11c on x86
                mesh = mem != nullptr ? joan::orig::as_plane(mem)->construct(1u, 0u) : nullptr;  // 0x005cbaf0
            }
            joan::orig::as_plane(mesh)->init(0, 0, 0, 1u);  // 0x005cc7a0
            {
                void* const mem = cdecl_call<void*>(kNew, static_cast<u32>(sizeof(jo::SceneObject)));  // 0x10c on x86
                jo::view<void*>(rec->object) = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
            }
            {
                void* const mem = cdecl_call<void*>(kNew, static_cast<u32>(sizeof(jo::ActorDef)));  // 0x2d8 on x86
                void* const actor = mem != nullptr ? joan::orig::as_actor_def(mem)->construct() : nullptr;  // 0x005af390
                jo::view<void*>(jo::as_scene_object(rec->object)->def) = actor;
            }
            joan::orig::as_actor_def(jo::view<void*>(jo::as_scene_object(rec->object)->def))->create(joan::orig::as_plane(mesh), 0u);  // 0x005b02f0
            jo::as_scene_object(rec->object)->anim = nullptr;
            joan::orig::as_scene_object(rec->object)->init();  // 0x005aab70
            std::uint8_t* const plane = jo::view<std::uint8_t*>(jo::as_scene_object(rec->object)->plane);
            const std::int32_t mat = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_materials(1));  // 0x005cd470
            jo::Material* const material = &jo::as_plane(plane)->materials[mat];
            const std::int32_t mode = joan::orig::arg::as<std::int32_t>(joan::orig::as_texture_manager(kSound)->mode());  // 0x0060a720
            joan::orig::as_texture_manager(kSound)->set_mode(1u);                                          // 0x0060a740
            std::uint8_t* const tex = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_texture_manager(kSound)->load(joan::orig::arg::str(static_cast<void*>(jo::as_bmp_entry(bmp)->file)), 1, 0u));  // 0x006099c0
            joan::orig::as_texture_manager(kSound)->set_mode(joan::orig::arg::u32(mode));
            joan::orig::as_material(material)->construct();  // 0x005d52a0
            {
                std::uint8_t rgba[4] = {0xf0, 0xf0, 0xf0, 0xff};
                joan::orig::as_material(material)->set_colour0(joan::orig::arg::mptr(static_cast<void*>(rgba)));  // 0x005d5540
            }
            joan::orig::as_material(material)->set_texture(0, joan::orig::arg::mptr(static_cast<void*>(tex)));  // 0x005d5650
            jo::as_material(material)->blend = 3;
            jo::as_material(material)->texture_mode = 0x10002;
            const std::uint32_t z = 0;
            const float w = static_cast<float>(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_bmp_entry(bmp)->width)));
            const float h = static_cast<float>(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_bmp_entry(bmp)->height)));
            const float two = global<float>(d::flt_2);  // 2.0
            if (static_cast<std::int32_t>(rec->kind) == 0) {
                rec->radius = w / two - global<float>(d::flt_3);  // 3.0
            } else {
                const float sum = w * w + h * h;
                const double root = cdecl_call<double>(kSqrt, static_cast<double>(sum));  // CRT sqrt
                rec->radius = static_cast<float>(fpu.div(root, static_cast<double>(two)));
            }
            const float tw = static_cast<float>(jo::view<std::int32_t>(jo::as_texture(tex)->width) - 1);
            const float th = static_cast<float>(jo::view<std::int32_t>(jo::as_texture(tex)->height) - 1);
            const float u0 = static_cast<float>(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_bmp_entry(bmp)->x0))) / tw;
            const float v0 = static_cast<float>(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_bmp_entry(bmp)->y0))) / th;
            const float u1 = static_cast<float>(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_bmp_entry(bmp)->x1))) / tw;
            const float v1 = static_cast<float>(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_bmp_entry(bmp)->y1))) / th;
            joan::orig::texture_ref::release(joan::orig::arg::mptr(static_cast<void*>(tex)));  // 0x0042cef0
            auto bits = [](float v) {
                std::uint32_t b = 0;
                std::memcpy(&b, &v, 4);
                return b;
            };
            const std::uint32_t xl = bits(-w / two), xr = bits(w / two);
            const std::uint32_t yt = bits(-h / two), yb = bits(h / two);
            constexpr std::uint32_t kCol = 0x33f0f0f0u;  // bytes f0 f0 f0 33
            Vertex quad[4] = {
                {xl, yt, z, 0u, 0u, kOne, kCol, bits(u0), bits(v0)},
                {xr, yt, z, 0u, 0u, kOne, kCol, bits(u0), bits(v1)},
                {xl, yb, z, 0u, 0u, kOne, kCol, bits(u1), bits(v0)},
                {xr, yb, z, 0u, 0u, kOne, kCol, bits(u1), bits(v1)},
            };
            const std::uint16_t order[6] = {0, 2, 1, 1, 2, 3};
            const std::int32_t first = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_vertices(4));  // 0x005cd7f0
            cdecl_call<void*>(kMemcpy, static_cast<void*>(&jo::as_plane(plane)->vertices_f1[first]),
                              static_cast<const void*>(quad), static_cast<std::uint32_t>(sizeof quad));  // 0x90
            const std::int32_t tri = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_triangles(2));  // 0x005ce6f0
            for (std::int32_t k = 0; k < 6; ++k) {
                jo::view<std::uint16_t*>(jo::as_plane(plane)->triangles)[mul32(tri, 3) + k] =
                    static_cast<std::uint16_t>(first + static_cast<std::int32_t>(order[k]));
            }
            for (std::int32_t k = 0; k < 2; ++k) {
                jo::view<std::uint16_t*>(jo::as_plane(plane)->triangle_materials)[tri + k] = static_cast<std::uint16_t>(mat);
            }
            joan::orig::as_plane(plane)->set_format(4);   // 0x005cdc70
            joan::orig::as_plane(plane)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
            joan::orig::as_plane(plane)->sort_by_material(1);   // 0x005cf1d0
            {
                void* const mem = cdecl_call<void*>(kNew, 4u);
                jo::view<void*>(jo::as_mini_res_map(rec)->overlay) = mem != nullptr ? joan::orig::as_overlay2d(mem)->construct() : nullptr;  // 0x00628e70
            }
            joan::orig::as_overlay2d(jo::view<void*>(jo::as_mini_res_map(rec)->overlay))->attach(joan::orig::arg::mptr(rec->object));  // 0x00629230
        }
        joan::orig::as_file_mem(&file)->destruct();  // 0x00587fd0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b82_15", name, addr, fn)
B("MiniRes::read_maps", 0x0071bf10, &H::minires_read_maps);
B("MiniRes::read_frames", 0x0071b6a0, &H::minires_read_frames);
B("MiniResItem::scalar_deleting_dtor", 0x0071b330, &H::item_delete);
#undef B

}  // namespace game_b82_15
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
//
//    - MiniRes::slot_ptr 0x0071b380 (i), ret 4: [this+5] + i * 32 - 32 (no references - dead code).
namespace {
namespace game_b103_16 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

struct H {
    std::uint8_t* slot_ptr(i32 i) { return reinterpret_cast<std::uint8_t*>(&jo::self<jo::MiniRes>(this)->frames[i - 1]); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("MiniRes::slot_ptr_71b380", 0x0071b380, &H::slot_ptr);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
//
//      0x0071b520 Zones::find(short id, x, y, -)                thiscall, ret 0x10
namespace {
namespace game_b92_06 {

JOAN_COUNTER(c_zones, "Zones::find");

struct H {
    // 0x0071b520
    std::uint8_t* zones_find(std::int16_t id, std::int32_t x, std::int32_t y, std::int32_t) {
        c_zones.hit();
        for (std::int32_t i = 0; i < jo::self<jo::MiniRes>(this)->frame_count; ++i) {
            std::uint8_t* const e = reinterpret_cast<std::uint8_t*>(&jo::self<jo::MiniRes>(this)->frames[i]);
            if (jo::view<std::int8_t>(jo::as_mini_res_frame(e)->is_default) != 0) continue;
            if (static_cast<std::int32_t>(jo::as_mini_res_frame(e)->kind) != static_cast<std::int32_t>(id)) continue;
            if (jo::as_mini_res_frame(e)->x0 > x) continue;
            if (jo::as_mini_res_frame(e)->x1 < x) continue;
            if (jo::as_mini_res_frame(e)->y0 < y) continue;
            if (jo::as_mini_res_frame(e)->y1 > y) continue;
            return e;
        }
        for (std::int32_t i = 0; i < jo::self<jo::MiniRes>(this)->frame_count; ++i) {
            std::uint8_t* const e = reinterpret_cast<std::uint8_t*>(&jo::self<jo::MiniRes>(this)->frames[i]);
            if (static_cast<std::int32_t>(jo::as_mini_res_frame(e)->kind) != static_cast<std::int32_t>(id)) continue;
            if (jo::view<std::int8_t>(jo::as_mini_res_frame(e)->is_default) == 0) continue;
            return e;
        }
        return nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Zones::find", 0x0071b520, &H::zones_find);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
//
//    - repeat_step 0x006e5e00, get_d32 0x006e7b50, menu reset 0x006f21c0, menu slide-out
//      0x006f26e0, Triple::clear 0x006f5e90, Screen::release 0x0070e3c0, screen offset
//      0x0070fa70, Marker::update 0x00714040, Marker::visible 0x00717650, set_90 0x007563f0.
namespace {
namespace game_b90_06 {

using joan::call::thiscall;
using joan::call::cdecl_call;

constexpr img::Addr kTextures(d::g_texture_array);

JOAN_COUNTER(c_release, "Screen::release");

struct H {
    // 0x0070e3c0 Screen::release: child (+4), buffer (+0x478), texture (+0x47c).
    std::uintptr_t screen_release() {
        c_release.hit();
        if (jo::self<jo::MiniMap>(this)->view != nullptr) {
            joan::orig::as_mini_res(jo::view<void*>(jo::self<jo::MiniMap>(this)->view))->free();  // 0x0071b080
            void* const child = jo::view<void*>(jo::self<jo::MiniMap>(this)->view);
            if (child != nullptr) joan::orig::as_mini_res(child)->destroy(1u);  // 0x0070e4b0 deleting dtor
            jo::self<jo::MiniMap>(this)->view = nullptr;
        }
        std::uintptr_t r = reinterpret_cast<std::uintptr_t>(this);
        if (jo::self<jo::MiniMap>(this)->fog != nullptr) {
            joan::orig::operator_(jo::view<void*>(jo::self<jo::MiniMap>(this)->fog));  // operator delete
            r = 0;  // was operator delete's EAX: every caller of 0x0070e3c0 ignores the result
            jo::self<jo::MiniMap>(this)->fog = nullptr;
        }
        if (jo::self<jo::MiniMap>(this)->texture != nullptr) {
            joan::orig::as_texture_manager(img::obj(kTextures))->release(joan::orig::arg::mptr(jo::self<jo::MiniMap>(this)->texture));  // 0x0060a550
            jo::self<jo::MiniMap>(this)->texture = nullptr;
            r = reinterpret_cast<std::uintptr_t>(this);
        }
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Screen::release", 0x0070e3c0, &H::screen_release);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_b82_15.cpp (game/b82_15)
//
//      - PanelView::`scalar deleting dtor' 0x0070e4b0 (ret 4), PanelView::~PanelView 0x00715d20,
//        Panel::clear_unit 0x0070f9b0, Panel::set_flag_e 0x006e0dc0 (ret 4), Panel::set_slot
//        0x00718010 (ret 0x18), Panel::refresh_buttons 0x006e39a0.
namespace {
namespace game_b82_15_2 {

using joan::call::thiscall;

struct H {
    // 0x00715d20. ret 0.
    void view_dtor() { joan::orig::as_mini_res(this)->free(); }  // 0x0071b080
};

#define B(name, addr, fn) JOAN_REPLACE("game/b82_15", name, addr, fn)
B("PanelView::~PanelView", 0x00715d20, &H::view_dtor);
#undef B

}  // namespace game_b82_15_2
}  // namespace

// ---------------------------------------- from modules/game_ui2.cpp (game/ui2)
//
//      - Panel::set_unit 0x00715d50 (id): drops the old view +4 (0x0071b080 + deleting dtor),
//        then either 0x0070f9b0 (id rejected by 0x00561320) or builds a new 0x31-byte view
//        for the unit, reads its size (+0xa / +0xc), resets 30 slots (0x00718010) and the
//        flags +0x2f8..; finally clears +0x64[30], +0x2b, +0x11. ret 4.
namespace {
namespace game_ui2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12);

constexpr img::Addr kUnitsDb(d::g_a45f08);

JOAN_COUNTER(c_set_unit, "Panel::set_unit");

struct H {
    // 0x00715d50 Panel::set_unit. The original's SEH frame only frees the `new` if the view
    // constructor throws. ret 4.
    void set_unit(std::int16_t id) {
        c_set_unit.hit();
        if (jo::view<void*>(jo::self<jo::MiniMap>(this)->view) != nullptr) {
            joan::orig::as_mini_res(jo::view<void*>(jo::self<jo::MiniMap>(this)->view))->free();  // 0x0071b080
            void* const view = jo::view<void*>(jo::self<jo::MiniMap>(this)->view);
            if (view != nullptr) joan::orig::as_mini_res(view)->destroy(1u);  // 0x0070e4b0 (deleting dtor)
            jo::view<void*>(jo::self<jo::MiniMap>(this)->view) = nullptr;
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(img::obj(kUnitsDb))->empty_at_561320(static_cast<std::int32_t>(id))) != 0) {  // 0x00561320
            joan::orig::as_panel(this)->clear_unit();                                                         // 0x0070f9b0
            jo::self<jo::MiniMap>(this)->x = 0;
        } else {
            joan::orig::as_panel(this)->set_flag_e(static_cast<std::uint8_t>(joan::orig::arg::u32(1)));                             // 0x006e0dc0
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::MiniRes)));    // 0x31 on x86; operator new
            void* const view = mem != nullptr ? joan::orig::as_mini_res(mem)->construct() : nullptr;  // 0x0071aff0
            jo::view<void*>(jo::self<jo::MiniMap>(this)->view) = view;
            std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(img::obj(kUnitsDb))->get_420240(static_cast<std::int32_t>(id)));  // 0x00420240
            joan::orig::as_mini_res(jo::view<void*>(jo::self<jo::MiniMap>(this)->view))->load(joan::orig::arg::mptr(&jo::as_world(unit)->bitmaps), joan::orig::arg::i16(id));                                                       // 0x0071b020
            jo::self<jo::MiniMap>(this)->x = id;
            const char* const frame = joan::orig::arg::str(joan::orig::as_mini_res(jo::view<void*>(jo::self<jo::MiniMap>(this)->view))->find_frame(joan::orig::arg::i16(jo::self<jo::MiniMap>(this)->visible), joan::orig::arg::i16(1), joan::orig::arg::i16(1)));  // 0x0071b3b0
            std::uint8_t* const unit2 = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(img::obj(kUnitsDb))->get_420240(static_cast<std::int32_t>(jo::self<jo::MiniMap>(this)->x)));
            std::uint8_t* const image = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_bmp_res(&jo::as_world(unit2)->bitmaps)->find(joan::orig::arg::str(frame)));  // 0x00620b40
            jo::self<jo::MiniMap>(this)->width = jo::view<std::int16_t>(jo::as_bmp_entry(image)->width);
            jo::self<jo::MiniMap>(this)->height = jo::view<std::int16_t>(jo::as_bmp_entry(image)->height);
            for (std::int32_t i = 1; i <= 0x1e; ++i) {
                const Vec3 v = global<Vec3>(d::g_pos);
                joan::orig::as_mini_map(this)->set_waypoint(joan::orig::arg::i16(static_cast<std::int16_t>(i)), static_cast<char>(joan::orig::arg::i32(0)), v, static_cast<char>(joan::orig::arg::i32(0)));  // 0x00718010
            }
            for (std::int32_t i = 0; i < 10; ++i) jo::self<jo::MiniMap>(this)->slot_used[i] = 0;
            joan::orig::as_marker(this)->set_word_f(joan::orig::arg::u16(1));  // 0x0044cf50
        }
        for (std::int32_t i = 0; i < 0x1e; ++i) jo::self<jo::MiniMap>(this)->marker_unit[i] = 0;
        jo::self<jo::MiniMap>(this)->rotation = 0;
        jo::self<jo::MiniMap>(this)->show_gold = 0;
    }
};

#define U2(name, addr, fn) JOAN_REPLACE("game/ui2", name, addr, fn)
U2("Panel::set_unit", 0x00715d50, &H::set_unit);
#undef U2

}  // namespace game_ui2
}  // namespace

// ---------------------------------------- from modules/game_ui8.cpp (game/ui8)
//
//      - MiniRes::MiniRes 0x0071aff0: memset(this, 0, 0x31). ret 0.
//
//      - MiniRes::load 0x0071b020 (bmpres, id): free if loaded, 0x0071b6a0, 0x0071bf10, +0 = 1.
//        ret 8.
namespace {
namespace game_ui8_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;

constexpr fn::Code kMemset = fn::crt_memset;

JOAN_COUNTER(c_mr_load, "MiniRes::load");

struct H {
    // 0x0071aff0. ret 0.
    void* minires_ctor() {
        cdecl_call<void*>(kMemset, static_cast<void*>(this), 0, static_cast<std::uint32_t>(sizeof(jo::MiniRes)));  // 0x31 on x86
        return this;
    }

    // 0x0071b020. ret 8.
    void minires_load(void* res, std::int16_t id) {
        c_mr_load.hit();
        if (static_cast<std::int8_t>(jo::self<jo::MiniRes>(this)->loaded) != 0) joan::orig::as_mini_res(this)->free();  // 0x0071b080
        joan::orig::as_mini_res(this)->read_frames(joan::orig::arg::mptr(res), joan::orig::arg::i16(static_cast<std::int32_t>(id)));                      // 0x0071b6a0
        joan::orig::as_mini_res(this)->read_maps(joan::orig::arg::i16(static_cast<std::int32_t>(id)));                           // 0x0071bf10
        jo::self<jo::MiniRes>(this)->loaded = 1;
    }
};

#define U8(name, addr, fn) JOAN_REPLACE("game/ui8", name, addr, fn)
U8("MiniRes::MiniRes", 0x0071aff0, &H::minires_ctor);
U8("MiniRes::load", 0x0071b020, &H::minires_load);
#undef U8

}  // namespace game_ui8_2
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    void* zero_480() {  // 0x0070e190
        memset_(this, 0, static_cast<u32>(sizeof(jo::MiniMap)));  // 0x480 on x86 (the MiniMap of hud_init 0x0070e1d0)
        return this;
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("Struct480::zero", 0x0070e190, &H::zero_480);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

using joan::hook::global;

using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

JOAN_COUNTER(c_hud_init, "Hud::init_70e1d0");

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x0070e1d0
    std::uintptr_t hud_init() {
        c_hud_init.hit();
        joan::orig::as_screen(me())->release_70e3c0();  // 0x0070e3c0
        jo::self<jo::MiniMap>(this)->active = 0;
        jo::view<u16>(jo::self<jo::MiniMap>(this)->visible) = 1;
        jo::view<u16>(jo::self<jo::MiniMap>(this)->unit_id) = global<u16>(d::g_a47c36);
        jo::self<jo::MiniMap>(this)->view = nullptr;
        jo::self<jo::MiniMap>(this)->u8_2d = 1;
        if (jo::self<jo::MiniMap>(this)->unit_id != 0) {
            jo::self<jo::MiniMap>(this)->unit =
                joan::orig::arg::as<jo::Unit*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(static_cast<i32>(jo::self<jo::MiniMap>(this)->unit_id)));  // 0x0041b3c0
            jo::self<jo::MiniMap>(this)->nation = joan::orig::arg::as<jo::Nation*>(joan::orig::as_unit(jo::self<jo::MiniMap>(this)->unit)->nation());               // 0x00509400
        }
        jo::view<u16>(jo::self<jo::MiniMap>(this)->blink_rate) = 0xb4;
        jo::self<jo::MiniMap>(this)->blink_time = global<u32>(d::g_now);
        jo::self<jo::MiniMap>(this)->blink_on = 0;
        jo::view<u16>(jo::self<jo::MiniMap>(this)->blink_rate_b) = 0xb4;
        jo::self<jo::MiniMap>(this)->blink_time_b = global<u32>(d::g_now);
        jo::self<jo::MiniMap>(this)->blink_on_b = 0;
        jo::view<u16>(jo::self<jo::MiniMap>(this)->frame) = 1;
        for (i32 i = 0; i < 0x1e; ++i) {
            jo::self<jo::MiniMap>(this)->marker_on[i] = 0;
            jo::self<jo::MiniMap>(this)->marker_frame[i] = 1;
            jo::self<jo::MiniMap>(this)->marker_time[i] = 0;
        }
        jo::self<jo::MiniMap>(this)->icons = static_cast<jo::BmpRes*>(img::obj(d::g_heroIcons));
        const u8 r = joan::orig::arg::as<u8>(joan::orig::as_ai(obj(d::g_keys_a51806))->is_ready(i32{5}));  // 0x00597ed0
        jo::self<jo::MiniMap>(this)->key_down = static_cast<i16>(static_cast<i8>(r));
        jo::self<jo::MiniMap>(this)->u32_474 = 0;
        jo::self<jo::MiniMap>(this)->fog = joan::orig::arg::as<u8*>(joan::crt::operator_new(u32{0x9c40}));  // operator new
        jo::self<jo::MiniMap>(this)->texture = joan::orig::arg::as<jo::Texture2D*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->create_2d());  // 0x006094c0
        joan::orig::as_texture(jo::self<jo::MiniMap>(this)->texture)->create(joan::orig::arg::u32(i32{0x100}), joan::orig::arg::u32(i32{0x100}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{0xb}));  // 0x00605e20
        return joan::orig::arg::as<std::uintptr_t>(std::memset(joan::orig::arg::mptr(jo::self<jo::MiniMap>(this)->slot_used), i32{0}, u32{0xa}));  // memset
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("Hud::init_70e1d0", 0x0070e1d0, &H::hud_init);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_ui8_3.cpp (game/ui8)
namespace {
namespace game_ui8_3 {

using joan::hook::global;

using joan::call::cdecl_call;

using joan::call::thiscall;

struct Raw3 {  // float members: the callers pass three floats (XMM registers on System V x86-64)
    float w[3];
};
static_assert(sizeof(Raw3) == 12);

constexpr fn::Code kDelete = fn::op_delete;

JOAN_COUNTER(c_slot, "Panel::set_slot");

struct H {
    // 0x0070e4b0. ret 4.
    void* view_delete(std::uint32_t flags) {
        joan::orig::as_mini_res(this)->destruct();  // 0x00715d20
        if ((flags & 1u) != 0) cdecl_call(kDelete, static_cast<void*>(this));
        return this;
    }

    // 0x0070f9b0. ret 0.
    void clear_unit() {
        joan::orig::as_panel(this)->set_flag_e(static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x006e0dc0
        joan::orig::as_marker(this)->set_word_f(joan::orig::arg::u16(1));  // 0x0044cf50
    }

    // 0x00718010. ret 0x18.
    void set_slot(std::int16_t index, char value, Raw3 v, char flag) {
        c_slot.hit();
        const std::int32_t i = static_cast<std::int16_t>(index - 1);
        if (static_cast<std::int32_t>(static_cast<std::int8_t>(value)) ==
            static_cast<std::int32_t>(static_cast<std::int8_t>(jo::self<jo::MiniMap>(this)->marker_on[i])))
            return;
        if (static_cast<std::int8_t>(value) != 0) {
            jo::view<char>(jo::self<jo::MiniMap>(this)->marker_on[i]) = value;
            jo::view<std::uint32_t>(jo::self<jo::MiniMap>(this)->marker_pos[i * 3]) = joan::orig::arg::bits(v.w[0]);  // the float's bits, as the dword store of the original  // marker i's x, y, z
            jo::view<std::uint32_t>(jo::self<jo::MiniMap>(this)->marker_pos[i * 3 + 1]) = joan::orig::arg::bits(v.w[1]);  // the float's bits, as the dword store of the original
            jo::view<std::uint32_t>(jo::self<jo::MiniMap>(this)->marker_pos[i * 3 + 2]) = joan::orig::arg::bits(v.w[2]);  // the float's bits, as the dword store of the original
            jo::self<jo::MiniMap>(this)->marker_frame[i] = 1;
            jo::self<jo::MiniMap>(this)->marker_time[i] = global<std::uint32_t>(d::g_now);
            jo::view<char>(jo::self<jo::MiniMap>(this)->marker_kind[i]) = flag;
        } else {
            jo::view<char>(jo::self<jo::MiniMap>(this)->marker_on[i]) = value;
        }
    }
};

#define U8(name, addr, fn) JOAN_REPLACE("game/ui8", name, addr, fn)
U8("PanelView::scalar_deleting_dtor", 0x0070e4b0, &H::view_delete);
U8("Panel::clear_unit", 0x0070f9b0, &H::clear_unit);
U8("Panel::set_slot", 0x00718010, &H::set_slot);
#undef U8

}  // namespace game_ui8_3
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

struct V3 {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers
    float v[3];
};

JOAN_COUNTER(c_minimap, "Minimap::draw_70e500");

using joan::orig::helpers::units;

// 0x0042a260 (thunk 0x004098b8, ret 0x10): builds a 4-byte colour in `c`, returns &c.
u32 make_color(i32 a, i32 b, i32 c, i32 d) {
    u32 color = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&color)->construct(a, b, c, d));
}

struct H {
    // ------------------------------------------------------------------ 0x0070e500
    std::uintptr_t minimap_draw_70e500(u32 flags) {  // eax: this, an early result or 0x0060a740's (MiniMap::draw returns uintptr_t)
        c_minimap.hit();
        if (jo::self<jo::MiniMap>(this)->active == 0) return reinterpret_cast<std::uintptr_t>(this);
        {
            const u32 r = joan::orig::arg::as<u32>(joan::orig::as_dynarray(units())->empty_at(i32{jo::self<jo::MiniMap>(this)->unit_id}));  // 0x00428b70
            if (r != 0) return r;
        }
        {
            const u32 r = joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_a45f08))->empty_at_561320(i32{jo::self<jo::MiniMap>(this)->x}));  // 0x00561320
            if (r != 0) return r;
        }
        const u32 prev = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(obj(d::g_texture_array))->mode());  // 0x0060a720
        joan::orig::as_texture_manager(obj(d::g_texture_array))->set_mode(joan::orig::arg::u32(i32{1}));            // 0x0060a740
        if ((static_cast<i32>(static_cast<i8>(flags & 0xff)) & 1) != 0) {
            joan::orig::as_marker(this)->update();          // 0x00714040
            joan::orig::as_mini_map(this)->draw_markers(i32{1});  // 0x00716010
            joan::orig::as_mini_map(this)->draw_tiles();          // 0x007141a0
            joan::orig::as_mini_map(this)->draw_markers(i32{0});
            for (i32 i = joan::orig::as_dynarray(units())->size(); i > 0; --i) {  // 0x00417ad0
                void* const u = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
                if (u == nullptr) continue;
                if (i == static_cast<i32>(jo::self<jo::MiniMap>(this)->unit_id)) continue;
                if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(u)->dead())) != 0 &&  // 0x0042d0e0
                    static_cast<i32>(jo::as_unit(u)->id) != static_cast<i32>(jo::self<jo::MiniMap>(this)->unit_id))
                    continue;
                if (global<i32>(d::g_control_mode) == 4 || jo::view<i32>(jo::self<jo::MiniMap>(this)->mode) == 4) joan::orig::as_mini_map(this)->update_unit(joan::orig::arg::mptr(u));  // 0x007190b0
                void* const p = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(i32{jo::as_unit(u)->type_id})));  // 0x0042d0a0
                if (p != nullptr && (jo::as_unit_type(p)->kind == 2 || jo::as_unit_type(p)->kind == 7)) continue;
                joan::orig::as_mini_map(this)->draw_unit(joan::orig::arg::mptr(u));  // 0x0070fbe0
            }
            joan::orig::as_mini_map(this)->draw_unit(joan::orig::arg::mptr(jo::self<jo::MiniMap>(this)->unit));
            {
                const i32 width = global<i32>(d::g_screen_w);
                const u32 c = make_color(0, 0, 0, 0xff);
                joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->blit_image(joan::orig::arg::i16(width - 0xd3), joan::orig::arg::i16(i32{0xa}), joan::orig::arg::u32(i32{9}), joan::orig::arg::mptr(jo::self<jo::MiniMap>(this)->texture), joan::orig::arg::u32(c), joan::orig::arg::i16(i32{1}), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{0xc7}), joan::orig::arg::i16(i32{0xc7}), joan::orig::arg::i16(i32{jo::self<jo::MiniMap>(this)->rotation}), joan::orig::arg::i16(global<i32>(d::g_screen_w) - 0xc), joan::orig::arg::i16(i32{0xd1}));  // 0x006332b0
            }
            joan::orig::as_mini_map(this)->draw_view_716be0();  // 0x00716be0
            {
                const u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::self<jo::MiniMap>(this)->icons)->find(img::str(d::s_FRAME)));  // 0x00620b40 'FRAME'
                void* const frame = joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(jo::as_bmp_entry(rec)->file), i32{1}, joan::orig::arg::u32(i32{0}));  // 0x006099c0
                const u32 c = make_color(0xff, 0xff, 0xff, 0xff);
                joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(global<i32>(d::g_screen_w) - 0xd7), 6, 8u, img::str(d::s_FRAME), joan::orig::arg::mptr(jo::self<jo::MiniMap>(this)->icons), joan::orig::arg::u32(c), 3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
                joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(frame));  // 0x0060a550
            }
            i32 shown = 1;  // [ebp-0x1c]
            i32 slot = 0x14;
            for (i32 i = joan::orig::as_dynarray(obj(d::g_map_object_list))->size(); i > 0; --i) {
                if (joan::orig::arg::as<i32>(joan::orig::as_types(obj(d::g_a50530))->empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00485410
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_a50530))->at_plus4(joan::orig::arg::u32(i)));  // 0x004853e0
                if (jo::self<jo::MiniMap>(this)->show_gold != 0) joan::orig::as_mini_map(this)->draw_gold_71aba0(joan::orig::arg::mptr(e));  // 0x0071aba0
                if (slot >= 0x1e) continue;
                if (jo::as_map_object(e)->class_id != 0xf) continue;
                void* const it = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(i32{jo::as_map_object(e)->type_id})));  // 0x00425ce0
                if (joan::crt::stricmp(joan::orig::arg::str(jo::as_item_def(it)->name), str(d::s_S7_GOLDBAR)) == 0) continue;  // _stricmp 'S7_GOLDBAR'
                jo::self<jo::MiniMap>(this)->marker_on[slot] = 1;
                const V3 v = *reinterpret_cast<const V3*>(jo::as_map_object(e)->pos);
                joan::orig::as_mini_map(this)->draw_waypoint(joan::orig::arg::u32(slot), v, &shown);  // 0x00718540
                ++slot;
            }
            for (i32 j = 0; j < 0x14; ++j) {
                if (jo::view<i8>(jo::self<jo::MiniMap>(this)->marker_on[j]) == 0) continue;
                const u32 id = jo::self<jo::MiniMap>(this)->marker_unit[j];
                if (id != 0) {
                    if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(id)) == 0) {
                        void* const u = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(jo::self<jo::MiniMap>(this)->marker_unit[j]));  // 0x0041b3c0
                        const V3 v = *reinterpret_cast<const V3*>(jo::as_unit(u)->position_);
                        joan::orig::as_mini_map(this)->draw_waypoint(joan::orig::arg::u32(j), v, &shown);
                    }
                } else {
                    const V3 v = *reinterpret_cast<const V3*>(&jo::self<jo::MiniMap>(this)->marker_pos[j * 3]);
                    joan::orig::as_mini_map(this)->draw_waypoint(joan::orig::arg::u32(j), v, &shown);
                }
            }
            for (i32 i = joan::orig::as_dynarray(obj(d::g_territory_array))->size(); i > 0; --i) {
                if (joan::orig::arg::as<i32>(joan::orig::as_dynarray(obj(d::g_territory_array))->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x004217e0
                void* const o = joan::orig::arg::as<void*>(joan::orig::as_dynarray(obj(d::g_territory_array))->get_420ff0(joan::orig::arg::i32(i)));  // 0x00420ff0
                if (jo::as_zone_def(o)->owner != -1 && jo::as_zone_def(o)->flag_46 != 0 &&
                    !(global<u32>(d::g_now) - jo::as_zone_def(o)->time > 0xea60u))
                    joan::orig::as_mini_map(this)->draw_marker_71a3b0(joan::orig::arg::i32(i));  // 0x0071a3b0
            }
        }
        jo::self<jo::MiniMap>(this)->mode = global<u32>(d::g_control_mode);
        return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(obj(d::g_texture_array))->set_mode_60a740(joan::orig::arg::u32(prev)));
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Minimap::draw_70e500", 0x0070e500, &H::minimap_draw_70e500);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_mapbutton_70eb20, "MapButton::click_70eb20");

struct H {
    // -------------------------------------------------------------- 0x0070eb20
    u8 map_button_click() {
        c_mapbutton_70eb20.hit();
        if (global<i32>(d::g_control_mode) == 4) {
            joan::orig::as_mini_map(this)->handle_click();  // 0x00717240
            return 0;
        }
        const u32 open = joan::orig::arg::as<u32>(joan::orig::as_bindings(obj(d::g_keys_a48820))->state(i32{1}, joan::orig::arg::u32(i32{0x2d})));  // 0x004cb6d0
        if (global<i32>(d::g_control_mode) == 1) {
            if (open != 0 && (global<u16>(d::g_key_modifiers) & 0x30u) == 0 && jo::self<jo::MiniMap>(this)->key_down == 0) {
                jo::view<u16>(jo::self<jo::MiniMap>(this)->visible) = jo::self<jo::MiniMap>(this)->visible == 0 ? u16{1} : u16{0};
                joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_MAP_BUTTON));  // 0x004ceef0 "MAP_BUTTON"
                joan::orig::as_marker(this)->update();                              // 0x00714040
            }
        } else if (joan::orig::arg::as<u32>(joan::orig::as_race_res(obj(d::g_keys_a48820))->test(joan::orig::arg::i32(global<u32>(d::g_control_mode)), i32{0x2d})) != 0 &&  // 0x004cb820
                   (global<u16>(d::g_key_modifiers) & 0x30u) == 0) {
            jo::view<u16>(jo::self<jo::MiniMap>(this)->visible) = jo::self<jo::MiniMap>(this)->visible == 0 ? u16{1} : u16{0};
            joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_MAP_BUTTON));
            joan::orig::as_marker(this)->update();
        }
        jo::view<u16>(jo::self<jo::MiniMap>(this)->key_down) = static_cast<u16>(open);
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("MapButton::click_70eb20", 0x0070eb20, &H::map_button_click);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b109_10.cpp (game/b109_10)
namespace {
namespace game_b109_10 {

using joan::hook::global;

using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::call::cdecl_call;

using joan::hook::x87::Fpu;

using joan::orig::helpers::wsub;

using joan::orig::helpers::imul;

using joan::hook::x87::ftol;

using joan::call::thiscall;
template <class R = void, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
// The marker slot fields a..i of slot j (MiniMap::slot_a..slot_i, 10 slots each).
u32 (jo::MiniMap::* const kSlotField[9])[10] = {&jo::MiniMap::slot_a, &jo::MiniMap::slot_b, &jo::MiniMap::slot_c, &jo::MiniMap::slot_d, &jo::MiniMap::slot_e,
                                              &jo::MiniMap::slot_f, &jo::MiniMap::slot_g, &jo::MiniMap::slot_h, &jo::MiniMap::slot_i};

volatile i32 g_sink = 0;  // keeps an unused idiv of the original (its trap) alive

JOAN_COUNTER(c_mini, "MiniMap::update_70ec50");

struct H {
    // 0x0070ec50
    void mini_update() {
        c_mini.hit();
        jo::MiniMap* const self = jo::self<jo::MiniMap>(this);
        if (self->active == 0 || self->unit_id == 0 || self->x == 0) return;
        const Fpu F{true};
        const u32 now = global<u32>(d::g_now);
        g_sink = (60000 / i32{self->blink_rate_b}) / 2;  // [ebp-0xc], unused
        joan::orig::as_marker(self)->update();  // 0x00714040
        if (self->frame_time + 0x168u < now) {
            self->frame = static_cast<i16>(self->frame + 1);
            if (self->frame > 3) self->frame = 1;
            self->frame_time = now;
        }
        for (i32 j = 0; j < 0x1e; ++j) {
            if (jo::view<i8>(self->marker_on[j]) == 0) continue;
            if (self->marker_time[j] + 100u < now) {
                self->marker_frame[j] = static_cast<u8>(self->marker_frame[j] + 1);
                if (jo::view<i8>(self->marker_frame[j]) > 7) self->marker_frame[j] = 1;
                self->marker_time[j] = now;
            }
        }
        const i32 blink = (60000 / i32{self->blink_rate}) / 2;
        if (self->blink_time + static_cast<u32>(blink) < now) {
            self->blink_time = now;
            self->blink_on = static_cast<u8>(self->blink_on == 0 ? 1 : 0);
        }
        if (self->fog == nullptr) return;
        if (!(self->u32_474 < now)) return;
        if (self->active == 0) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_a45f08))->empty_at_561320(i32{self->x})) != 0) return;  // 0x00561320
        std::memset(joan::orig::arg::mptr(self->fog), 0, 0x9c40u);  // memset

        // Scale divisors: word +0xf is always 0 / 1 (see header); 0 for the dead default.
        i32 d1 = 0, d2 = 0;
        switch (self->visible) {
            case 0:
                d1 = self->view->i32_21;
                d2 = self->view->i32_25;
                break;
            case 1:
                d1 = self->view->i32_29;
                d2 = self->view->i32_2d;
                break;
            default:
                break;
        }
        // The fog is rebuilt on every update (the units move), and it decides which enemy actors are shown and
        // clickable (update_unit_7190b0), so it stays bit-exact - but cheaper: at precision control 24 each Fpu
        // step is the exact result rounded to a 24-bit significand, which for operands that are floats (or
        // integers floats hold exactly) is IEEE single arithmetic, and the CRT's sqrt of a float is sqrtf.
        // Native float steps are used when every operand is such a value and nothing can be 0 / inf / NaN
        // (where the error-term arithmetic of Fpu and the hardware could part); the original steps otherwise.
        struct Exact {
            const Fpu& F;
            double add(double a, double b) const { return F.add(a, b); }
            double sub(double a, double b) const { return F.sub(a, b); }
            double mul(double a, double b) const { return F.mul(a, b); }
            double div(double a, double b) const { return F.div(a, b); }
            double root(float q) const { return joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(q))); }  // 0x004256d0 sqrt
        };
        struct Native {
            static float f(double v) { return static_cast<float>(v); }  // exact: the operands are floats
            double add(double a, double b) const { return static_cast<double>(f(a) + f(b)); }
            double sub(double a, double b) const { return static_cast<double>(f(a) - f(b)); }
            double mul(double a, double b) const { return static_cast<double>(f(a) * f(b)); }
            double div(double a, double b) const { return static_cast<double>(f(a) / f(b)); }
            double root(float q) const { return static_cast<double>(std::sqrt(q)); }
        };
        const Exact exact{F};
        const Native native{};
        auto in_float = [](i32 v) { return v >= -(1 << 24) && v <= (1 << 24); };
        const bool fast = joan::hook::x87::current_precision() == joan::hook::x87::Precision::Single && d1 > 0 && d2 > 0 &&
                          self->width > 0 && self->height > 0 && in_float(d1) && in_float(d2) && in_float(self->width) &&
                          in_float(self->height);
        auto scales = [&](const auto& o, float& sx, float& sy, float& ratio) {
            sx = static_cast<float>(o.div(static_cast<double>(self->width), static_cast<double>(d1)));
            sy = static_cast<float>(o.div(static_cast<double>(self->height), static_cast<double>(d2)));
            ratio = static_cast<float>(o.div(sx, sy));
        };
        float sx = 0.0f, sy = 0.0f, ratio = 0.0f;
        if (fast) scales(native, sx, sy, ratio);
        else scales(exact, sx, sy, ratio);

        // One unit's view circle into the fog (the original's steps, through `o`).
        auto circle = [&](const auto& o, float px, float py, i32 rad) {
            px = static_cast<float>(ftol(o.add(px, 0.5)));
            py = static_cast<float>(ftol(o.add(py, 0.5)));
            px = static_cast<float>(o.sub(px, self->scale_x));
            py = static_cast<float>(o.sub(self->scale_y, py));
            const double rd = static_cast<double>(rad);
            if (!(static_cast<double>(d1) >= o.sub(px, rd))) return;
            if (!(static_cast<double>(d2) >= o.sub(py, rd))) return;
            if (!(o.add(rd, px) >= 0.0)) return;
            if (!(o.add(rd, py) >= 0.0)) return;
            const float ry = static_cast<float>(o.mul(rd, sy));
            const float cy = static_cast<float>(o.mul(py, sy));
            i32 row0 = ftol(o.sub(cy, ry));
            i32 row1 = ftol(o.add(cy, ry));
            if (row0 < 0) row0 = 0;
            if (row1 >= 200) row1 = 199;
            for (i32 row = row0; row <= row1; ++row) {
                const float dy = static_cast<float>(o.sub(o.div(static_cast<double>(row), sy), py));
                const float q = static_cast<float>(o.sub(static_cast<double>(imul(rad, rad)), o.mul(dy, dy)));
                if (!(q >= 0.0f)) continue;
                const double root = o.root(q);
                const float half = static_cast<float>(o.mul(root, ratio));
                i32 lo = ftol(o.add(o.mul(o.sub(px, half), sx), 0.5));
                i32 hi = ftol(o.add(o.mul(o.add(px, half), sx), 0.5));
                if (hi < 0 || lo >= 200) continue;
                if (lo < 0) lo = 0;
                if (hi >= 200) hi = 199;
                u8* const dst = self->fog + static_cast<u32>(imul(row, 200)) + static_cast<u32>(lo);
                std::memset(joan::orig::arg::mptr(static_cast<void*>(dst)), 1, joan::orig::arg::u32(wsub(hi, lo) + 1));  // memset
            }
        };

        for (i32 k = joan::orig::as_dynarray(obj(d::g_units))->size(); k > 0; --k) {  // 0x00417ad0
            void* const unit = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(k));  // 0x0041b3f0
            if (unit == nullptr) continue;
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(unit)->dead())) != 0) continue;  // 0x0042d0e0
            void* const type = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(i32{jo::as_unit(unit)->type_id})));  // 0x0042d0a0
            if (jo::as_unit_type(type)->kind == 2 || jo::as_unit_type(type)->kind == 7 || jo::as_unit_type(type)->unit_class == 0) continue;
            if (jo::as_unit(unit)->side != self->unit->side) {
                void* const grp = joan::orig::arg::as<void*>(joan::orig::as_unit(unit)->nation());  // 0x00509400
                if (grp == nullptr) continue;
                void* const rel = joan::orig::arg::as<void*>(joan::orig::as_player(grp)->relation(i32{self->nation->id}));  // 0x004b42e0
                if (rel == nullptr || jo::object<jo::Relation>(rel)->attitude != 4) continue;
            }
            const float px = jo::as_unit(unit)->position_[0];
            const float py = jo::as_unit(unit)->position_[1];
            const i32 rad = jo::view<i32>(jo::as_unit(unit)->vision_range);
            // rad * rad must be a float too (and the positions finite: no inf / NaN steps)
            if (fast && rad >= -4096 && rad <= 4096 && std::isfinite(px) && std::isfinite(py)) circle(native, px, py, rad);
            else circle(exact, px, py, rad);
        }

        // Markers: kind 0 -> 0x00719900(a, b, c, d), 1 -> 0x00719b40(a, b, e), 2 -> 0x00719e60(a..h);
        // f(n) reads the n-th field (0..7 = a..h, 8 = e of kind 1).
        auto marker = [&](u8 kind, auto f) {
            switch (kind) {
                case 0: joan::orig::as_mini_map(self)->reveal_rect(joan::orig::arg::i32(f(0)), joan::orig::arg::i32(f(1)), joan::orig::arg::i32(f(2)), joan::orig::arg::i32(f(3))); break;  // 0x00719900
                case 1: joan::orig::as_mini_map(self)->reveal_circle(joan::orig::arg::i32(f(0)), joan::orig::arg::i32(f(1)), joan::orig::arg::i32(f(8))); break;        // 0x00719b40
                case 2:
                    joan::orig::as_mini_map(self)->reveal_quad(joan::orig::arg::i32(f(0)), joan::orig::arg::i32(f(1)), joan::orig::arg::i32(f(2)), joan::orig::arg::i32(f(3)), joan::orig::arg::i32(f(4)), joan::orig::arg::i32(f(5)), joan::orig::arg::i32(f(6)), joan::orig::arg::i32(f(7)));  // 0x00719e60
                    break;
                default: break;
            }
        };
        for (i32 k = joan::orig::as_dynarray(obj(d::g_territory_array))->size(); k > 0; --k) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_dynarray(obj(d::g_territory_array))->is_empty_at(joan::orig::arg::i32(k))) != 0) continue;    // 0x004217e0
            void* const item = joan::orig::arg::as<void*>(joan::orig::as_dynarray(obj(d::g_territory_array))->get_420ff0(joan::orig::arg::i32(k)));  // 0x00420ff0
            if (!(jo::as_zone_def(item)->owner > 0)) continue;
            if (jo::as_zone_def(item)->owner != i32{self->unit->side}) {
                void* const grp = joan::orig::arg::as<void*>(self->unit->nation());  // 0x00509400
                void* const rel = joan::orig::arg::as<void*>(joan::orig::as_player(grp)->relation(i32{self->nation->id}));  // 0x004b42e0
                if (rel == nullptr || jo::object<jo::Relation>(rel)->attitude != 4) continue;
            }
            for (i32 j = 0; j < jo::as_zone_def(item)->shape_count; ++j) {
                marker(jo::as_zone_def(item)->shape_kind[j], [&](i32 n) { return jo::view<u32>(jo::as_zone_def(item)->coords[j + n * 4]); });
            }
        }
        for (i32 j = 0; j < 10; ++j) {
            if (jo::view<i8>(self->slot_used[j]) == 0) continue;
            marker(self->slot_kind[j], [&](i32 n) { return (self->*kSlotField[n])[j]; });
        }
        self->u32_474 = now;

        i32 rect[4] = {0, 0, 0xc7, 0xc7};  // left, top, right, bottom
        struct {
            i32 pitch;
            u8* bits;
        } locked{0, nullptr};  // D3DLOCKED_RECT Pitch, pBits (0 if LockRect fails, see header)
        com_call<i32>(self->texture->d3d_texture, 0x40 / 4, 0u, static_cast<void*>(&locked),
                      static_cast<void*>(rect), 0u);  // LockRect
        u8* line = locked.bits;
        for (i32 yy = 0; yy < 200; ++yy) {
            u8* cell = line;
            for (i32 xx = 0; xx < 200; ++xx) {
                const i8 v = *reinterpret_cast<const i8*>(self->fog + static_cast<u32>(yy * 200 + xx));
                *reinterpret_cast<u32*>(cell) = v != 0 ? 0u : 0x80000000u;
                cell += 4;
            }
            line += static_cast<u32>(locked.pitch);
        }
        com_call<i32>(self->texture->d3d_texture, 0x44 / 4, 0u);  // UnlockRect
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_10", name, addr, fn)
B("MiniMap::update_70ec50", 0x0070ec50, &H::mini_update);
#undef B

}  // namespace game_b109_10
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

using joan::img::obj;

using joan::call::thiscall;

JOAN_COUNTER(c_to_screen, "MapView::to_screen");

struct H {
    // ------------------------------------------------------------------ 0x0070f9e0
    float* to_screen(void* unit, float* out) {
        c_to_screen.hit();
        float v[3];
        joan::orig::as_unit(unit)->position(joan::orig::arg::mptr(v));  // 0x004fedb0
        v[2] = 0.0f;
        const float sx = joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->round_decimals(joan::orig::arg::f32(v[0]), 0, 0));  // 0x00590740
        v[0] = sx;
        const float sy = joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->round_decimals(joan::orig::arg::f32(v[1]), 0, 0));
        v[1] = sy;
        out[0] = v[0] - jo::self<jo::MiniMap>(this)->scale_x;
        out[1] = jo::self<jo::MiniMap>(this)->scale_y - v[1];
        return out;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("MapView::to_screen", 0x0070f9e0, &H::to_screen);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b90_06_2.cpp (game/b90_06)
namespace {
namespace game_b90_06_2 {

using joan::hook::global;

using joan::img::obj;

using i32 = std::int32_t;

using joan::hook::x87::Fpu;

const Fpu fpu{true};

using joan::call::thiscall;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

constexpr img::Addr kUnitTable(d::g_units);
constexpr img::Addr kTerrain(d::g_misc);

// 0x00428b70: 1 when the id is NOT in the table.
std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kUnitTable))->empty_at(joan::orig::arg::i32(id)); }
double terrain_height(std::uint32_t v) {                                                    // 0x00590740
    return joan::orig::arg::as<double>(joan::orig::as_misc(img::obj(kTerrain))->round_decimals(joan::orig::arg::f32(v), 0, 0));
}

JOAN_COUNTER(c_marker, "Marker::update");
JOAN_COUNTER(c_marker_vis, "Marker::visible");

struct H {
    // 0x0070fa70: out = (terrain(a) - this.x(+0x1d), this.y(+0x21) - terrain(b)).
    float* screen_offset(float* out, std::uint32_t a, std::uint32_t b, std::uint32_t c) {
        (void)c;  // the original zeroes this argument slot
        const double ha = terrain_height(a);  // the fields are read after each call
        out[0] = static_cast<float>(fpu.sub(ha, jo::self<jo::MiniMap>(this)->scale_x));
        const double hb = terrain_height(b);
        out[1] = static_cast<float>(fpu.sub(jo::self<jo::MiniMap>(this)->scale_y, hb));
        return out;
    }
    // 0x00714040 Marker::update: active (+0xe), owner record (+4), unit id (+0x13), unit
    // (+0x15), side (+0xf) -> position (+0x1d / +0x21) snapped through the terrain.
    std::uintptr_t marker_update() {
        c_marker.hit();
        if (jo::self<jo::MiniMap>(this)->active == 0) return reinterpret_cast<std::uintptr_t>(this);
        if (jo::self<jo::MiniMap>(this)->view == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        const std::int32_t missing = unit_missing(jo::self<jo::MiniMap>(this)->unit_id);
        if (missing != 0) return static_cast<std::uintptr_t>(missing);
        V3 pos;
        joan::orig::as_unit(jo::view<void*>(jo::self<jo::MiniMap>(this)->unit))->position(&pos);  // 0x004fedb0
        const double two = global<float>(d::flt_2);
        const std::int32_t side = jo::self<jo::MiniMap>(this)->visible;
        if (side == 0) {
            void* const r = jo::view<void*>(jo::self<jo::MiniMap>(this)->view);
            jo::self<jo::MiniMap>(this)->scale_x = static_cast<float>(fpu.sub(pos.x, fpu.div(jo::as_mini_res(r)->i32_21, two)));
            jo::self<jo::MiniMap>(this)->scale_y = static_cast<float>(fpu.add(fpu.div(jo::self<jo::MiniMap>(this)->view->i32_25, two), pos.y));
        } else if (side == 1) {
            void* const r = jo::view<void*>(jo::self<jo::MiniMap>(this)->view);
            jo::self<jo::MiniMap>(this)->scale_x = static_cast<float>(fpu.sub(pos.x, fpu.div(jo::as_mini_res(r)->i32_29, two)));
            jo::self<jo::MiniMap>(this)->scale_y = static_cast<float>(fpu.add(fpu.div(jo::self<jo::MiniMap>(this)->view->i32_2d, two), pos.y));
        }
        jo::self<jo::MiniMap>(this)->scale_x = static_cast<float>(terrain_height(jo::view<std::uint32_t>(jo::self<jo::MiniMap>(this)->scale_x)));
        jo::self<jo::MiniMap>(this)->scale_y = static_cast<float>(terrain_height(jo::view<std::uint32_t>(jo::self<jo::MiniMap>(this)->scale_y)));
        jo::self<jo::MiniMap>(this)->u32_25 = 0;
        return reinterpret_cast<std::uintptr_t>(this);
    }
    // 0x00717650 Marker::visible: active and its item (+8) present, and the rectangle at
    // (+0xa, +0xc) intersects the view strip.
    std::uint8_t marker_visible() {
        c_marker_vis.hit();
        if (jo::self<jo::MiniMap>(this)->active == 0) return 0;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(img::obj(d::g_a45f08))->empty_at_561320(static_cast<std::int32_t>(jo::self<jo::MiniMap>(this)->x))) !=
            0)  // 0x00561320
            return 0;
        const std::int32_t a = jo::self<jo::MiniMap>(this)->width;
        const std::int32_t b = jo::self<jo::MiniMap>(this)->height;
        const std::int32_t g = global<std::int32_t>(d::g_screen_w);
        const std::int32_t hit = joan::orig::as_point(img::obj(d::g_mouse))
                                     ->inside(static_cast<std::int32_t>(static_cast<std::uint32_t>(g) - 0xd3u), 0xa,
                                              static_cast<std::int32_t>(static_cast<std::uint32_t>(g) + static_cast<std::uint32_t>(a) - 0xd3u),
                                              static_cast<std::int32_t>(static_cast<std::uint32_t>(b) + 0xau));  // 0x00593980
        return hit != 0 ? 1 : 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Screen::offset", 0x0070fa70, &H::screen_offset);
B("Marker::update", 0x00714040, &H::marker_update);
B("Marker::visible", 0x00717650, &H::marker_visible);
#undef B

}  // namespace game_b90_06_2
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

using joan::img::obj;

using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_random_offset, "Obj::random_offset");

struct H {
    // ------------------------------------------------------------------ 0x0070fae0
    float* random_offset(void* src, float* out) {
        c_random_offset.hit();
        u32 v[3];
        joan::orig::as_map_object(src)->position(joan::orig::arg::mptr(v));  // 0x00499930 (copies 12 bytes)
        void* const rnd = obj(d::g_misc);
        const float rx = joan::orig::arg::as<float>(joan::orig::as_misc(rnd)->round_decimals(joan::orig::arg::f32(v[0]), 0, 0));  // 0x00590740
        const float ry = joan::orig::arg::as<float>(joan::orig::as_misc(rnd)->round_decimals(joan::orig::arg::f32(v[1]), 0, 0));  // 0x00590740
        out[0] = rx - jo::self<jo::MiniMap>(this)->scale_x;
        out[1] = jo::self<jo::MiniMap>(this)->scale_y - ry;
        return out;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj::random_offset", 0x0070fae0, &H::random_offset);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b87_06.cpp (game/b87_06)
namespace {
namespace game_b87_06 {

using joan::img::obj;

using i32 = std::int32_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

constexpr img::Addr kUnitTable = d::g_units;

JOAN_COUNTER(c_in_range, "UnitMarker::in_range");

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(obj(kUnitTable))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
void* unit_get(std::int32_t id) { return joan::orig::as_dynarray(obj(kUnitTable))->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------ 0x00717a90 (ret 0xc)
    bool in_range(float x, float y, std::uint32_t /*unused*/) {
        c_in_range.hit();
        if (jo::self<jo::MiniMap>(this)->active == 0 ||
            joan::orig::arg::as<std::int32_t>(joan::orig::as_list(obj(d::g_a45f08))->empty_at_561320(static_cast<std::int32_t>(jo::self<jo::MiniMap>(this)->x))) != 0)  // 0x00561320
            return true;
        if (unit_missing(jo::self<jo::MiniMap>(this)->unit_id) != 0) return false;
        float p[3];
        joan::orig::as_unit(jo::view<void*>(jo::self<jo::MiniMap>(this)->unit))->position(joan::orig::arg::mptr(static_cast<void*>(p)));  // 0x004fedb0
        const std::int32_t dx = joan::hook::x87::ftol(x - p[0]);
        const std::int32_t dy = joan::hook::x87::ftol(y - p[1]);
        void* const shape = jo::view<void*>(jo::self<jo::MiniMap>(this)->view);
        if (joan::orig::arg::as<std::int32_t>(joan::orig::libc::abs(joan::orig::arg::i32(dx))) < jo::as_mini_res(shape)->i32_21 / 2 &&  // abs
            joan::orig::arg::as<std::int32_t>(joan::orig::libc::abs(joan::orig::arg::i32(dy))) < jo::as_mini_res(shape)->i32_25 / 2)
            return true;
        return false;
    }

    // ------------------------------------------------------------ 0x0070fb70 (ret 4)
    void set_unit(std::int16_t id) {
        unit_missing(id);
        jo::view<void*>(jo::self<jo::MiniMap>(this)->unit) = unit_get(id);
        jo::self<jo::MiniMap>(this)->unit_id = id;
        jo::self<jo::MiniMap>(this)->nation = joan::orig::arg::as<jo::Nation*>(joan::orig::as_unit(jo::view<void*>(jo::self<jo::MiniMap>(this)->unit))->nation());  // 0x00509400
    }
};

#define B06(name, addr, fn) JOAN_REPLACE("game/b87_06", name, addr, fn)
B06("UnitMarker::in_range", 0x00717a90, &H::in_range);
B06("UnitMarker::set_unit", 0x0070fb70, &H::set_unit);
#undef B06

}  // namespace game_b87_06
}  // namespace

// ---------------------------------------- from modules/game_b111_06.cpp (fix/uninit)
namespace {
namespace game_b111_06 {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::call::cdecl_call;

using joan::hook::x87::Fpu;

const Fpu fpu{true};

void* const kTextures = obj(d::g_texture_array);

using joan::hook::x87::ftol;

using joan::call::thiscall;

// vec3 by value (0x00718140 takes x, y, z; z is never read).
struct V3 {
    float x, y;
    u32 z;
};
static_assert(sizeof(V3) == 12, "vec3 by value");

JOAN_COUNTER(c_70fbe0, "MapOverlay::draw_unit_70fbe0");

// ------------------------------------------------------------------ 0x0070fbe0 helpers
void* const kUnits = obj(d::g_units);
void* const kGroups = obj(d::g_types);
void* const kCanvas = obj(d::g_sprites_a46128);

// Path string object (0x194-byte frame slot, ctor writes 0x191 bytes; no destructor is called).
struct Path {
    alignas(4) char b[0x194];
};
const char* c_str(Path& p) { return joan::orig::arg::str(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90

u8* group_of(const u8* unit) {
    return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kGroups)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
}
i32 has_flag(const u8* unit, i32 k) { return joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->has_mask_ready(static_cast<std::int8_t>(joan::orig::arg::i32(k)))); }  // 0x0050d550

u32 make_color() {
    u32 slot = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&slot)->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff}));  // 0x0042a260
}

// 3x3 dot at (x, y): 0x00632a70 (thunk 0x00407c7a).
void dot(float x, float y, u32 color) {
    const i32 y1 = ftol(fpu.add(y, 1.0));
    const i32 x1 = ftol(fpu.add(x, 1.0));
    const i32 y0 = ftol(fpu.sub(y, 1.0));
    const i32 x0 = ftol(fpu.sub(x, 1.0));
    joan::orig::as_sprite2d(kCanvas)->queue(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::u32(i32{0xa}), joan::orig::arg::u32(color), joan::orig::arg::mptr(i32{0}), i32{3}, i32{0}, i32{0}, i32{0}, i32{-1}, i32{-1}, i32{-1}, i32{-1}, i32{0});  // 0x00632a70
}

// Hero portraits: group name (_stricmp), '_H' marker when +0x2a5 == 0 && +0x2aa != 0, else '_M'.
const img::Addr kHeroes[8][3] = {
    {d::s_JOAN_OF_ARC, d::s_joan_H, d::s_joan_M},  // JOAN_OF_ARC          !joan_H / !joan_M
    {d::s_JEAN_DE_METZ, d::s_metz_H, d::s_metz_M},  // JEAN_DE_METZ         !metz_H / !metz_M
    {d::s_ALENCON_9cfb30, d::s_alen_H, d::s_alen_M},  // ALENCON              !alen_H / !alen_M
    {d::s_LAHIRE_9cfb3c, d::s_lahi_H, d::s_lahi_M},  // LAHIRE               !lahi_H / !lahi_M
    {d::s_ORLEAN, d::s_bast_H, d::s_bast_M},  // ORLEAN               !bast_H / !bast_M
    {d::s_FLAVY, d::s_flav_H, d::s_flav_M},  // FLAVY                !flav_H / !flav_M
    {d::s_CHARLIE7, d::s_char_H, d::s_char_M},  // CHARLIE7             !char_H / !char_M
    {d::s_arthur_de_richemont, d::s_rich_H, d::s_rich_M},  // arthur_de_richemont  !rich_H / !rich_M
};

struct H {
    // 0x00633500 (thunk 0x00401bbd): sprite `name` at (x, y) with the corner words c[0..3].
    void draw(float x, float y, img::Addr name, u32 color, const i32 (&c)[4]) {
        const i32 iy = ftol(y);
        const i32 ix = ftol(x);
        joan::orig::as_sprite_queue(kCanvas)->draw_named(joan::orig::arg::i16(ix), joan::orig::arg::i16(iy), 9u, joan::orig::arg::str(img::addr(name)), joan::orig::arg::mptr(jo::self<jo::MiniMap>(this)->icons), joan::orig::arg::u32(color), 3, 0, 0, 0, joan::orig::arg::i16(c[0]), joan::orig::arg::i16(c[1]), joan::orig::arg::i16(c[2]), joan::orig::arg::i16(c[3]), 0, -1, -1);  // 0x00633500
    }

    // Sprite marker `p` at (x, y) with the margin k (7.0f or 8.0f); with `outbox` the '!outbox'
    // frame is drawn first. Off the panel the original returns from the whole function (every
    // caller returns right after this, and no destructors are involved).
    void marker(Path& p, float k, float x, float y, bool outbox) {
        const i16 w1 = jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::self<jo::MiniMap>(this)->icons)->find(joan::orig::arg::str(c_str(p)))))->width);  // 0x00620b40
        const i16 w2 = jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::self<jo::MiniMap>(this)->icons)->find(joan::orig::arg::str(c_str(p)))))->height);
        const u32 sw = global<u32>(d::g_screen_w);  // no calls (other than _ftol) until the last read
        if (!(static_cast<double>(static_cast<i32>(sw - 0xbu)) >= x)) return;  // test ah,1 jne exit
        if (y > 210.0f) return;                                                  // test ah,0x41 jne go
        x = x - k;
        y = y - k;
        if (static_cast<double>(static_cast<i32>(sw - 0xd3u)) > x) return;  // test ah,0x41 je exit
        if (!(y >= 10.0f)) return;                                           // test ah,1 je go
        const i32 h1 = w1 / 2;  // cdq; sub; sar
        const i32 h2 = w2 / 2;
        i32 c[4] = {0, 0, 0, 0};
        c[0] = ftol(fpu.sub(fpu.sub(static_cast<double>(h1), k), 1.0));
        c[1] = ftol(fpu.sub(fpu.sub(static_cast<double>(h2), k), 1.0));
        const double k2 = static_cast<double>(k) + static_cast<double>(k);  // fadd st, st (exact)
        float cf = 0.0f;
        // fcompp (st0 = 2k, st1 = t); test ah,1 jne -> half-width branch when 2k < t or NaN.
        if (!(k2 >= fpu.sub(static_cast<double>(static_cast<i32>(sw - 0xbu)), x)))
            cf = static_cast<float>(fpu.sub(fpu.add(static_cast<double>(h1), k), 1.0));
        else
            cf = static_cast<float>(
                fpu.sub(static_cast<double>(static_cast<i32>(static_cast<u32>(c[0]) + sw - 0xbu)), x));
        c[2] = ftol(cf);
        float df = 0.0f;
        if (!(k2 >= fpu.sub(210.0, y)))
            df = static_cast<float>(fpu.sub(fpu.add(static_cast<double>(h2), k), 1.0));
        else
            df = static_cast<float>(fpu.sub(static_cast<double>(static_cast<i32>(static_cast<u32>(c[1]) + 0xd2u)), y));
        c[3] = ftol(df);
        u8* const def = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::self<jo::MiniMap>(this)->icons)->find(joan::orig::arg::str(c_str(p))));         // 0x00620b40
        void* const tex = joan::orig::as_texture_manager(kTextures)->load(joan::orig::arg::str(def + 9), i32{1}, joan::orig::arg::u32(i32{0}));  // 0x006099c0
        if (outbox) {
            const u32 color = make_color();
            draw(x, y, d::s_outbox, color, c);  // '!outbox'
        }
        {
            const u32 color = make_color();
            jo::BmpRes* const owner = jo::self<jo::MiniMap>(this)->icons;
            const char* const name = c_str(p);
            const i32 iy = ftol(y);
            const i32 ix = ftol(x);
            joan::orig::as_sprite_queue(kCanvas)->draw_named(joan::orig::arg::i16(ix), joan::orig::arg::i16(iy), 9u, joan::orig::arg::str(name), joan::orig::arg::mptr(owner), joan::orig::arg::u32(color), 3, 0, 0, 0, joan::orig::arg::i16(c[0]), joan::orig::arg::i16(c[1]), joan::orig::arg::i16(c[2]), joan::orig::arg::i16(c[3]), 0, -1, -1);  // 0x00633500
        }
        joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(tex));  // 0x0060a550
    }

    void attack(float x, float y) {
        // z: the original's never-written [ebp-0x1b8], not read by the callee
        joan::orig::as_map(this)->attack_marker_718140(x, y, 0u);  // 0x00718140
    }

    // ------------------------------------------------------------------ 0x0070fbe0
    void draw_unit_70fbe0(u8* unit) {
        c_70fbe0.hit();
        if (jo::self<jo::MiniMap>(this)->active == 0 || unit == nullptr) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_a45f08))->empty_at_561320(static_cast<i32>(jo::self<jo::MiniMap>(this)->x))) != 0) return;  // 0x00561320
        if (jo::as_unit(unit)->idle_time == 1 && jo::self<jo::MiniMap>(this)->blink_on != 0) return;
        u8* rec = nullptr;  // [ebp-0x8]
        Path path;          // [ebp-0x1a8]
        joan::orig::as_string(&path)->construct();  // 0x005a1170
        float px = jo::as_unit(unit)->position_[0];
        float py = jo::as_unit(unit)->position_[1];
        px = static_cast<float>(ftol(fpu.add(px, 0.5)));  // fadd qword 0.5; _ftol; fild; fstp
        py = static_cast<float>(ftol(fpu.add(py, 0.5)));
        px = px - jo::self<jo::MiniMap>(this)->scale_x;
        py = jo::self<jo::MiniMap>(this)->scale_y - py;
        {
            const i32 mode = jo::self<jo::MiniMap>(this)->visible;
            if (mode == 0 || mode == 1) {
                const jo::MiniRes* const m = jo::self<jo::MiniMap>(this)->view;
                const i32 jo::MiniRes::* const ox = mode == 0 ? &jo::MiniRes::i32_21 : &jo::MiniRes::i32_29;  // the kind's cell size
                const i32 jo::MiniRes::* const oy = mode == 0 ? &jo::MiniRes::i32_25 : &jo::MiniRes::i32_2d;
                if (!(static_cast<double>(m->*ox) >= px)) return;
                if (!(static_cast<double>(m->*oy) >= py)) return;
                if (!(px >= 0.0f)) return;
                if (!(py >= 0.0f)) return;
            }
        }
        u8* const grp = joan::orig::arg::as<u8*>(joan::orig::as_unit(unit)->nation());  // 0x00509400
        if (grp != nullptr)
            rec = joan::orig::arg::as<u8*>(joan::orig::as_player(grp)->relation(static_cast<i32>(jo::self<jo::MiniMap>(this)->nation->id)));  // 0x004b42e0
        const i32 sw = jo::self<jo::MiniMap>(this)->width;
        const i32 sh = jo::self<jo::MiniMap>(this)->height;
        // [ebp-0x1c4] / [ebp-0x1c8]: +0xf is always 0 or 1 (see the header); 0 otherwise.
        float mx = 0.0f;
        float my = 0.0f;
        {
            const i32 mode = jo::self<jo::MiniMap>(this)->visible;
            if (mode == 0 || mode == 1) {
                const i32 jo::MiniRes::* const ox = mode == 0 ? &jo::MiniRes::i32_21 : &jo::MiniRes::i32_29;
                const i32 jo::MiniRes::* const oy = mode == 0 ? &jo::MiniRes::i32_25 : &jo::MiniRes::i32_2d;
                mx = static_cast<float>(
                    fpu.div(fpu.mul(static_cast<double>(sw), px), static_cast<double>(jo::self<jo::MiniMap>(this)->view->*ox)));
                my = static_cast<float>(
                    fpu.div(fpu.mul(static_cast<double>(sh), py), static_cast<double>(jo::self<jo::MiniMap>(this)->view->*oy)));
            }
        }
        {
            const i32 iy = ftol(my);
            const i32 ix = ftol(mx);
            const i32 idx = static_cast<i32>(static_cast<u32>(iy) * 0xc8u + static_cast<u32>(ix));
            if (jo::view<i8>(jo::self<jo::MiniMap>(this)->fog[idx]) == 0) return;  // fog of war
        }
        i32 rot = static_cast<i32>(jo::self<jo::MiniMap>(this)->rotation) % 4;
        // fix/uninit: a negative remainder matches no case of the jump table 0x0071327f and the
        // original keeps the uninitialised [ebp-0x1c0] / [ebp-0x1bc]; use rotation + 4 instead.
        if (rot < 0) rot += 4;
        float x = 0.0f;  // [ebp-0x1c0]
        float y = 0.0f;  // [ebp-0x1bc]
        {
            const u32 w = global<u32>(d::g_screen_w);
            switch (rot) {
                case 0:
                    x = static_cast<float>(fpu.add(static_cast<double>(static_cast<i32>(w - 0xd3u)), mx));
                    y = static_cast<float>(fpu.add(10.0, my));
                    break;
                case 1:
                    x = static_cast<float>(fpu.add(fpu.add(static_cast<double>(static_cast<i32>(w - 0xd3u)), my), 0.0));
                    y = static_cast<float>(fpu.add(fpu.sub(10.0, mx), 200.0));
                    break;
                case 2:
                    x = static_cast<float>(fpu.sub(static_cast<double>(static_cast<i32>(w - 0xbu)), mx));
                    y = static_cast<float>(fpu.sub(210.0, my));
                    break;
                default:  // 3
                    x = static_cast<float>(fpu.add(fpu.sub(static_cast<double>(static_cast<i32>(w - 0xd3u)), my), 199.5));
                    y = static_cast<float>(fpu.add(fpu.add(10.0, mx), 0.0));
                    break;
            }
        }
        x = joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->round_decimals(joan::orig::arg::f32(x), i32{0}, i32{0}));  // 0x00590740
        y = joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->round_decimals(joan::orig::arg::f32(y), i32{0}, i32{0}));

        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::MiniMap>(this)->unit_id)))) != 0) {  // 0x005094d0
            if (jo::as_unit(unit)->rank == 4) {
                u8* const g = group_of(unit);
                if (jo::as_unit_type(g)->unit_class == 8 && has_flag(unit, 1) == 0 && has_flag(unit, 2) == 0) {
                    dot(x, y, global<u32>(d::g_a6a5a0));
                    return;
                }
                Path enemy;                                                    // [ebp-0x360]
                joan::orig::as_string(&enemy)->construct(str(d::s_ENEMY));              // 0x005a11a0 '!ENEMY'
                marker(enemy, 7.0f, x, y, false);
                return;
            }
            u8* const g = group_of(unit);
            if (jo::as_unit_type(g)->unit_class == 8 && has_flag(unit, 1) == 0 && has_flag(unit, 2) == 0)
                dot(x, y, global<u32>(d::g_a6a5a0));
            else
                dot(x, y, global<u32>(d::g_a6a5a8));
            return;
        }

        if (jo::as_unit(unit)->id != jo::self<jo::MiniMap>(this)->unit_id) {
            const i32 kind = jo::as_unit(unit)->side;  // loaded before the call, as in the original
            u8* const leader = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(jo::self<jo::MiniMap>(this)->unit_id)));  // 0x0041b3c0
            if (kind == static_cast<i32>(jo::as_unit(leader)->side)) {
                if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(unit)->use_spiritual_item_4fd980(i32{0}))) != 0) {  // 0x004fd980
                    attack(x, y);
                    return;
                }
                for (i32 i = 0; i < 8; ++i) {
                    u8* const g = group_of(unit);
                    if (joan::crt::stricmp(joan::orig::arg::str(jo::as_unit_type(g)->name), joan::orig::arg::str(str(kHeroes[i][0]))) != 0) continue;  // _stricmp
                    const bool h = jo::view<jo::TaskSlot>(jo::as_unit(unit)->task).kind == 0 && jo::view<i32>(jo::view<jo::TaskSlot>(jo::as_unit(unit)->task).target) != 0;
                    joan::orig::as_string(&path)->assign(joan::orig::arg::str(str(h ? kHeroes[i][1] : kHeroes[i][2])));  // 0x005a1440
                    marker(path, 8.0f, x, y, true);
                    return;
                }
                if (jo::as_unit(unit)->rank >= 3) {
                    joan::orig::as_string(&path)->assign(str(d::s_capt));          // 0x005a1440 '!capt'
                    marker(path, 8.0f, x, y, true);
                    return;
                }
                if (jo::as_unit_type(group_of(unit))->unit_class == 0) {
                    dot(x, y, global<u32>(d::g_a6a5b8));
                    return;
                }
                if (jo::as_unit_type(group_of(unit))->unit_class != 8) {
                    dot(x, y, global<u32>(d::g_a6a5b4));
                    return;
                }
                if (has_flag(unit, 1) == 0 && has_flag(unit, 2) == 0) {
                    dot(x, y, global<u32>(d::g_a6a5a0));
                    return;
                }
                Path siege;                                           // [ebp-0x63c]
                joan::orig::as_string(&siege)->construct(str(d::s_siege));    // 0x005a11a0 '!siege'
                marker(siege, 7.0f, x, y, false);
                return;
            }
        }
        // 0x00712f84
        if (jo::as_unit(unit)->id != jo::self<jo::MiniMap>(this)->unit_id && rec != nullptr && static_cast<i8>(rec[3]) == 4) {
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(unit)->use_spiritual_item_4fd980(i32{0}))) != 0)  // 0x004fd980
                attack(x, y);
            else
                dot(x, y, global<u32>(d::g_a6a59c));
        }
        // 0x0071306c: returns on both outcomes of the unit == leader test (the arrow code behind
        // it is an `if (0)`).
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: negative overlay rotation -> rotation + 4 instead of uninitialised marker coordinates.
FIX("MapOverlay::draw_unit_70fbe0", 0x0070fbe0, &H::draw_unit_70fbe0);
#undef FIX

}  // namespace game_b111_06
}  // namespace

// ---------------------------------------- from modules/game_b110_16.cpp (game/b110_16)
namespace {
namespace game_b110_16 {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::x87::Fpu;

const Fpu fpu{true};

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}
using joan::hook::x87::ftol;

using joan::call::thiscall;

// 0x0042a260 (thunk 0x004098b8, ret 0x10): builds a 4-byte colour in `c`, returns &c.
u32 make_color(i32 a, i32 b, i32 c, i32 d) {
    u32 color = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&color)->construct(a, b, c, d));
}

JOAN_COUNTER(c_tiles, "Minimap::draw_tiles_7141a0");
JOAN_COUNTER(c_tiles_fallback, "Minimap::draw_tiles_7141a0 fallback");
JOAN_COUNTER(c_tiles_fix, "Minimap::draw_tiles_7141a0 (fix/uninit: position 0)");

struct H {
    // =================================================================== 0x007141a0 helpers
    // 0x00420240 (group of the minimap owner +8) + 0x61, then 0x00620b40(tile): the tile record.
    void* tile_record(void* tile) {
        u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_a45f08))->get_420240(static_cast<i32>(jo::self<jo::MiniMap>(this)->x)));  // 0x00420240
        return joan::orig::arg::as<void*>(joan::orig::as_bmp_res(&jo::as_world(g)->bitmaps)->find(joan::orig::arg::str(tile)));                                            // 0x00620b40
    }
    // 0x006099c0(record + 9, 1, 0) on 0xa403a0.
    void* tile_frame(void* tile) {
        u8* const rec = static_cast<u8*>(tile_record(tile));
        return joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(jo::as_bmp_entry(rec)->file), i32{1}, joan::orig::arg::u32(i32{0}));  // 0x006099c0
    }
    // 0x00633500 on 0xa46128 with the 4 clip values (args 11..14).
    void blit(void* tile, i32 x, i32 y, u32 color, i32 a11, i32 a12, i32 a13, i32 a14) {
        const i32 rot = jo::self<jo::MiniMap>(this)->rotation;
        u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_a45f08))->get_420240(static_cast<i32>(jo::self<jo::MiniMap>(this)->x)));  // 0x00420240
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 0xau, joan::orig::arg::str(tile), joan::orig::arg::mptr(&jo::as_world(g)->bitmaps), joan::orig::arg::u32(color), 3, 0, 0, 0, joan::orig::arg::i16(a11), joan::orig::arg::i16(a12), joan::orig::arg::i16(a13), joan::orig::arg::i16(a14), joan::orig::arg::i16(rot), -1, -1);  // 0x00633500
    }
    void release(void* frame) { joan::orig::as_texture_manager(obj(d::g_texture_array))->release(frame); }  // 0x0060a550
    void* neighbour(i32 tx, i32 ty) {
        return joan::orig::arg::as<void*>(joan::orig::as_mini_res(jo::self<jo::MiniMap>(this)->view)->find_frame(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::MiniMap>(this)->visible)), joan::orig::arg::i16(tx), joan::orig::arg::i16(ty)));  // 0x0071b3b0
    }
    i32 rot4() { return static_cast<i32>(jo::self<jo::MiniMap>(this)->rotation) % 4; }  // and 0x80000003 sequence: signed remainder

    // ------------------------------------------------------------------ 0x007141a0
    u32 draw_tiles_7141a0() {
        if (rot4() < 0) {  // the original reads uninitialised blit coordinates
            if (!fix_uninit()) {
                c_tiles_fallback.hit();
                return thiscall<u32>(fn::Minimap_draw_tiles_7141a0_body, this);  // the untouched original body
            }
            c_tiles_fix.hit();  // fix/uninit: x4c / x50 stay 0 (never written for a negative remainder)
        }
        c_tiles.hit();
        const u32 prev = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(obj(d::g_texture_array))->mode());  // 0x0060a720
        joan::orig::as_texture_manager(obj(d::g_texture_array))->set_mode(joan::orig::arg::u32(i32{1}));            // 0x0060a740
        const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
        {
            const u32 r = joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_a45f08))->empty_at_561320(static_cast<i32>(jo::self<jo::MiniMap>(this)->x)));  // 0x00561320
            if (r != 0) return r;
        }
        u32 pos[3];
        joan::orig::as_unit(jo::self<jo::MiniMap>(this)->unit)->position(joan::orig::arg::mptr(static_cast<void*>(pos)));  // 0x004fedb0
        const float x = joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->round_decimals(joan::orig::arg::f32(pos[0]), i32{0}, i32{0}));  // 0x00590740
        const float y = joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->round_decimals(joan::orig::arg::f32(pos[1]), i32{0}, i32{0}));
        const i32 iy = ftol(y);
        const i32 ix = ftol(x);
        void* tile = joan::orig::arg::as<void*>(joan::orig::as_zones(jo::self<jo::MiniMap>(this)->view)->find_71b520(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::MiniMap>(this)->visible)), joan::orig::arg::i32(ix), joan::orig::arg::i32(iy), i32{0}));  // 0x0071b520
        void* rec = tile_record(tile);
        const i32 tx = jo::as_mini_res_frame(tile)->sub_a;
        const i32 ty = jo::as_mini_res_frame(tile)->sub_b;
        float fx, fy;
        {
            const i32 x0 = jo::as_mini_res_frame(tile)->x0;
            const i32 span = static_cast<i32>(static_cast<u32>(jo::as_mini_res_frame(tile)->x1) - static_cast<u32>(x0));
            const double v = fpu.div(fpu.mul(static_cast<double>(jo::view<i16>(jo::as_bmp_entry(rec)->width)),
                                             fpu.sub(static_cast<double>(x), static_cast<double>(x0))),
                                     static_cast<double>(span));
            fx = static_cast<float>(static_cast<i16>(ftol(v)));
        }
        {
            const i32 y0 = jo::as_mini_res_frame(tile)->y0;
            const i32 span = static_cast<i32>(static_cast<u32>(y0) - static_cast<u32>(jo::as_mini_res_frame(tile)->y1));
            const double v = fpu.div(fpu.mul(static_cast<double>(jo::view<i16>(jo::as_bmp_entry(rec)->height)),
                                             fpu.sub(static_cast<double>(y0), static_cast<double>(y))),
                                     static_cast<double>(span));
            fy = static_cast<float>(static_cast<i16>(ftol(v)));
        }
        const i32 x34 = static_cast<i16>(ftol(fx - 100.0f));
        const i32 x38 = static_cast<i16>(ftol(fy - 100.0f));
        const float fx2 = fx + 100.0f;
        const float fy2 = fy + 100.0f;
        const i32 x3c = static_cast<i16>(ftol(fx2 - 1.0f));
        const i32 x40 = static_cast<i16>(ftol(fy2 - 1.0f));
        const auto W = [] { return global<i32>(d::g_screen_w); };
        const i32 x44 = x34 < 0 ? W() - 0xd3 - x34 : W() - 0xd3;
        const i32 x48 = x38 < 0 ? 0xa - x38 : 0xa;
        i32 x4c = 0, x50 = 0;  // keep their values across switches (negative remainders; fix/uninit: 0)
        const auto r34 = [&] { return static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(rec)->width)); };
        const auto r36 = [&] { return static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(rec)->height)); };

        // centre tile
        switch (rot4()) {
            case 0: x4c = x44; x50 = x48; break;
            case 1: x4c = x38 < 0 ? W() - 0xd3 - x38 : W() - 0xd3; x50 = x34 < 0 ? 0xa : x34 + 0xa; break;
            case 2: x4c = x34 < 0 ? W() - 0xd3 : W() + x34 - 0xd3; x50 = x38 < 0 ? 0xa : x38 + 0xa; break;
            case 3: x4c = x38 < 0 ? W() - 0xd3 : W() + x38 - 0xd3; x50 = x34 < 0 ? 0xa - x34 : 0xa; break;
            default: break;
        }
        {
            void* const frame = tile_frame(tile);
            const i32 bc = x40 < r36() - 1 ? x40 : r36() - 1;
            const i32 c0 = x3c < r34() - 1 ? x3c : r34() - 1;
            const i32 c4 = x38 > 0 ? x38 : 0;
            const i32 c8 = x34 > 0 ? x34 : 0;
            blit(tile, x4c, x50, color, c8, c4, c0, bc);
            release(frame);
        }
        // (tx, ty - 1)
        if (x34 < 0) {
            tile = neighbour(tx, ty - 1);
            if (tile != nullptr) {
                rec = tile_record(tile);
                void* const frame = tile_frame(tile);
                switch (rot4()) {
                    case 0: x4c = W() - 0xd3; x50 = x48; break;
                    case 1: x4c = x38 < 0 ? W() - 0xd3 - x38 : W() - 0xd3; x50 = x34 + 0xd2; break;
                    case 2: x4c = W() + x34 - 0xb; x50 = x38 < 0 ? 0xa : x38 + 0xa; break;
                    case 3: x4c = x38 < 0 ? W() - 0xd3 : W() + x38 - 0xd3; x50 = 0xa; break;
                    default: break;
                }
                const i32 d0 = x40 < r36() - 1 ? x40 : r36() - 1;
                const i32 d4 = x38 > 0 ? x38 : 0;
                blit(tile, x4c, x50, color, r34() + x34, d4, r34() - 1, d0);
                release(frame);
            }
        }
        // (tx - 1, ty)
        if (x38 < 0) {
            tile = neighbour(tx - 1, ty);
            if (tile != nullptr) {
                rec = tile_record(tile);
                void* const frame = tile_frame(tile);
                switch (rot4()) {
                    case 0: x4c = x44; x50 = 0xa; break;
                    case 1: x4c = W() - 0xd3; x50 = x34 < 0 ? 0xa : x34 + 0xa; break;
                    case 2: x4c = x34 < 0 ? W() - 0xd3 : W() + x34 - 0xd3; x50 = x38 + 0xd2; break;
                    case 3: x4c = W() + x38 - 0xb; x50 = x34 < 0 ? 0xa - x34 : 0xa; break;
                    default: break;
                }
                const i32 dc = x3c < r34() - 1 ? x3c : r34() - 1;
                const i32 e0 = x34 > 0 ? x34 : 0;
                blit(tile, x4c, x50, color, e0, r36() + x38, dc, r36() - 1);
                release(frame);
            }
        }
        // (tx + 1, ty): tested against the current record (possibly a neighbour's), as the original
        if (!(x40 < r36())) {
            tile = neighbour(tx + 1, ty);
            if (tile != nullptr) {
                rec = tile_record(tile);
                void* const frame = tile_frame(tile);
                switch (rot4()) {
                    case 0: x4c = x44; x50 = x48 + (r36() - x38); break;
                    case 1: x4c = W() + (r36() - x38) - 0xd3; x50 = x34 < 0 ? 0xa : x34 + 0xa; break;
                    case 2: x4c = x34 < 0 ? W() - 0xd3 : W() + x34 - 0xd3; x50 = 0xa; break;
                    case 3: x4c = W() - 0xd3; x50 = x34 < 0 ? 0xa - x34 : 0xa; break;
                    default: break;
                }
                const i32 e8 = x3c < r34() - 1 ? x3c : r34() - 1;
                const i32 ec = x34 > 0 ? x34 : 0;
                blit(tile, x4c, x50, color, ec, 0, e8, x40 - r36());
                release(frame);
            }
        }
        // (tx, ty + 1)
        if (!(x3c < r34())) {
            tile = neighbour(tx, ty + 1);
            if (tile != nullptr) {
                rec = tile_record(tile);
                void* const frame = tile_frame(tile);
                switch (rot4()) {
                    case 0: x4c = x44 + (r34() - x34); x50 = x48; break;
                    case 1: x4c = x38 < 0 ? W() - 0xd3 - x38 : W() - 0xd3; x50 = 0xa; break;
                    case 2: x4c = W() - 0xd3; x50 = x38 < 0 ? 0xa : x38 + 0xa; break;
                    case 3: x4c = x38 < 0 ? W() - 0xd3 : W() + x38 - 0xd3; x50 = r34() + 0xa - x34; break;
                    default: break;
                }
                const i32 f4 = x40 < r36() - 1 ? x40 : r36() - 1;
                const i32 f8 = x38 > 0 ? x38 : 0;
                blit(tile, x4c, x50, color, 0, f8, x3c - r34(), f4);
                release(frame);
            }
        }
        // (tx - 1, ty - 1)
        if (x34 < 0 && x38 < 0) {
            tile = neighbour(tx - 1, ty - 1);
            if (tile != nullptr) {
                rec = tile_record(tile);
                void* const frame = tile_frame(tile);
                switch (rot4()) {
                    case 0: x4c = W() - 0xd3; x50 = 0xa; break;
                    case 1: x4c = W() - 0xd3; x50 = x34 + 0xd2; break;
                    case 2: x4c = W() + x34 - 0xb; x50 = x38 + 0xd2; break;
                    case 3: x4c = W() + x38 - 0xb; x50 = 0xa; break;
                    default: break;
                }
                blit(tile, x4c, x50, color, r34() + x34, r36() + x38, r34() - 1, r36() - 1);
                release(frame);
            }
        }
        // (tx - 1, ty + 1)
        if (x38 < 0 && !(x3c < r34())) {
            tile = neighbour(tx - 1, ty + 1);
            if (tile != nullptr) {
                rec = tile_record(tile);
                void* const frame = tile_frame(tile);
                switch (rot4()) {
                    case 0: x4c = x44 + (r34() - x34); x50 = 0xa; break;
                    case 1: x4c = W() - 0xd3; x50 = 0xa; break;
                    case 2: x4c = W() - 0xd3; x50 = x38 + 0xd2; break;
                    case 3: x4c = W() + x38 - 0xb; x50 = 0xd2 - x34; break;
                    default: break;
                }
                blit(tile, x4c, x50, color, 0, r36() + x38, x3c - r34(), r36() - 1);
                release(frame);
            }
        }
        // (tx + 1, ty - 1)
        if (!(x40 < r36()) && x34 < 0) {
            tile = neighbour(tx + 1, ty - 1);
            if (tile != nullptr) {
                rec = tile_record(tile);
                void* const frame = tile_frame(tile);
                switch (rot4()) {
                    case 0: x4c = W() - 0xd3; x50 = x48 + (r36() - x38); break;
                    case 1: x4c = W() + (r36() - x38) - 0xd3; x50 = x34 + 0xd2; break;
                    case 2: x4c = W() + x34 - 0xb; x50 = 0xa; break;
                    case 3: x4c = W() - 0xd3; x50 = 0xa; break;
                    default: break;
                }
                blit(tile, x4c, x50, color, r34() + x34, 0, r34() - 1, x40 - r36());
                release(frame);
            }
        }
        // (tx + 1, ty + 1)
        if (!(x40 < r36()) && !(x3c < r34())) {
            tile = neighbour(tx + 1, ty + 1);
            if (tile != nullptr) {
                rec = tile_record(tile);
                void* const frame = tile_frame(tile);
                switch (rot4()) {
                    case 0: x4c = x44 + (r34() - x34); x50 = x48 + (r36() - x38); break;
                    case 1: x4c = W() + (r36() - x38) - 0xd3; x50 = 0xa; break;
                    case 2: x4c = W() - 0xd3; x50 = 0xa; break;
                    case 3: x4c = W() - 0xd3; x50 = r34() - x34 + 0xa; break;
                    default: break;
                }
                blit(tile, x4c, x50, color, 0, 0, x3c - r34(), x40 - r36());
                release(frame);
            }
        }
        joan::orig::as_texture_manager(obj(d::g_texture_array))->set_mode_60a740(joan::orig::arg::u32(prev));  // 0x0060a740
        return 0;  // Textures::set_mode returns nothing; callers (0x0070e586, Minimap::draw_70e500) discard the result
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b110_16", name, addr, fn)
B("Minimap::draw_tiles_7141a0", 0x007141a0, &H::draw_tiles_7141a0);
#undef B

}  // namespace game_b110_16
}  // namespace

// ---------------------------------------- from modules/game_b115_10.cpp (fix/uninit)
namespace {
namespace game_b115_10 {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::hook::x87::Fpu;

using joan::orig::helpers::addr;

constexpr Fpu kFpu{true};

using joan::hook::x87::ftol;

using joan::call::thiscall;
i32 sx(i16 v) { return static_cast<i32>(v); }
i32 sx(u16 v) { return static_cast<i32>(static_cast<i16>(v)); }
i32 sb(u8 v) { return static_cast<i32>(static_cast<i8>(v)); }
using joan::orig::helpers::fild;
using joan::orig::helpers::fd;

struct V3 {
    u32 d[3];
};
static_assert(sizeof(V3) == 12, "V3");

JOAN_COUNTER(c_markers, "Minimap::draw_markers_716010");

struct H {
    // ===================================================================== 0x00716010
    void draw_markers(i32 lit) {
        c_markers.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_a45f08))->empty_at_561320(joan::orig::arg::i32(sx(jo::self<jo::MiniMap>(this)->x)))) != 0) return;  // 0x00561320
        // fix: these slots start at 0 and keep their last value across markers (see header)
        float pos[3] = {0.0f, 0.0f, 0.0f};  // [ebp-0x18]
        float ox = 0.0f, oy = 0.0f;         // [ebp-0x34], [ebp-0x38]
        float px_f = 0.0f, py_f = 0.0f;     // [ebp-0x30], [ebp-0x2c]
        float scx = 0.0f, scy = 0.0f;       // [ebp-0x40], [ebp-0x44]
        auto info = [&]() { return jo::self<jo::MiniMap>(this)->view; };
        const auto W0 = []() { return global<i32>(d::g_screen_w); };
        for (i32 i = 0; i < jo::as_mini_res(info())->map_count; ++i) {
            jo::MiniResMap* const e = &jo::as_mini_res(info())->maps[i];
            if (sb(e->enabled) == 0) continue;
            if ((sb(e->no_mesh) != 0) != (lit != 0)) continue;
            switch (sx(e->kind)) {
            case 0:
                joan::orig::as_screen(this)->offset(joan::orig::arg::mptr(pos), jo::view<u32>(e->pos[0]), jo::view<u32>(e->pos[1]), jo::view<u32>(e->pos[2]));  // 0x0070fa70
                break;
            case 1: {
                const u32 k = joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_units))->find_name_51fdd0(joan::orig::arg::str(e->unit_name)));  // 0x0051fdd0
                if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(k)) != 0) continue;  // 0x00428b70
                joan::orig::as_unit(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(k)))->position(joan::orig::arg::mptr(pos));  // 0x0041b3c0 / 0x004fedb0
                const float a = joan::orig::arg::as<float>(joan::orig::as_ground_object(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(k)))->heading());  // 0x004ff850
                e->f32_97 = static_cast<float>(kFpu.div(a, fd(d::flt_0_017453292)));
                V3 v;
                std::memcpy(&v, pos, 12);
                joan::orig::as_screen(this)->offset(joan::orig::arg::mptr(pos), v.d[0], v.d[1], v.d[2]);  // 0x0070fa70
                break;
            }
            case 2: {
                const u32 h0 = joan::orig::arg::as<u32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id());  // 0x004203c0
                if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(h0)) != 0) continue;
                const u32 h1 = joan::orig::arg::as<u32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id());
                joan::orig::as_unit(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(h1)))->position(joan::orig::arg::mptr(pos));
                const u32 h2 = joan::orig::arg::as<u32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id());
                const float a = joan::orig::arg::as<float>(joan::orig::as_ground_object(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(h2)))->heading());
                e->f32_97 = static_cast<float>(kFpu.div(a, fd(d::flt_0_017453292)));
                V3 v;
                std::memcpy(&v, pos, 12);
                joan::orig::as_screen(this)->offset(joan::orig::arg::mptr(pos), v.d[0], v.d[1], v.d[2]);  // 0x0070fa70
                break;
            }
            default:
                break;
            }
            const i32 mode = sx(jo::self<jo::MiniMap>(this)->visible);
            if (mode == 0 || mode == 1) {
                const i32 jo::MiniRes::* const ow = mode == 0 ? &jo::MiniRes::i32_21 : &jo::MiniRes::i32_29;  // the kind's cell size
                const i32 jo::MiniRes::* const oh = mode == 0 ? &jo::MiniRes::i32_25 : &jo::MiniRes::i32_2d;
                if (!(fild(info()->*ow) >= static_cast<double>(pos[0]))) continue;
                if (!(fild(info()->*oh) >= static_cast<double>(pos[1]))) continue;
                if (!(pos[0] >= 0.0f)) continue;
                if (!(pos[1] >= 0.0f)) continue;
            }
            const i32 wx = sx(jo::self<jo::MiniMap>(this)->width);
            const i32 wy = sx(jo::self<jo::MiniMap>(this)->height);
            if (mode == 0 || mode == 1) {
                const i32 jo::MiniRes::* const ow = mode == 0 ? &jo::MiniRes::i32_21 : &jo::MiniRes::i32_29;
                const i32 jo::MiniRes::* const oh = mode == 0 ? &jo::MiniRes::i32_25 : &jo::MiniRes::i32_2d;
                ox = static_cast<float>(kFpu.div(kFpu.mul(fild(wx), pos[0]), fild(info()->*ow)));
                oy = static_cast<float>(kFpu.div(kFpu.mul(fild(wy), pos[1]), fild(info()->*oh)));
            }
            i32 rot = sx(jo::self<jo::MiniMap>(this)->rotation) % 4;
            if (rot < 0) rot += 4;  // fix: negative rotations use the matching case (see header)
            switch (rot) {
            case 0:
                px_f = static_cast<float>(kFpu.add(fild(W0() - 0xd3), ox));
                py_f = static_cast<float>(kFpu.add(fd(d::flt_10), oy));
                break;
            case 1:
                px_f = static_cast<float>(kFpu.add(kFpu.add(fild(W0() - 0xd3), oy), fd(d::k_9cf5c4)));
                py_f = static_cast<float>(kFpu.add(kFpu.sub(fd(d::flt_10), ox), fd(d::flt_200)));
                break;
            case 2:
                px_f = static_cast<float>(kFpu.sub(fild(W0() - 0xb), ox));
                py_f = static_cast<float>(kFpu.sub(fd(d::flt_210), oy));
                break;
            default:
                px_f = static_cast<float>(kFpu.add(kFpu.sub(fild(W0() - 0xd3), oy), fd(d::flt_199_5)));
                py_f = static_cast<float>(kFpu.add(kFpu.add(fd(d::flt_10), ox), fd(d::k_9cf5c4)));
                break;
            }
            if (sb(e->no_mesh) != 0) {
                const u32 tick = (global<u32>(d::g_frame_step_ms) << 9) / 1000u;
                e->u32_a2 = (tick + e->u32_a2) % 0x200u;
                u8* const spr = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::self<jo::MiniMap>(this)->icons)->find(joan::orig::arg::str(e->icon)));  // 0x00620b40
                const i32 mode2 = sx(jo::self<jo::MiniMap>(this)->visible);  // re-read after 0x00620b40 like the original
                if (mode2 == 0) {
                    scx = 1.0f;
                    scy = 1.0f;
                } else if (mode2 == 1) {
                    scx = static_cast<float>(jo::as_mini_res(info())->i32_21 / jo::as_mini_res(info())->i32_29);
                    scy = static_cast<float>(jo::as_mini_res(info())->i32_21 / jo::as_mini_res(info())->i32_29);
                }
                const double c2 = fd(d::flt_2);
                const double c1 = fd(d::flt_1);
                i32 px = ftol(kFpu.sub(px_f, kFpu.div(kFpu.mul(fild(sx(jo::as_bmp_entry(spr)->width)), scx), c2)));
                i32 py = ftol(kFpu.sub(py_f, kFpu.div(kFpu.mul(fild(sx(jo::as_bmp_entry(spr)->height)), scy), c2)));
                i32 x2 = px + 0xc7;
                i32 y2 = py + 0xc7;
                if (x2 < W0() - 0xd3 || y2 < 0xa || px > W0() - 0xc || py > 0xd1) continue;
                i32 cl = 0;
                i32 ct = 0;
                i32 w = ftol(kFpu.sub(kFpu.mul(fild(sx(jo::as_bmp_entry(spr)->width)), scx), c1));
                i32 h = ftol(kFpu.sub(kFpu.mul(fild(sx(jo::as_bmp_entry(spr)->height)), scy), c1));
                if (px < W0() - 0xd3) {
                    cl = ftol(kFpu.div(fild(W0() - 0xd3 - px), scx));
                    px = W0() - 0xd3;
                }
                if (py < 0xa) {
                    ct = ftol(kFpu.div(fild(0xa - py), scx));
                    py = 0xa;
                }
                if (x2 > W0() - 0xc) {
                    w = ftol(kFpu.sub(fild(w), kFpu.div(fild(W0() - 0xc - x2), scx)));
                    x2 = 0xd1;
                }
                if (y2 > 0xd1) {
                    h = ftol(kFpu.sub(fild(h), kFpu.div(fild(0xd1 - y2), scy)));
                    y2 = 0xd1;
                }
                const i32 alpha = jo::view<i32>(e->u32_a2) > 0x100 ? 0x200 - jo::view<i32>(e->u32_a2) : jo::view<i32>(e->u32_a2);
                void* const tex = joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(jo::as_bmp_entry(spr)->file), 1, 0u);  // 0x006099c0
                const u32 r6c = static_cast<u32>(addr(tex));  // its low dword: the stale high words below (only bits 16..31 / the low bytes reach the callees)
                u32 colour_obj = 0;
                // eax pushed for the alpha byte: r6c's high word, w's second byte (mov ax / mov al)
                const u32 alpha_arg = (r6c & 0xffff0000u) | (static_cast<u32>(w) & 0xff00u) | (static_cast<u32>(alpha) & 0xffu);
                const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&colour_obj)->construct(0xffu, 0xffu, 0xffu, alpha_arg));  // 0x0042a260
                auto lo = [](i32 v) { return static_cast<u32>(static_cast<u16>(v)); };
                const u32 hi_r = r6c & 0xffff0000u;
                joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16((joan::orig::arg::as<u32>(jo::self<jo::MiniMap>(this)->icons) & 0xffff0000u) | lo(px)), joan::orig::arg::i16((addr(this) & 0xffff0000u) | lo(py)), 0xau, joan::orig::arg::str(spr), joan::orig::arg::mptr(jo::self<jo::MiniMap>(this)->icons), joan::orig::arg::u32(colour), 3, 0, 2, 0, joan::orig::arg::i16(static_cast<u32>(cl)), joan::orig::arg::i16(static_cast<u32>(ct)), joan::orig::arg::i16(hi_r | lo(w)), joan::orig::arg::i16(static_cast<u32>(h)), 0, joan::orig::arg::i16(static_cast<u32>(x2)), joan::orig::arg::i16(hi_r | lo(y2)));  // 0x00633500 (word args read as words)
                joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(tex));  // 0x0060a550
                continue;
            }
            const float rad = e->radius;
            if (!(fild(W0() - 0xb) >= kFpu.add(px_f, rad))) continue;
            if (kFpu.add(py_f, rad) > fd(d::flt_210)) continue;
            if (fild(W0() - 0xd3) > kFpu.sub(px_f, rad)) continue;
            if (!(kFpu.sub(py_f, rad) >= fd(d::flt_10))) continue;  // 0x71681e: test ah,1 / je proceeds
            if (e->object == nullptr) continue;
            if (e->overlay == nullptr) continue;
            i32 sheet = 0xa;
            float ang = 0.0f;
            if (sx(e->kind) == 2) {
                const double a = kFpu.mul(kFpu.sub(-static_cast<double>(e->f32_97), fd(d::flt_180)), fd(d::flt_0_017453292));
                const double b = kFpu.mul(kFpu.mul(fild(sx(jo::self<jo::MiniMap>(this)->rotation)), fd(d::flt_90)), fd(d::flt_0_017453292));
                ang = static_cast<float>(kFpu.sub(a, b));
                sheet = 9;
            } else {
                ang = static_cast<float>(
                    kFpu.mul(kFpu.sub(-static_cast<double>(e->f32_97), fd(d::flt_270)), fd(d::flt_0_017453292)));
            }
            const i32 y = ftol(joan::orig::arg::as<double>(joan::orig::as_misc(obj(d::g_misc))->round_decimals(joan::orig::arg::f32(py_f), 0, 0)));  // 0x00590740
            const i32 x = ftol(joan::orig::arg::as<double>(joan::orig::as_misc(obj(d::g_misc))->round_decimals(joan::orig::arg::f32(px_f), 0, 0)));
            joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->place_model(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(sheet), joan::orig::arg::mptr(e->overlay), joan::orig::arg::bits(ang), 0u, 0u, joan::orig::arg::bits(1.0f));  // 0x006349b0
        }
    }
};

// 0x00716010: position / offset / screen point / scale never written for unhandled types / modes
// -> start at 0 (then last value); negative rotation remainder -> r + 4.
JOAN_REPLACE("fix/uninit", "Minimap::draw_markers_716010", 0x00716010, &H::draw_markers);

}  // namespace game_b115_10
}  // namespace

// ---------------------------------------- from modules/game_b111_19.cpp (game/b111_19)
namespace {
namespace game_b111_19 {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::call::cdecl_call;

using joan::hook::x87::Fpu;

using joan::orig::helpers::gf;  // float constant in .rdata

const Fpu fpu{true};

using joan::orig::helpers::wrap_sub;
using joan::orig::helpers::wrap_mul;

using joan::hook::x87::ftol;

using joan::call::thiscall;
using joan::orig::helpers::wrap_add;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");
struct alignas(4) Str {  // 0x005a11a0 string object: 0x190 chars + NUL (no destructor)
    char b[0x194];
};

JOAN_COUNTER(c_view, "MiniMap::draw_view_716be0");
JOAN_COUNTER(c_view_orig, "MiniMap::draw_view_716be0 (ran original: +0x2b % 4 < 0)");
JOAN_COUNTER(c_wp, "MiniMap::draw_waypoint_718540");
JOAN_COUNTER(c_wp_orig, "MiniMap::draw_waypoint_718540 (ran original: +0x2b / +0xf)");
JOAN_COUNTER(c_gold, "MiniMap::draw_gold_71aba0");
JOAN_COUNTER(c_gold_orig, "MiniMap::draw_gold_71aba0 (ran original: +0x2b / +0xf)");
JOAN_COUNTER(c_mm_fix, "MiniMap draw_view / draw_waypoint / draw_gold: never-written locals = 0 (fix/uninit)");

// ---- shared helpers -------------------------------------------------------------------------
// 0x00590740 on 0xa45fa8 (thunk 0x00404b01, ret 0xc): (value, 0, 0) -> st0, stored to a float.
float snap(float v) { return joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->round_decimals(joan::orig::arg::f32(v), i32{0}, i32{0})); }

using joan::orig::helpers::screen_w;

// The minimap rotation switch (short +0x2b % 4, cases 0..3 only; jump tables 0x7170c8 / 0x718e54
// / 0x71af00): map position (fx, fy) -> screen (sx, sy).
void rotate_to_screen(i32 r, float fx, float fy, float& sx, float& sy) {
    switch (r) {
        case 0:
            sx = static_cast<float>(fpu.add(static_cast<double>(wrap_sub(screen_w(), 0xd3)), fx));
            sy = gf(d::flt_10) + fy;  // 10
            break;
        case 1:
            sx = static_cast<float>(
                fpu.add(fpu.add(static_cast<double>(wrap_sub(screen_w(), 0xd3)), fy), gf(d::k_9cf5c4)));  // + 0
            sy = (gf(d::flt_10) - fx) + gf(d::flt_200);  // 10 - fx + 200
            break;
        case 2:
            sx = static_cast<float>(fpu.sub(static_cast<double>(wrap_sub(screen_w(), 0xb)), fx));
            sy = gf(d::flt_210) - fy;  // 210
            break;
        case 3:
            sx = static_cast<float>(
                fpu.add(fpu.sub(static_cast<double>(wrap_sub(screen_w(), 0xd3)), fy), gf(d::flt_199_5)));  // 199.5
            sy = (gf(d::flt_10) + fx) + gf(d::k_9cf5c4);
            break;
        default:
            break;  // negative remainder: sx / sy keep their value (never written in the original;
                    // fix/uninit: the callers' 0)
    }
}

// (fild(a) * v) / fidiv(d) at PC24.
float scale_div(i32 a, float v, i32 d) {
    return static_cast<float>(fpu.div(fpu.mul(static_cast<double>(a), v), static_cast<double>(d)));
}

i32 rot_of(const void* mm) { return static_cast<i32>(jo::as_mini_map(mm)->rotation) % 4; }  // MSVC remainder

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled):
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile i32 g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

// 0x0042a260 (thunk 0x004098b8, ret 0x10): 4-byte colour ctor (r, g, b, a); returns this.
u32 make_color(u32 r, u32 g, u32 b, u32 a) {
    u32 c = 0;
    const u32* p = joan::orig::arg::as<const u32*>(joan::orig::as_colour(&c)->construct(r, g, b, a));
    return *p;
}
// 0x005e9640 (thunk 0x00407577, ret 4): colour object from a packed dword; returns this.
u32 color_from(u32 argb) {
    u32 c = 0;
    const u32* p = joan::orig::arg::as<const u32*>(joan::orig::as_color(&c)->Color_dword(joan::orig::arg::u32(argb)));
    return *p;
}

// ---- fix/uninit: 0x00433af0 ------------------------------------------------------------------
struct H {
    // ---- 0x00716be0 --------------------------------------------------------------------------
    void draw_view() {
        if (rot_of(this) < 0) {  // the original would read never-written corners
            if (!fix_uninit()) {   // exact: run the original body
                c_view_orig.hit();
                thiscall<void>(fn::MiniMap_draw_view_716be0_body, this);
                return;
            }
            c_mm_fix.hit();  // fix/uninit: the never-written screen corners are 0 (screen[] below)
        }
        c_view.hit();
        if (jo::self<jo::MiniMap>(this)->active == 0) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_a45f08))->empty_at_561320(static_cast<i32>(jo::self<jo::MiniMap>(this)->x))) != 0) return;  // 0x00561320
        if (static_cast<i16>(joan::orig::arg::as<u16>(joan::orig::as_mini_map(this)->zoom_mode())) != 0) return;                               // 0x00717220
        if (global<i32>(d::g_control_mode) != 4) return;
        if (static_cast<i16>(joan::orig::arg::as<u16>(joan::orig::as_track(global<void*>(d::g_a5ba00))->is_type1())) != 0) return;  // 0x00428aa0
        Vec3 world[4] = {};   // [ebp-0x34 + 12 i] (z never written / read)
        Vec3 screen[4] = {};  // [ebp-0x64 + 12 i] (z never written; 0x00717f40 does not read it)
        float tx = 0.0f, ty = 0.0f;  // [ebp-0x74] / [ebp-0x78] (+0xf is 0 here)
        for (i32 i = 0; i < 4; ++i) {
            const Vec3 corner = global<Vec3>(d::g_a5ba04 + static_cast<std::uintptr_t>(i) * 12u);
            joan::orig::as_screen(this)->offset(&world[i], joan::orig::arg::bits(corner.x), joan::orig::arg::bits(corner.y), joan::orig::arg::bits(corner.z));  // 0x0070fa70
            const i32 a = jo::self<jo::MiniMap>(this)->width;
            const i32 b = jo::self<jo::MiniMap>(this)->height;
            const jo::MiniRes* const dims = jo::self<jo::MiniMap>(this)->view;
            const i32 mode = jo::self<jo::MiniMap>(this)->visible;
            if (mode == 0) {
                tx = scale_div(a, world[i].x, dims->i32_21);
                ty = scale_div(b, world[i].y, dims->i32_25);
            } else if (mode == 1) {
                tx = scale_div(a, world[i].x, dims->i32_29);
                ty = scale_div(b, world[i].y, dims->i32_2d);
            }
            rotate_to_screen(rot_of(this), tx, ty, screen[i].x, screen[i].y);
            screen[i].x = snap(screen[i].x);
            screen[i].y = snap(screen[i].y);
        }
        auto line = [&](const Vec3& p, const Vec3& q) {
            const u32 color = global<u32>(d::g_a6a5b0);
            joan::orig::as_mini_map(this)->draw_clipped_line(p.x, p.y, joan::orig::arg::bits(p.z), q.x, q.y, joan::orig::arg::bits(q.z), joan::orig::arg::u32(i32{8}), color);  // 0x00717f40
        };
        line(screen[0], screen[1]);
        line(screen[1], screen[3]);
        line(screen[2], screen[0]);
        line(screen[3], screen[2]);
    }

    // ---- 0x00718540 --------------------------------------------------------------------------
    void draw_waypoint(u32 idx_arg, Vec3 pos, i32* counter) {
        const i32 mode0 = jo::self<jo::MiniMap>(this)->visible;
        if (rot_of(this) < 0 || (mode0 != 0 && mode0 != 1)) {  // uninitialised locals in the original
            if (!fix_uninit()) {  // exact: run the original body
                c_wp_orig.hit();
                thiscall<void>(fn::MiniMap_draw_waypoint_718540_body, this, idx_arg, pos, counter);
                return;
            }
            c_mm_fix.hit();  // fix/uninit: fx / fy (+0xf not 0 / 1) and sx / sy (rotation < 0) are 0
        }
        c_wp.hit();
        const i32 idx = static_cast<i16>(idx_arg & 0xffffu);
        if (jo::view<i8>(jo::self<jo::MiniMap>(this)->marker_on[idx]) == 0) return;
        Vec3 c{0.0f, 0.0f, 0.0f};  // [ebp-0x10] (x / y written)
        joan::orig::as_screen(this)->offset(&c, joan::orig::arg::bits(pos.x), joan::orig::arg::bits(pos.y), joan::orig::arg::bits(pos.z));  // 0x0070fa70
        Str s;                                       // [ebp-0x1a4]
        joan::orig::as_string(&s)->construct(str(d::s_WP));                                        // 0x005a11a0 "WP_"
        joan::orig::as_string(&s)->append(static_cast<i32>(jo::view<i8>(jo::self<jo::MiniMap>(this)->marker_frame[idx])));                // 0x005a15d0
        const i32 a = jo::self<jo::MiniMap>(this)->width;
        const i32 b = jo::self<jo::MiniMap>(this)->height;
        float fx = 0.0f, fy = 0.0f;  // [ebp-0x1bc] / [ebp-0x1c0]
        const jo::MiniRes* const dims = jo::self<jo::MiniMap>(this)->view;
        if (jo::self<jo::MiniMap>(this)->visible == 0) {
            fx = scale_div(a, c.x, dims->i32_21);
            fy = scale_div(b, c.y, dims->i32_25);
        } else if (jo::self<jo::MiniMap>(this)->visible == 1) {
            fx = scale_div(a, c.x, dims->i32_29);
            fy = scale_div(b, c.y, dims->i32_2d);
        }  // other modes (fix/uninit only): never written in the original, 0 here
        fx = snap(fx);
        fy = snap(fy);
        float sx = 0.0f, sy = 0.0f;  // [ebp-0x1b8] / [ebp-0x1b4]
        rotate_to_screen(rot_of(this), fx, fy, sx, sy);
        const char* name = joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str());  // 0x00417d90
        const i16 w = jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::self<jo::MiniMap>(this)->icons)->find(joan::orig::arg::str(name))))->width);  // 0x00620b40
        name = joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str());
        const i16 h = jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::self<jo::MiniMap>(this)->icons)->find(joan::orig::arg::str(name))))->height);
        const float k = 9.0f;  // [ebp-0x1cc] = 0x41100000
        sx = static_cast<float>(fpu.sub(sx, static_cast<double>(static_cast<i32>(w) / 2)));
        sy = static_cast<float>(fpu.sub(sy, static_cast<double>(static_cast<i32>(h) / 2)));

        const float sx9 = sx + k;
        const float sy9 = sy + k;
        const bool inside = static_cast<double>(wrap_sub(screen_w(), 0xc)) >= static_cast<double>(sx9) &&
                            !(sy9 > gf(d::flt_209)) &&                                                  // 209
                            !(static_cast<double>(wrap_sub(screen_w(), 0xd6)) > static_cast<double>(sx9)) &&
                            sy9 >= gf(d::flt_7);                                                       // 7
        if (!inside) {
            i32 xi = 0, yi = 0;  // [ebp-0x1d0] / [ebp-0x1d4]
            if (!(static_cast<double>(wrap_sub(screen_w(), 0xc)) >= static_cast<double>(sx + k))) {
                xi = ftol(fpu.sub(fpu.sub(static_cast<double>(wrap_sub(screen_w(), 0xb)), k), gf(d::flt_1)));
            } else if (static_cast<double>(wrap_sub(screen_w(), 0xd6)) > static_cast<double>(sx + k)) {
                xi = ftol(fpu.sub(fpu.sub(static_cast<double>(wrap_sub(screen_w(), 0xd3)), k), gf(d::flt_3)));
            } else {
                xi = ftol(sx);
            }
            if (sy + k > gf(d::flt_209)) {
                yi = ftol((gf(d::flt_210) - k) - gf(d::flt_1));
            } else if (sy + k >= gf(d::flt_7)) {  // test ah,1 je: not below, not unordered
                yi = ftol(sy);
            } else {
                yi = ftol((gf(d::flt_10) - k) - gf(d::flt_3));
            }
            const u8* const far_spr = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::self<jo::MiniMap>(this)->icons)->find(str(d::s_WP_FAR)));  // "WP_FAR"
            void* const tex = joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(far_spr + 9), i32{1}, joan::orig::arg::u32(i32{0}));    // 0x006099c0
            const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
            joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(xi), joan::orig::arg::i16(yi), 8u, str(d::s_WP_FAR), joan::orig::arg::mptr(jo::self<jo::MiniMap>(this)->icons), joan::orig::arg::u32(color), 3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
            joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(tex));  // 0x0060a550
            if (jo::view<i8>(jo::self<jo::MiniMap>(this)->marker_kind[idx]) != 0) {
                const u32 c2 = color_from(0xffffffffu);
                const char* const txt =
                    joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_58fca0(joan::orig::arg::i32(*counter), i32{1}));  // 0x0058fca0
                joan::orig::as_text(obj(d::g_font_verdana_10_bold))->draw_line(joan::orig::arg::i32(xi), joan::orig::arg::i32(yi), joan::orig::arg::u32(i32{8}), txt, joan::orig::arg::u32(c2), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, 1.0f, 1.0f);  // 0x006255d0
                *counter = wrap_add(*counter, 1);
            }
            return;
        }
        // inside the minimap frame: clip insets a1..a4 ([ebp-0x1dc] / [ebp-0x1e4] / [ebp-0x1e0] / [ebp-0x1e8])
        const float k2 = k + k;  // fadd st, st (18)
        i32 a1 = 0, a2 = 0, a3 = 0, a4 = 0;
        if (!(static_cast<double>(wrap_sub(screen_w(), 0xc)) >= static_cast<double>(k2 + sx))) {
            a1 = 0;
            a3 = ftol(fpu.sub(fpu.sub(static_cast<double>(wrap_sub(screen_w(), 0xb)), sx), gf(d::flt_1)));
        } else if (static_cast<double>(wrap_sub(screen_w(), 0xd6)) > static_cast<double>(sx)) {
            a1 = ftol(fpu.sub(fpu.sub(static_cast<double>(wrap_sub(screen_w(), 0xd3)), sx), gf(d::flt_3)));
            a3 = ftol(k2);
            sx = static_cast<float>(wrap_sub(screen_w(), 0xd6));
        } else {
            a1 = 0;
            a3 = ftol(k2);
        }
        if (k2 + sy > gf(d::flt_209)) {
            a2 = 0;
            a4 = ftol((gf(d::flt_210) - sy) - gf(d::flt_1));
        } else if (sy >= gf(d::flt_7)) {  // test ah,1 je: not below, not unordered
            a2 = 0;
            a4 = ftol(k2);
        } else {
            a2 = ftol((gf(d::flt_10) - sy) - gf(d::flt_3));
            a4 = ftol(k2);
            sy = gf(d::flt_7);  // 7.0
        }
        name = joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str());
        const u8* const spr = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::self<jo::MiniMap>(this)->icons)->find(joan::orig::arg::str(name)));
        void* const tex = joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(spr + 9), i32{1}, joan::orig::arg::u32(i32{0}));  // 0x006099c0
        const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
        jo::BmpRes* const set = jo::self<jo::MiniMap>(this)->icons;
        name = joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str());
        const i32 yi = ftol(sy);
        const i32 xi = ftol(sx);
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(xi), joan::orig::arg::i16(yi), 9u, joan::orig::arg::str(name), joan::orig::arg::mptr(set), joan::orig::arg::u32(color), 3, 0, 0, 0, joan::orig::arg::i16(a1), joan::orig::arg::i16(a2), joan::orig::arg::i16(a3), joan::orig::arg::i16(a4), 0, -1, -1);  // 0x00633500
        joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(tex));   // 0x0060a550
        if (jo::view<i8>(jo::self<jo::MiniMap>(this)->marker_kind[idx]) != 0) {
            const u32 c2 = color_from(0xffffffffu);
            const char* const txt = joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_58fca0(joan::orig::arg::i32(*counter), i32{1}));  // 0x0058fca0
            const i32 ly = ftol(sy);
            const i32 lx = ftol(sx);
            joan::orig::as_text(obj(d::g_font_verdana_10_bold))->draw_line(joan::orig::arg::i32(lx), joan::orig::arg::i32(ly), joan::orig::arg::u32(i32{8}), txt, joan::orig::arg::u32(c2), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, 1.0f, 1.0f);  // 0x006255d0
            *counter = wrap_add(*counter, 1);
        }
    }

    // ---- 0x0071aba0 --------------------------------------------------------------------------
    void draw_gold(u8* unit) {
        const i32 mode0 = jo::self<jo::MiniMap>(this)->visible;
        if (rot_of(this) < 0 || (mode0 != 0 && mode0 != 1)) {  // uninitialised locals in the original
            if (!fix_uninit()) {  // exact: run the original body
                c_gold_orig.hit();
                thiscall<void>(fn::MiniMap_draw_gold_71aba0_body, this, unit);
                return;
            }
            c_mm_fix.hit();  // fix/uninit: fx / fy (+0xf not 0 / 1) and sx / sy (rotation < 0) are 0
        }
        c_gold.hit();
        if (jo::self<jo::MiniMap>(this)->active == 0 || unit == nullptr) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_a45f08))->empty_at_561320(static_cast<i32>(jo::self<jo::MiniMap>(this)->x))) != 0) return;  // 0x00561320
        u8* const kind = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(unit)->type_id))));  // 0x00425ce0
        if (joan::crt::stricmp(joan::orig::arg::str(jo::view<const char*>(jo::as_item_def(kind)->name)), str(d::s_S7_GOLDBAR)) != 0 &&      // "S7_GOLDBAR"
            joan::crt::stricmp(joan::orig::arg::str(jo::as_map_object(unit)->name), str(d::s_s7_goldbag)) != 0) {  // "s7_goldbag"
            return;
        }
        float pos[2] = {0.0f, 0.0f};                         // [ebp-0x14] / [ebp-0x10]
        joan::orig::as_mini_map(this)->pos_to_map(joan::orig::arg::mptr(unit), joan::orig::arg::mptr(pos));        // 0x0070fae0
        const jo::MiniRes* const dims = jo::self<jo::MiniMap>(this)->view;
        const i32 m = jo::self<jo::MiniMap>(this)->visible;
        const bool known = m == 0 || m == 1;  // other modes (fix/uninit only) skip both switches
        const i32 jo::MiniRes::* const wo = m == 0 ? &jo::MiniRes::i32_21 : &jo::MiniRes::i32_29;  // the kind's cell size
        const i32 jo::MiniRes::* const ho = m == 0 ? &jo::MiniRes::i32_25 : &jo::MiniRes::i32_2d;
        if (known) {
            if (!(static_cast<double>(dims->*wo) >= static_cast<double>(pos[0]))) return;
            if (!(static_cast<double>(dims->*ho) >= static_cast<double>(pos[1]))) return;
            if (!(pos[0] >= gf(d::k_9cf5c4))) return;
            if (!(pos[1] >= gf(d::k_9cf5c4))) return;
        }
        const i32 a = jo::self<jo::MiniMap>(this)->width;
        const i32 b = jo::self<jo::MiniMap>(this)->height;
        // [ebp-0x2c] / [ebp-0x30]; never written for other modes (fix/uninit: 0)
        const float fx = known ? scale_div(a, pos[0], dims->*wo) : 0.0f;
        const float fy = known ? scale_div(b, pos[1], dims->*ho) : 0.0f;
        const i32 row = ftol(fy);
        const i32 cell = wrap_add(wrap_mul(row, 200), ftol(fx));
        if (jo::view<i8>(jo::self<jo::MiniMap>(this)->fog[cell]) == 0) return;
        float sx = 0.0f, sy = 0.0f;  // [ebp-0x28] / [ebp-0x24]
        rotate_to_screen(rot_of(this), fx, fy, sx, sy);
        sx = snap(sx);
        sy = snap(sy);
        const u32 color = global<u32>(d::g_a6a5a4);
        const float one = gf(d::flt_1);
        const i32 y2 = ftol(sy + one);
        const i32 x2 = ftol(sx + one);
        const i32 y1 = ftol(sy - one);
        const i32 x1 = ftol(sx - one);
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->queue(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(i32{0xa}), joan::orig::arg::u32(color), joan::orig::arg::mptr(i32{0}), i32{3}, i32{0}, i32{0}, i32{0}, i32{-1}, i32{-1}, i32{-1}, i32{-1}, i32{0});  // 0x00632a70
    }
};

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b111_19", name, addr, fn)
B("MiniMap::draw_view_716be0", 0x00716be0, &H::draw_view);
B("MiniMap::draw_waypoint_718540", 0x00718540, &H::draw_waypoint);
B("MiniMap::draw_gold_71aba0", 0x0071aba0, &H::draw_gold);
#undef B

}  // namespace game_b111_19
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

struct H {
    void* self() { return this; }

    // 0x00717220
    std::uint16_t get_f_717220() { return jo::view<std::uint16_t>(jo::self<jo::MiniMap>(this)->visible); }
    // 0x007196e0
    void* set_slot(std::int32_t i, std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d) {
        jo::self<jo::MiniMap>(this)->slot_used[i - 1] = 1;
        jo::self<jo::MiniMap>(this)->slot_kind[i - 1] = 0;
        jo::self<jo::MiniMap>(this)->slot_a[i - 1] = a;
        jo::self<jo::MiniMap>(this)->slot_b[i - 1] = b;
        jo::self<jo::MiniMap>(this)->slot_c[i - 1] = c;
        jo::self<jo::MiniMap>(this)->slot_d[i - 1] = d;
        return self();
    }
    // 0x007198d0
    void clear_slot(std::int32_t i) { jo::self<jo::MiniMap>(this)->slot_used[i - 1] = 0; }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj717::get_f", 0x00717220, &H::get_f_717220);
B91("Obj719::set_slot", 0x007196e0, &H::set_slot);
B91("Obj719::clear_slot", 0x007198d0, &H::clear_slot);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b111_12.cpp (fix/uninit)
namespace {
namespace game_b111_12 {

using joan::hook::global;

using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::call::thiscall;

JOAN_COUNTER(c_map_click, "MapButton::click_717240");

// ------------------------------------------------------------------------ 0x00717240
struct Cam {
    u32 d[10];  // 0x28 bytes, passed by value to 0x00453930
};
static_assert(sizeof(Cam) == 0x28, "camera record size");

void* cam_owner() {  // 0x00563dc0 then 0x00563c80 on 0xa48cc0
    const u32 a = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(obj(d::g_camera_man))->target());
    return joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(joan::orig::arg::i32(a)));
}

struct H {
    // -------------------------------------------------------------------- 0x00717240
    u8 map_click() {
        c_map_click.hit();
        if (jo::self<jo::MiniMap>(this)->active == 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_a45f08))->empty_at_561320(static_cast<i32>(jo::self<jo::MiniMap>(this)->x))) != 0) return 0;  // 0x00561320
        if (joan::orig::arg::as<i16>(joan::orig::as_mini_map(this)->zoom_mode()) != 0) return 0;  // 0x00717220
        if (global<i32>(d::g_control_mode) != 4) return 0;
        if (joan::orig::arg::as<i16>(joan::orig::as_track(obj(global<std::uintptr_t>(d::g_a5ba00)))->is_type1()) != 0) return 0;  // 0x00428aa0
        const i32 w = jo::self<jo::MiniMap>(this)->width;
        const i32 h = jo::self<jo::MiniMap>(this)->height;
        const u32 base = global<u32>(d::g_screen_w);
        if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(base - 0xd3u), i32{0xa}, joan::orig::arg::i32(base + static_cast<u32>(w) - 0xd3u), joan::orig::arg::i32(h + 0xa))) == 0)  // 0x00593980
            return 0;
        u8* const cc = global<u8*>(d::g_camera);
        if (joan::orig::arg::as<i32>(joan::orig::as_parser(obj(d::g_mouse))->rect_value(joan::orig::arg::i32(jo::as_camera(cc)->view_x1), joan::orig::arg::i32(jo::as_camera(cc)->view_y1), joan::orig::arg::i32(jo::as_camera(cc)->view_x2), joan::orig::arg::i32(jo::as_camera(cc)->view_y2), i32{0})) == 0)  // 0x005945d0
            return 0;
        i32 rot = static_cast<i32>(jo::self<jo::MiniMap>(this)->rotation) % 4;
        // fix/uninit: a negative rotation gives a negative remainder, no case matches and the
        // original uses uninitialised dx / dy. Use the equivalent rotation 0..3 instead.
        if (rot < 0) rot += 4;
        const u32 mx = global<u32>(d::g_mouse_x);
        const u32 my = global<u32>(d::g_mouse_y);
        u32 dx = 0;
        u32 dy = 0;
        switch (rot) {
            case 0:
                dx = mx - (global<u32>(d::g_screen_w) - 0xd3u);
                dy = my - 0xau;
                break;
            case 1:
                dx = 0xd2u - my;
                dy = mx - (global<u32>(d::g_screen_w) - 0xd3u);
                break;
            case 2:
                dx = (global<u32>(d::g_screen_w) - 0xbu) - mx;
                dy = 0xd2u - my;
                break;
            default:  // 3
                dx = my - 0xau;
                dy = (global<u32>(d::g_screen_w) - 0xbu) - mx;
                break;
        }
        const i32 qx = static_cast<i32>(dx * jo::view<u32>(jo::as_mini_res(jo::self<jo::MiniMap>(this)->view)->i32_21)) / w;
        float fx = static_cast<float>(qx);
        const i32 qy = static_cast<i32>(dy * jo::view<u32>(jo::as_mini_res(jo::self<jo::MiniMap>(this)->view)->i32_25)) / h;
        float fy = static_cast<float>(qy);
        fx = fx + jo::self<jo::MiniMap>(this)->scale_x;
        fy = jo::self<jo::MiniMap>(this)->scale_y - fy;
        // Degenerate box {min, max} = {(fx, fy, 0), (fx, fy, 0)}: 0x00429090 -> 0x0054ad90 ->
        // 0x0060d6b0 (box overlap test) reads all six floats ([ebp-0x38] and its copy [ebp-0x2c]).
        float pos[6] = {fx, fy, 0.0f, fx, fy, 0.0f};
        i32 found = 0;
        joan::orig::as_world(obj(global<std::uintptr_t>(d::g_world)))->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(pos)), &found);  // 0x00429090
        if (found == 0) return 0;
        Cam cam{};
        joan::orig::as_camera_motion(cam_owner())->end(&cam);  // 0x00423fe0 copy camera (0x28 bytes)
        std::memcpy(&cam.d[1], &fx, 4);
        std::memcpy(&cam.d[2], &fy, 4);
        for (;;) {
            float dist = 0.0f;
            std::memcpy(&dist, &cam.d[7], 4);
            if (!(dist > global<float>(d::g_a0bbf0))) break;
            if (joan::orig::arg::as<i8>(joan::orig::as_player(obj(d::g_player_unit_recno))->corners_on_map(cam)) != 0) {  // 0x00453930
                void* const o = cam_owner();
                joan::orig::as_camera_motion(o)->start(&cam, &cam);  // 0x00423ec0
                break;
            }
            dist = dist - global<float>(d::flt_200);  // 200.0
            std::memcpy(&cam.d[7], &dist, 4);
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_camera_motion(cam_owner())->update()) != 0) {  // 0x004237a0
            u8* const o = static_cast<u8*>(cam_owner());
            joan::orig::as_camera(obj(global<std::uintptr_t>(d::g_camera)))->set_frame(joan::orig::arg::mptr(&jo::as_camera_motion(o)->current_));  // 0x00614040
        }
        return 1;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: negative map rotation -> rotation + 4 instead of uninitialised cursor offsets.
FIX("MapButton::click_717240", 0x00717240, &H::map_click);
#undef FIX

}  // namespace game_b111_12
}  // namespace

// ---------------------------------------- from modules/game_b111_13.cpp (game/b111_13)
namespace {
namespace game_b111_13 {

using joan::hook::global;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::orig::helpers::wrap_sub;
using joan::orig::helpers::wrap_mul;

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

using joan::call::thiscall;

constexpr img::Addr kScreenW = d::g_screen_w;

JOAN_COUNTER(c_to_screen, "MapMarker::to_screen");

struct H {
    // ------------------------------------------------------------------------ 0x00717700
    u8 to_screen(float* out) {
        const i32 rot = static_cast<i32>(jo::self<jo::MiniMap>(this)->rotation) % 4;
        // Negative remainder: both offsets uninitialised in the original - original body, or with
        // fix/uninit (batch 118/03) tx = ty = 0 (the switch below writes neither).
        if (rot < 0 && !fix_uninit()) return thiscall<u8>(fn::MapMarker_to_screen_body, this, out);
        c_to_screen.hit();
        if (joan::orig::arg::as<i8>(joan::orig::as_marker(this)->visible()) == 0) return 0;  // 0x00717650
        const i32 sx = jo::self<jo::MiniMap>(this)->width;
        const i32 sy = jo::self<jo::MiniMap>(this)->height;
        i32 tx = 0, ty = 0;
        switch (rot) {
            case 0:
                tx = wrap_sub(global<i32>(d::g_mouse_x), wrap_sub(global<i32>(kScreenW), 0xd3));
                ty = wrap_sub(global<i32>(d::g_mouse_y), 0xa);
                break;
            case 1:
                tx = wrap_sub(0xd2, global<i32>(d::g_mouse_y));
                ty = wrap_sub(global<i32>(d::g_mouse_x), wrap_sub(global<i32>(kScreenW), 0xd3));
                break;
            case 2:
                tx = wrap_sub(wrap_sub(global<i32>(kScreenW), 0xb), global<i32>(d::g_mouse_x));
                ty = wrap_sub(0xd2, global<i32>(d::g_mouse_y));
                break;
            case 3:
                tx = wrap_sub(global<i32>(d::g_mouse_y), 0xa);
                ty = wrap_sub(wrap_sub(global<i32>(kScreenW), 0xb), global<i32>(d::g_mouse_x));
                break;
            default:  // negative remainder (fix/uninit only): never written in the original, 0 here
                break;
        }
        const i32 qx = wrap_mul(tx, jo::self<jo::MiniMap>(this)->view->i32_21) / sx;
        out[0] = static_cast<float>(qx);
        const i32 qy = wrap_mul(ty, jo::self<jo::MiniMap>(this)->view->i32_25) / sy;
        out[1] = static_cast<float>(qy);
        jo::view<u32>(out[2]) = 0;
        out[0] = out[0] + jo::self<jo::MiniMap>(this)->scale_x;
        out[1] = jo::self<jo::MiniMap>(this)->scale_y - out[1];
        const i32 py = joan::hook::x87::ftol(static_cast<double>(out[1]));
        const i32 px = joan::hook::x87::ftol(static_cast<double>(out[0]));
        const u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_zones(jo::view<void*>(jo::self<jo::MiniMap>(this)->view))->find_71b520(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::MiniMap>(this)->visible)), joan::orig::arg::i32(px), joan::orig::arg::i32(py), 0));  // 0x0071b520
        return static_cast<i8>(jo::as_mini_res_frame(r)->is_default) != 0 ? u8{0} : u8{1};
    }
};

#define B111(name, addr, fn) JOAN_REPLACE("game/b111_13", name, addr, fn)
B111("MapMarker::to_screen", 0x00717700, &H::to_screen);
#undef B111

}  // namespace game_b111_13
}  // namespace

// ---------------------------------------- from modules/game_b92_06_2.cpp (game/b92_06)
namespace {
namespace game_b92_06_2 {

using joan::hook::global;

using joan::img::obj;

using joan::call::thiscall;

struct V3f {
    float x, y, z;
};
static_assert(sizeof(V3f) == 12, "v3");

JOAN_COUNTER(c_drop, "Cursor::can_drop_at");

struct H {
    // 0x00717910
    std::uint8_t can_drop_at(V3f pos) {
        c_drop.hit();
        if (jo::self<jo::MiniMap>(this)->active == 0) return 1;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(obj(d::g_a45f08))->empty_at_561320(static_cast<std::int32_t>(jo::self<jo::MiniMap>(this)->x))) != 0)
            return 1;  // 0x00561320
        if (joan::orig::arg::as<std::int16_t>(joan::orig::as_mini_map(this)->zoom_mode()) != 0) return 1;  // 0x00717220
        if (global<std::int32_t>(d::g_control_mode) != 4) return 0;
        if (joan::orig::arg::as<std::int16_t>(joan::orig::as_track(global<void*>(d::g_a5ba00))->is_type1()) != 0) return 0;  // 0x00428aa0
        // 0x00429090 reads a box (min, max): the degenerate box {pos, pos} ([ebp-0x1c] and its copy [ebp-0x10]).
        V3f p_box[2] = {pos, pos};
        std::int32_t hit = 0;
        joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(p_box, &hit);  // 0x00429090
        if (hit == 0) return 0;
        V3f scr{0.0f, 0.0f, 0.0f};
        joan::orig::as_screen(this)->offset(&scr, joan::orig::arg::bits(pos.x), joan::orig::arg::bits(pos.y), joan::orig::arg::bits(pos.z));  // 0x0070fa70
        void* const view = jo::view<void*>(jo::self<jo::MiniMap>(this)->view);
        if (!(static_cast<double>(jo::as_mini_res(view)->i32_21) >= static_cast<double>(scr.x))) return 0;
        if (!(static_cast<double>(jo::as_mini_res(view)->i32_25) >= static_cast<double>(scr.y))) return 0;
        if (!(scr.x >= global<float>(d::k_9cf5c4))) return 0;
        if (!(scr.y >= global<float>(d::k_9cf5c4))) return 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Cursor::can_drop_at", 0x00717910, &H::can_drop_at);
#undef B

}  // namespace game_b92_06_2
}  // namespace

// ---------------------------------------- from modules/game_b104_14.cpp (game/b104_14)
namespace {
namespace game_b104_14 {

using joan::hook::global;

using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::hook::x87::Fpu;

const Fpu fpu{true};

using joan::hook::x87::ftol;

using joan::call::thiscall;

constexpr img::Addr kUnits = d::g_units;

i32 ftol_f(double v) { return ftol(v); }

JOAN_COUNTER(c_clip, "MapOverlay::clip_segment_717ba0");
JOAN_COUNTER(c_update_unit, "MapOverlay::update_unit_7190b0");
JOAN_COUNTER(c_reveal, "MapOverlay::reveal_quad_719e60");

struct H {
    // ------------------------------------------------------------------ map overlay helpers
    // scale of a map-space point to the overlay: +0xf selects the map size pair of [+4]
    // (returns false for any other +0xf - unreachable: the original leaves its locals untouched)
    bool overlay_scale(float x, float y, float& sx, float& sy) {
        const i32 mode = jo::self<jo::MiniMap>(this)->visible;
        const i32 jo::MiniRes::* ox;  // the kind's cell size
        const i32 jo::MiniRes::* oy;
        if (mode == 0) {
            ox = &jo::MiniRes::i32_21;
            oy = &jo::MiniRes::i32_25;
        } else if (mode == 1) {
            ox = &jo::MiniRes::i32_29;
            oy = &jo::MiniRes::i32_2d;
        } else {
            return false;
        }
        const jo::MiniRes* const m = jo::self<jo::MiniMap>(this)->view;
        sx = static_cast<float>(fpu.div(fpu.mul(static_cast<double>(jo::self<jo::MiniMap>(this)->width), x), m->*ox));
        sy = static_cast<float>(fpu.div(fpu.mul(static_cast<double>(jo::self<jo::MiniMap>(this)->height), y), m->*oy));
        return true;
    }

    // ------------------------------------------------------------------ 0x00717ba0
    void clip_segment_717ba0(float* p0, float* p1) {
        c_clip.hit();
        const float s = static_cast<float>(fpu.div(fpu.sub(p1[1], p0[1]), fpu.sub(p1[0], p0[0])));
        const i32 wm1 = static_cast<i32>(jo::self<jo::MiniMap>(this)->width) - 1;
        const i32 hm1 = static_cast<i32>(jo::self<jo::MiniMap>(this)->height) - 1;
        jo::view<u32>(p0[2]) = 0;
        jo::view<u32>(p1[2]) = 0;
        // x against the left edge W-0xd3 / right edge W+wm1-0xd3
        auto clip_x = [&](float* dst, i32 edge) {
            dst[1] = static_cast<float>(fpu.add(fpu.mul(fpu.sub(static_cast<double>(edge), p0[0]), s), p0[1]));
            dst[0] = static_cast<float>(edge);
        };
        auto clip_y = [&](float* dst, double edge, float stored) {
            dst[0] = static_cast<float>(fpu.add(fpu.div(fpu.sub(edge, p0[1]), s), p0[0]));
            dst[1] = stored;
        };
        {
            const i32 e = global<i32>(d::g_screen_w) - 0xd3;
            if (static_cast<double>(e) > p0[0]) clip_x(p0, global<i32>(d::g_screen_w) - 0xd3);
        }
        {
            const i32 e = global<i32>(d::g_screen_w) + wm1 - 0xd3;
            if (!(static_cast<double>(e) >= p0[0])) clip_x(p0, global<i32>(d::g_screen_w) + wm1 - 0xd3);
        }
        {
            const i32 e = global<i32>(d::g_screen_w) - 0xd3;
            if (static_cast<double>(e) > p1[0]) clip_x(p1, global<i32>(d::g_screen_w) - 0xd3);
        }
        {
            const i32 e = global<i32>(d::g_screen_w) + wm1 - 0xd3;
            if (!(static_cast<double>(e) >= p1[0])) clip_x(p1, global<i32>(d::g_screen_w) + wm1 - 0xd3);
        }
        const float ten = global<float>(d::flt_10);
        const i32 bottom = hm1 + 0xa;
        if (!(p0[1] >= ten)) clip_y(p0, ten, ten);
        if (!(static_cast<double>(bottom) >= p0[1])) clip_y(p0, bottom, static_cast<float>(bottom));
        if (!(p1[1] >= ten)) clip_y(p1, ten, ten);
        if (!(static_cast<double>(bottom) >= p1[1])) clip_y(p1, bottom, static_cast<float>(bottom));
    }

    // ------------------------------------------------------------------ 0x007190b0
    void set_marker(void* marker, u8 flag) {
        joan::orig::as_model_instance(marker)->set_10a(joan::orig::arg::u32(static_cast<u32>(flag)));  // 0x0042cf20
    }
    void banner_markers(i32 id, u8 flag) {
        if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(id)) != 0) return;  // 0x00425d50
        u8* const ban = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
        if (jo::as_map_object(ban)->actor != nullptr) set_marker(jo::as_map_object(ban)->actor, flag);
        for (i32 base = 0; base <= 2; base += 2) {  // MapObject::sound_slots[0..3] (+0x96..+0x9d)
            for (i32 k = 0; k < 2; ++k) {
                const i32 a = jo::as_map_object(ban)->sound_slots[base + k];
                if (joan::orig::as_dynarray(obj(d::g_particle_effect_array))->empty_at(joan::orig::arg::i32(a)) != 0) continue;  // 0x0041bd90
                u8* const att = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_particle_effect_array))->ptr_at(static_cast<i32>(jo::as_map_object(ban)->sound_slots[base + k])));  // 0x0041bd60
                // the 0x00a57078 entries are ModelObj: +0x53 = fx.object (the model the marker is set on)
                if (jo::as_model_obj(att)->fx.object_ != nullptr) set_marker(jo::as_model_obj(att)->fx.object_, flag);
            }
        }
    }

    void update_unit_7190b0(u8* unit) {
        c_update_unit.hit();
        void* const u = joan::orig::arg::as<void*>(joan::orig::as_unit(unit)->nation());  // 0x00509400
        if (u != nullptr) joan::orig::as_player(u)->relation(static_cast<i32>(jo::self<jo::MiniMap>(this)->nation->id));  // 0x004b42e0
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::MiniMap>(this)->unit_id)))) == 0) {  // 0x005094d0
            (void)joan::orig::as_dynarray(obj(kUnits))->ptr_at(static_cast<i32>(jo::self<jo::MiniMap>(this)->unit_id));  // 0x0041b3c0
            return;
        }
        u8 flag = 1;
        if (global<i32>(d::g_control_mode) == 4) {
            float pt[3] = {0.0f, 0.0f, 0.0f};
            joan::orig::as_mini_map(this)->unit_to_map(joan::orig::arg::mptr(unit), joan::orig::arg::mptr(pt));  // 0x0070f9e0
            const i32 mode = jo::self<jo::MiniMap>(this)->visible;
            if (mode == 0 || mode == 1) {
                const jo::MiniRes* const m = jo::self<jo::MiniMap>(this)->view;
                const i32 w = mode == 0 ? m->i32_21 : m->i32_29;  // the kind's cell size
                const i32 h = mode == 0 ? m->i32_25 : m->i32_2d;
                const float zero = global<float>(d::k_9cf5c4);
                if (!(static_cast<double>(w) >= pt[0]) || !(static_cast<double>(h) >= pt[1]) || !(pt[0] >= zero) ||
                    !(pt[1] >= zero))
                    flag = 0;
            }
            if (flag != 0) {
                float sx = 0.0f, sy = 0.0f;
                overlay_scale(pt[0], pt[1], sx, sy);
                const u32 row = static_cast<u32>(ftol_f(sy)) * 0xc8u;
                const u32 idx = row + static_cast<u32>(ftol_f(sx));
                flag = static_cast<i8>(jo::self<jo::MiniMap>(this)->fog[static_cast<i32>(idx)]) != 0 ? 1 : 0;
            }
        }
        if (global<u8>(d::g_a6a5bc) != 0) flag = 1;
        if (jo::as_unit(unit)->actor != nullptr) set_marker(jo::as_unit(unit)->actor, flag);
        banner_markers(jo::as_unit(unit)->carried_a, flag);
        banner_markers(jo::as_unit(unit)->carried_b, flag);
        if (jo::as_unit(unit)->trail_a != nullptr && jo::as_unit(unit)->trail_a->actor != nullptr)
            set_marker(jo::as_unit(unit)->trail_a->actor, flag);
        if (jo::as_unit(unit)->trail_b != nullptr && jo::as_unit(unit)->trail_b->actor != nullptr)
            set_marker(jo::as_unit(unit)->trail_b->actor, flag);
    }

    // ------------------------------------------------------------------ 0x00719e60
    void reveal_quad_719e60(i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3) {
        c_reveal.hit();
        i32 dw = 0, dh = 0;  // uninitialised in the original for +0xf not 0 / 1 (cannot happen)
        const i32 mode = jo::self<jo::MiniMap>(this)->visible;
        if (mode == 0) {
            dw = jo::self<jo::MiniMap>(this)->view->i32_21;
            dh = jo::self<jo::MiniMap>(this)->view->i32_25;
        } else if (mode == 1) {
            dw = jo::self<jo::MiniMap>(this)->view->i32_29;
            dh = jo::self<jo::MiniMap>(this)->view->i32_2d;
        }
        const float kx = static_cast<float>(fpu.div(static_cast<double>(jo::self<jo::MiniMap>(this)->width), static_cast<double>(dw)));
        const float ky = static_cast<float>(fpu.div(static_cast<double>(jo::self<jo::MiniMap>(this)->height), static_cast<double>(dh)));
        // ([ebp-0x20] = kx / ky is never read)
        const float ox = jo::self<jo::MiniMap>(this)->scale_x;
        const float oy = jo::self<jo::MiniMap>(this)->scale_y;
        const i32 ix[4] = {x0, x1, x2, x3};
        const i32 iy[4] = {y0, y1, y2, y3};
        float X[4], Y[4];
        for (i32 k = 0; k < 4; ++k) {
            X[k] = static_cast<float>(fpu.mul(fpu.sub(static_cast<double>(ix[k]), ox), kx));
            Y[k] = static_cast<float>(fpu.mul(fpu.sub(oy, static_cast<double>(iy[k])), ky));
        }
        float mn = Y[0], mx = Y[0];
        if (!(Y[1] >= mn))
            mn = Y[1];
        else
            mx = Y[1];
        for (i32 k = 2; k < 4; ++k) {
            if (!(Y[k] >= mn))
                mn = Y[k];
            else if (Y[k] > mx)
                mx = Y[k];
        }
        const float half = global<float>(d::flt_0_5);
        i32 lo = ftol_f(fpu.add(mn, half));
        i32 hi = ftol_f(mx);
        if (hi < 0) return;
        if (lo >= 0xc8) return;
        const float zero = global<float>(d::k_9cf5c4);
        const float lim = global<float>(d::flt_200);
        if (!(X[0] >= zero) && !(X[1] >= zero) && !(X[2] >= zero) && !(X[3] >= zero)) return;
        if (X[0] >= lim && X[1] >= lim && X[2] >= lim && X[3] >= lim) return;
        if (lo < 0) lo = 0;
        if (hi > 0xc7) hi = 0xc7;
        const float one = global<float>(d::flt_1);
        for (i32 row = lo; row <= hi; ++row) {
            i32 left = 0, right = 0xc7, ok = 0;
            for (i32 e = 0; e < 4; ++e) {
                const i32 a = e;
                const i32 b = (e + 1) % 4;
                if (!(Y[a] < Y[b]) && !(Y[a] > Y[b])) continue;  // equal or unordered
                const float t = static_cast<float>(
                    fpu.div(fpu.sub(static_cast<double>(row), Y[a]), fpu.sub(Y[b], Y[a])));
                if (!(t >= zero)) continue;
                if (t > one) continue;
                ok = 1;
                if (!(Y[a] >= Y[b])) {
                    const i32 v = ftol_f(fpu.add(fpu.mul(fpu.sub(X[b], X[a]), t), X[a]));
                    if (v >= 0xc8) {
                        ok = 0;
                        break;
                    }
                    if (v > left) left = v;
                } else {
                    const i32 v = ftol_f(fpu.add(fpu.add(fpu.mul(fpu.sub(X[b], X[a]), t), X[a]), half));
                    if (v < 0) {
                        ok = 0;
                        break;
                    }
                    if (v < right) right = v;
                }
            }
            if (ok != 0) {
                u8* const p = jo::self<jo::MiniMap>(this)->fog + static_cast<u32>(row) * 0xc8u + static_cast<u32>(left);
                std::memset(joan::orig::arg::mptr(static_cast<void*>(p)), 1, joan::orig::arg::u32(static_cast<u32>(right - left + 1)));  // memset
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_14", name, addr, fn)
B("MapOverlay::clip_segment_717ba0", 0x00717ba0, &H::clip_segment_717ba0);
B("MapOverlay::update_unit_7190b0", 0x007190b0, &H::update_unit_7190b0);
B("MapOverlay::reveal_quad_719e60", 0x00719e60, &H::reveal_quad_719e60);
#undef B

}  // namespace game_b104_14
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using joan::hook::global;

using joan::img::obj;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::call::thiscall;

void* const kScreen = obj(d::g_sprites_a46128);

i32 ftol(double v) { return joan::hook::x87::ftol(v); }

JOAN_COUNTER(c_draw_clipped, "Obj::draw_clipped");
JOAN_COUNTER(c_set_slot, "Obj::set_slot_7197f0");

struct H {
    // ------------------------------------------------------------------ 0x00717f40
    void draw_clipped(float x1, float y1, u32 unused2, float x2, float y2, u32 unused5, u32 a6, u32 a7) {
        c_draw_clipped.hit();
        (void)unused2;
        (void)unused5;
        const float lo = static_cast<float>(global<i32>(d::g_screen_w) - 0xd3);  // fild: exact
        const float hi = static_cast<float>(global<i32>(d::g_screen_w) - 0xc);
        const u8 visible = joan::orig::arg::as<u8>(joan::orig::clip_line(&x1, &y1, &x2, &y2, joan::orig::arg::f32(lo), joan::orig::arg::f32(hi), 10.0f, 209.0f));  // 0x0068cb50
        if (visible == 0) return;
        joan::orig::as_sprite2d(kScreen)->queue_line(joan::orig::arg::i16(ftol(x1)), joan::orig::arg::i16(ftol(y1)), joan::orig::arg::i16(ftol(x2)), joan::orig::arg::i16(ftol(y2)), joan::orig::arg::i32(a6), joan::orig::arg::u32(a7), joan::orig::arg::u32(a7));  // 0x00632100
    }

    // ------------------------------------------------------------------ 0x007197f0
    H* set_slot_7197f0(i32 i, u32 a, u32 b, u32 c, u32 d, u32 e, u32 g, u32 h, u32 j) {
        c_set_slot.hit();
        jo::self<jo::MiniMap>(this)->slot_used[i - 1] = 1;
        jo::self<jo::MiniMap>(this)->slot_kind[i - 1] = 2;
        jo::self<jo::MiniMap>(this)->slot_a[i - 1] = a;
        jo::self<jo::MiniMap>(this)->slot_b[i - 1] = b;
        jo::self<jo::MiniMap>(this)->slot_c[i - 1] = c;
        jo::self<jo::MiniMap>(this)->slot_d[i - 1] = d;
        jo::self<jo::MiniMap>(this)->slot_e[i - 1] = e;
        jo::self<jo::MiniMap>(this)->slot_f[i - 1] = g;
        jo::self<jo::MiniMap>(this)->slot_g[i - 1] = h;
        jo::self<jo::MiniMap>(this)->slot_h[i - 1] = j;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::draw_clipped", 0x00717f40, &H::draw_clipped);
B("Obj::set_slot_7197f0", 0x007197f0, &H::set_slot_7197f0);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x007180f0
    void set_slot_7180f0(i16 k, u32 v, u32 b) {
        const i32 i = static_cast<i16>(k - 1);
        jo::self<jo::MiniMap>(this)->marker_unit[i] = v;
        jo::self<jo::MiniMap>(this)->marker_kind[i] = static_cast<u8>(b);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("Obj::set_slot_7180f0", 0x007180f0, &H::set_slot_7180f0);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::x87::Fpu;

const Fpu fpu{true};

using joan::hook::x87::ftol;

using joan::call::thiscall;

JOAN_COUNTER(c_718140, "Map::attack_marker_718140");

// 0x0042a260 (thunk 0x004098b8): 4-byte colour ctor (r, g, b, a), returns this.
u32 make_color(u32* slot) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff}));
}

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x00718140
    void attack_marker_718140(float x, float y, u32 /*z*/) {
        c_718140.hit();
        alignas(4) char name[0x194];  // frame slot [ebp-0x198, ebp-0x4); ctor writes 0x191 bytes
        joan::orig::as_string(name)->construct(str(d::s_ATTACK_9dede0));                         // 0x005a11a0 "ATTACK_"
        joan::orig::as_string(name)->append(static_cast<i32>(jo::self<jo::MiniMap>(this)->frame));         // 0x005a15d0
        const i16 w1 = jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::self<jo::MiniMap>(this)->icons)->find(joan::orig::arg::str(joan::orig::as_string(name)->c_str()))))->width);  // 0x00620b40(0x00417d90())
        const i16 w2 = jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::self<jo::MiniMap>(this)->icons)->find(joan::orig::arg::str(joan::orig::as_string(name)->c_str()))))->height);
        const double k = 9.0;
        const i32 sw = global<i32>(d::g_screen_w);
        if (!(static_cast<double>(static_cast<i32>(static_cast<u32>(sw) - 0xbu)) >= x)) return;
        if (y > 210.0f) return;
        x = x - 9.0f;
        y = y - 9.0f;
        if (static_cast<double>(static_cast<i32>(static_cast<u32>(sw) - 0xd3u)) > x) return;
        if (!(y >= 10.0f)) return;
        const i32 h1 = w1 / 2, h2 = w2 / 2;
        const i32 a = ftol(fpu.sub(fpu.sub(static_cast<double>(h1), k), 1.0));
        const i32 b = ftol(fpu.sub(fpu.sub(static_cast<double>(h2), k), 1.0));
        float cf;
        const double t = fpu.sub(static_cast<double>(static_cast<i32>(static_cast<u32>(global<i32>(d::g_screen_w)) - 0xbu)), x);
        if (!(18.0 >= t)) {
            cf = static_cast<float>(fpu.sub(fpu.add(static_cast<double>(h1), k), 1.0));
        } else {
            const i32 v = static_cast<i32>(static_cast<u32>(a) + static_cast<u32>(global<i32>(d::g_screen_w)) - 0xbu);
            cf = static_cast<float>(fpu.sub(static_cast<double>(v), x));
        }
        const i32 c = ftol(cf);
        float df;
        const double t2 = fpu.sub(210.0, y);
        if (!(18.0 >= t2)) {
            df = static_cast<float>(fpu.sub(fpu.add(static_cast<double>(h2), k), 1.0));
        } else {
            df = static_cast<float>(fpu.sub(static_cast<double>(static_cast<i32>(static_cast<u32>(b) + 0xd2u)), y));
        }
        const i32 d = ftol(df);
        u8* const def = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::self<jo::MiniMap>(this)->icons)->find(joan::orig::arg::str(joan::orig::as_string(name)->c_str())));  // 0x00620b40
        void* const h = joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(def + 9), i32{1}, joan::orig::arg::u32(i32{0}));           // 0x006099c0
        u32 slot = 0;
        const u32 color = make_color(&slot);
        jo::BmpRes* const owner = jo::self<jo::MiniMap>(this)->icons;
        const char* const cstr = joan::orig::arg::str(joan::orig::as_string(name)->c_str());  // 0x00417d90
        // 0x00633500 reads the four corner args as words only.
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(ftol(x)), joan::orig::arg::i16(ftol(y)), 9u, joan::orig::arg::str(cstr), joan::orig::arg::mptr(owner), joan::orig::arg::u32(color), 3, 0, 0, 0, joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::i16(c), joan::orig::arg::i16(d), 0, -1, -1);  // 0x00633500
        joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(h));                         // 0x0060a550
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Map::attack_marker_718140", 0x00718140, &H::attack_marker_718140);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct H {
    void set_slot(i32 i, u32 a, u32 b, u32 c) {  // 0x00719770
        jo::self<jo::MiniMap>(this)->slot_used[i - 1] = 1;
        jo::self<jo::MiniMap>(this)->slot_kind[i - 1] = 1;
        jo::self<jo::MiniMap>(this)->slot_a[i - 1] = a;
        jo::self<jo::MiniMap>(this)->slot_b[i - 1] = b;
        jo::self<jo::MiniMap>(this)->slot_i[i - 1] = c;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::set_slot", 0x00719770, &H::set_slot);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b111_07.cpp (game/b111_07)
namespace {
namespace game_b111_07 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::x87::Fpu;

const Fpu fpu{true};

using joan::hook::x87::ftol;

JOAN_COUNTER(c_fill_rect, "MapView::fill_rect");

struct H {
    // ------------------------------------------------------------------------------ 0x00719900
    void fill_rect(i32 x0, i32 y0, i32 x1, i32 y1) {
        c_fill_rect.hit();
        i32 w = 0;  // +0xf is always 0 or 1 (see the header)
        i32 h = 0;
        const i32 mode = jo::self<jo::MiniMap>(this)->visible;
        if (mode == 0) {
            w = jo::self<jo::MiniMap>(this)->view->i32_21;
            h = jo::self<jo::MiniMap>(this)->view->i32_25;
        } else if (mode == 1) {
            w = jo::self<jo::MiniMap>(this)->view->i32_29;
            h = jo::self<jo::MiniMap>(this)->view->i32_2d;
        }
        const i32 a = jo::self<jo::MiniMap>(this)->width;
        const i32 b = jo::self<jo::MiniMap>(this)->height;
        const double sx = fpu.div(a, w);  // fild / fidiv -> [ebp-0x18]
        const double sy = fpu.div(b, h);  // [ebp-0x1c]
        const double ox = jo::self<jo::MiniMap>(this)->scale_x;
        const double oy = jo::self<jo::MiniMap>(this)->scale_y;
        const double fx0 = fpu.sub(x0, ox);  // fild x0; fsub [+0x1d]
        const double fy0 = fpu.sub(oy, y0);  // fild y0; fsubr [+0x21]
        const double fx1 = fpu.sub(x1, ox);
        const double fy1 = fpu.sub(oy, y1);
        if (!(static_cast<double>(w) >= fx0)) return;  // fild w; fcomp: w < fx0 or NaN -> out
        if (!(static_cast<double>(h) >= fy1)) return;
        if (!(fx1 >= 0.0)) return;
        if (!(fy0 >= 0.0)) return;
        i32 c0 = ftol(fpu.add(fpu.mul(fx0, sx), 0.5));
        i32 c1 = ftol(fpu.mul(fx1, sx));
        i32 r0 = ftol(fpu.add(fpu.mul(fy1, sy), 0.5));
        i32 r1 = ftol(fpu.mul(fy0, sy));
        if (c0 < 0) c0 = 0;
        if (c1 >= 0xc8) c1 = 0xc7;
        if (r0 < 0) r0 = 0;
        if (r1 >= 0xc8) r1 = 0xc7;
        for (i32 row = r0; row <= r1; ++row) {
            u8* const dst = jo::self<jo::MiniMap>(this)->fog + static_cast<u32>(row) * 0xc8u + static_cast<u32>(c0);
            std::memset(joan::orig::arg::mptr(static_cast<void*>(dst)), i32{1}, joan::orig::arg::u32(static_cast<u32>(c1 - c0 + 1)));  // memset
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b111_07", name, addr, fn)
B("MapView::fill_rect_719900", 0x00719900, &H::fill_rect);
#undef B

}  // namespace game_b111_07
}  // namespace

// ---------------------------------------- from modules/game_b101_15.cpp (game/b101_15)
namespace {
namespace game_b101_15 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::hook::x87::Fpu;

const Fpu fpu{true};

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

i32 ftol(double v) { return joan::hook::x87::ftol(v); }

JOAN_COUNTER(c_reveal, "Minimap::reveal_circle");

struct H {
    // 0x00719b40 (ret 0xc)
    void reveal_circle(i32 px, i32 py, i32 r) {
        c_reveal.hit();
        const i32 mode = jo::self<jo::MiniMap>(this)->visible;
        i32 w, h;
        if (mode == 0) {
            w = jo::self<jo::MiniMap>(this)->view->i32_21;
            h = jo::self<jo::MiniMap>(this)->view->i32_25;
        } else if (mode == 1) {
            w = jo::self<jo::MiniMap>(this)->view->i32_29;
            h = jo::self<jo::MiniMap>(this)->view->i32_2d;
        } else {
            // The original reads uninitialised locals here (the frame size w / h): keep it on the
            // original body, or with fix/uninit (batch 118/03) skip the action - nothing is
            // revealed (nothing precedes the switch; the only effect of the function is the fog
            // bytes it sets).
            if (fix_uninit()) return;
            using Fn = void (H::*)(i32, i32, i32);
            (this->*joan::hook::original<Fn>(fn::Minimap_reveal_circle_body))(px, py, r);
            return;
        }
        const float sx = static_cast<float>(fpu.div(static_cast<double>(static_cast<i32>(jo::self<jo::MiniMap>(this)->width)), w));  // [ebp-0x18]
        const float sy = static_cast<float>(fpu.div(static_cast<double>(static_cast<i32>(jo::self<jo::MiniMap>(this)->height)), h));  // [ebp-0x1c]
        const float ratio = sx / sy;                                                                           // [ebp-0x20]
        const float dx = static_cast<float>(fpu.sub(static_cast<double>(px), jo::self<jo::MiniMap>(this)->scale_x));                // [ebp-0x2c]
        const float dy = static_cast<float>(fpu.sub(jo::self<jo::MiniMap>(this)->scale_y, static_cast<double>(py)));                // [ebp-0x28]
        const double rd = r;
        // fcompp / fcomp + test ah,1: leave when "less or unordered".
        if (!(static_cast<double>(w) >= fpu.sub(dx, rd))) return;
        if (!(static_cast<double>(h) >= fpu.sub(dy, rd))) return;
        if (!(fpu.add(rd, dx) >= 0.0)) return;
        if (!(fpu.add(rd, dy) >= 0.0)) return;
        // [ebp-0x40] = r * sx and [ebp-0x3c] / the re-rounded [ebp-0x38] are never read again (pure, dropped).
        const float ry = static_cast<float>(fpu.mul(rd, sy));  // [ebp-0x44]
        const float cy = dy * sy;                              // [ebp-0x38]
        const float lo_f = cy - ry;
        const float hi_f = cy + ry;
        i32 y0 = ftol(lo_f);
        i32 y1 = ftol(hi_f);
        if (y0 < 0) y0 = 0;
        if (y1 >= 0xc8) y1 = 0xc7;
        for (i32 y = y0; y <= y1; ++y) {
            const float yt = static_cast<float>(fpu.div(static_cast<double>(y), sy));
            const float t = yt - dy;  // [ebp-0x5c]
            const i32 rr = static_cast<i32>(static_cast<u32>(r) * static_cast<u32>(r));
            const float tt = t * t;
            const float q = static_cast<float>(fpu.sub(static_cast<double>(rr), tt));  // [ebp-0x60]
            if (!(q >= 0.0f)) continue;
            const double root = joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(q)));  // 0x004256d0 (CRT sqrt, st0)
            const float half = static_cast<float>(fpu.mul(root, ratio));  // [ebp-0x64]
            const float a0 = dx - half;
            const float a1 = a0 * sx;
            const float a2 = a1 + 0.5f;
            i32 x0 = ftol(a2);
            const float b0 = dx + half;
            const float b1 = b0 * sx;
            const float b2 = b1 + 0.5f;
            i32 x1 = ftol(b2);
            if (x1 < 0 || x0 >= 0xc8) continue;
            if (x0 < 0) x0 = 0;
            if (x1 >= 0xc8) x1 = 0xc7;
            u8* const dst = jo::self<jo::MiniMap>(this)->fog + static_cast<u32>(y) * 0xc8u + static_cast<u32>(x0);
            std::memset(joan::orig::arg::mptr(static_cast<void*>(dst)), 1, joan::orig::arg::u32(static_cast<u32>(x1 - x0 + 1)));  // memset
        }
    }
};

#define B15(name, addr, fn) JOAN_REPLACE("game/b101_15", name, addr, fn)
B15("Minimap::reveal_circle", 0x00719b40, &H::reveal_circle);
#undef B15

}  // namespace game_b101_15
}  // namespace

// ---------------------------------------- from modules/game_b115_11.cpp (fix/uninit)
namespace {
namespace game_b115_11 {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::hook::x87::Fpu;

const Fpu fpu{true};

void* const kTextures = obj(d::g_texture_array);

using joan::hook::x87::ftol;

using joan::call::thiscall;

void* const kRender = obj(d::g_sprites_a46128);
void* const kSprites = obj(d::g_sprites_a528c8);

// String object 0x194 bytes (0x005a1170 ctor, no dtor).
struct alignas(4) Str {
    u8 b[0x194];
};

JOAN_COUNTER(c_marker, "MiniMap::draw_marker_71a3b0 (fix/uninit)");

struct H {
    // ======================================================== 0x0071a3b0
    u32 draw_marker(i32 id) {
        c_marker.hit();
        u8* const st = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_territory_array))->get_420ff0(joan::orig::arg::i32(id)));  // 0x00420ff0
        u8* const dims = reinterpret_cast<u8*>(jo::self<jo::MiniMap>(this)->view);
        i32 dx = 0, dy = 0;  // [ebp-0xc] / [ebp-0x10] (fix: 0 for +0xf outside 0 / 1, never happens)
        const i32 mode = jo::self<jo::MiniMap>(this)->visible;
        if (mode == 0) {
            dx = jo::as_mini_res(dims)->i32_21;
            dy = jo::as_mini_res(dims)->i32_25;
        } else if (mode == 1) {
            dx = jo::as_mini_res(dims)->i32_29;
            dy = jo::as_mini_res(dims)->i32_2d;
        }
        const float sx = static_cast<float>(fpu.div(static_cast<double>(jo::self<jo::MiniMap>(this)->width), static_cast<double>(dx)));
        const float sy = static_cast<float>(fpu.div(static_cast<double>(jo::self<jo::MiniMap>(this)->height), static_cast<double>(dy)));
        const float px = static_cast<float>(
            fpu.mul(fpu.sub(static_cast<double>(jo::as_zone_def(st)->values[0]), static_cast<double>(jo::self<jo::MiniMap>(this)->scale_x)), sx));
        const float py = static_cast<float>(
            fpu.mul(fpu.sub(static_cast<double>(jo::self<jo::MiniMap>(this)->scale_y), static_cast<double>(jo::as_zone_def(st)->values[1])), sy));
        float zoom = 0.25f;  // fix: kind outside 0..4 -> kind 0
        switch (static_cast<i32>(static_cast<i8>(jo::as_zone_def(st)->settle))) {
            case 0: zoom = 0.25f; break;
            case 1: zoom = 0.5f; break;
            case 2: zoom = 1.0f; break;
            case 3: zoom = 2.0f; break;
            case 4: zoom = 4.0f; break;
            default: break;
        }
        zoom = static_cast<float>(fpu.mul(
            fpu.div(static_cast<double>(jo::as_mini_res(dims)->i32_29), static_cast<double>(dx)), static_cast<double>(zoom)));
        const double k200 = static_cast<double>(global<float>(d::flt_200));
        i32 rot = static_cast<i32>(jo::self<jo::MiniMap>(this)->rotation) % 4;
        if (rot < 0) rot += 4;  // fix: the original leaves the corners unwritten
        i32 cx = 0, cy = 0;     // [ebp-0x38] / [ebp-0x3c]
        switch (rot) {
            case 0:
                cx = ftol(px);
                cy = ftol(py);
                break;
            case 1:
                cx = ftol(py);
                cy = ftol(fpu.sub(k200, px));
                break;
            case 2:
                cx = ftol(-static_cast<double>(px));
                cy = ftol(-static_cast<double>(py));
                break;
            default:
                cx = ftol(fpu.sub(k200, py));
                cy = ftol(px);
                break;
        }
        u8 flag[3] = {0, 0, 0};
        const u32 phase = (global<u32>(d::g_now) - jo::as_zone_def(st)->time) % 0x465u;
        if (phase >= 0x271) return (global<u32>(d::g_now) - jo::as_zone_def(st)->time) / 0x465u;
        const u32 prev = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kTextures)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kTextures)->set_mode(1u);                 // 0x0060a740
        void* const pack = joan::orig::as_texture_manager(kTextures)->load(str(d::s_MINI_MP), 1, 0u);  // 0x006099c0 "MINI_MP"
        joan::orig::as_texture_manager(kTextures)->set_mode(joan::orig::arg::u32(prev));
        switch (phase / 0x7d) {
            case 0: flag[0] = 1; break;
            case 1: flag[0] = flag[1] = 1; break;
            case 2: flag[0] = flag[1] = flag[2] = 1; break;
            case 3: flag[1] = flag[2] = 1; break;
            case 4: flag[2] = 1; break;
            default: break;
        }
        Str name;
        joan::orig::as_string(&name)->construct();  // 0x005a1170
        for (i32 i = 0; i < 3; ++i) {
            if (flag[i] == 0) continue;
            const i32 owner = jo::as_zone_def(st)->owner;
            if (owner == 0) {
                joan::orig::as_string(&name)->assign(str(d::s_N));            // 0x005a1440
            } else if (owner == static_cast<i32>(jo::as_unit(jo::self<jo::MiniMap>(this)->unit)->side)) {
                joan::orig::as_string(&name)->assign(str(d::s_F_9dee04));
            } else {
                joan::orig::as_string(&name)->assign(str(d::s_E_9dee00));
            }
            joan::orig::as_string(&name)->append(joan::orig::arg::i32(i + 1));  // 0x005a15d0
            u8* const spr =
                joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(kSprites)->find(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_string(&name)->c_str()))));  // 0x00620b40 / 0x00417d90
            const double w = static_cast<double>(jo::view<i16>(jo::as_bmp_entry(spr)->width));
            const double h = static_cast<double>(jo::view<i16>(jo::as_bmp_entry(spr)->height));
            const double z = static_cast<double>(zoom);
            i32 x0 = ftol(fpu.sub(static_cast<double>(cx), fpu.mul(fpu.mul(w, z), 0.5)));
            i32 y0 = ftol(fpu.sub(static_cast<double>(cy), fpu.mul(fpu.mul(h, z), 0.5)));
            i32 x1 = ftol(fpu.add(static_cast<double>(x0), fpu.mul(w, z)));
            i32 y1 = ftol(fpu.add(static_cast<double>(y0), fpu.mul(h, z)));
            i32 u0 = 0, v0 = 0;
            i32 u1 = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(spr)->width)) - 1;
            i32 v1 = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(spr)->height)) - 1;
            if (x0 >= 200 || x1 < 0 || y0 >= 200 || y1 < 0) continue;
            if (x0 < 0) {
                u0 = ftol(fpu.sub(static_cast<double>(u0), fpu.div(static_cast<double>(x0), z)));
                x0 = 0;
            }
            if (y0 < 0) {
                v0 = ftol(fpu.sub(static_cast<double>(v0), fpu.div(static_cast<double>(y0), z)));
                y0 = 0;
            }
            if (x1 >= 200) {
                u1 = ftol(fpu.sub(static_cast<double>(u1), fpu.div(static_cast<double>(x1 - 199), z)));
                x1 = 199;
            }
            if (y1 >= 200) {
                v1 = ftol(fpu.sub(static_cast<double>(v1), fpu.div(static_cast<double>(y1 - 199), z)));
                y1 = 199;
            }
            const i32 ox = global<i32>(d::g_screen_w);
            u32 tmp = 0;  // [ebp-0x210]
            const u32 color = *joan::orig::arg::as<const u32*>(joan::orig::as_colour(&tmp)->construct(0xff, 0xff, 0xff, 0xff));  // 0x0042a260
            const char* const cs = joan::orig::arg::as<const char*>(joan::orig::as_string(&name)->c_str());
            // Word arguments 11..14 are read as words by 0x00633500.
            joan::orig::as_sprite_queue(kRender)->draw_named(joan::orig::arg::i16(x0 + ox - 0xd3), joan::orig::arg::i16(y0 + 10), 8u, joan::orig::arg::str(cs), joan::orig::arg::mptr(kSprites), joan::orig::arg::u32(color), 3, 0, 0, 0, joan::orig::arg::i16(u0), joan::orig::arg::i16(v0), joan::orig::arg::i16(u1), joan::orig::arg::i16(v1), 0, joan::orig::arg::i16(x1 + ox - 0xd3), joan::orig::arg::i16(y1 + 10));  // 0x00633500
        }
        return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(pack)));  // 0x0060a550
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: negative rotation % 4 -> + 4; kind byte outside 0..4 -> zoom of kind 0.
F("MiniMap::draw_marker_71a3b0", 0x0071a3b0, &H::draw_marker);
#undef F

}  // namespace game_b115_11
}  // namespace

// ---------------------------------------- from modules/game_b98_08_2.cpp (game/b98_08)
namespace {
namespace game_b98_08_2 {

using joan::call::cdecl_call;

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

JOAN_COUNTER(c_set_flag_name, "X::set_flag_by_name");

struct H {
    // ------------------------------------------------------------------------ 0x0071d150
    i32 set_flag_by_name(const char* name, i8 flag) {
        c_set_flag_name.hit();
        for (i32 i = 0; i < jo::self<jo::MiniRes>(this)->map_count; ++i) {
            jo::MiniResMap* const e = &jo::self<jo::MiniRes>(this)->maps[i];
            if (joan::crt::stricmp(joan::orig::arg::str(e->name), joan::orig::arg::str(name)) == 0) {  // _stricmp
                jo::view<i8>(e->enabled) = flag;
                return 1;
            }
        }
        return 0;
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::set_flag_by_name", 0x0071d150, &H::set_flag_by_name);
#undef B98

}  // namespace game_b98_08_2
}  // namespace

// ---------------------------------------- from modules/game_b97_08_2.cpp (game/b97_08)
namespace {
namespace game_b97_08_2 {

using joan::call::cdecl_call;

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::orig::helpers::stricmp_;

JOAN_COUNTER(c_byte_by_name, "Table::byte_by_name");

struct H {
    // ------------------------------------------------------------------ 0x0071d1e0
    i32 byte_by_name(const char* name) {
        c_byte_by_name.hit();
        for (i32 i = 0; i < jo::self<jo::MiniRes>(this)->map_count; ++i) {
            const jo::MiniResMap* const e = &jo::self<jo::MiniRes>(this)->maps[i];
            if (stricmp_(e->name, name) == 0) return static_cast<i32>(static_cast<i8>(e->enabled));
        }
        return 0;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Table::byte_by_name", 0x0071d1e0, &H::byte_by_name);
#undef B97

}  // namespace game_b97_08_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::MiniMap::zero() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_08::H::zero_480>(this)); }  // 0x70e190
auto joan::orig::Hud::init_70e1d0() -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b108_10::H::hud_init>(this)); }  // 0x70e1d0
auto joan::orig::Screen::release_70e3c0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_06::H::screen_release>(this)); }  // 0x70e3c0
auto joan::orig::MiniRes::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_ui8_3::H::view_delete>(this, a0)); }  // 0x70e4b0
auto joan::orig::MiniMap::draw(std::uint32_t a0) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b108_04::H::minimap_draw_70e500>(this, a0)); }  // 0x70e500
auto joan::orig::MiniMap::handle_zoom_key() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_20::H::map_button_click>(this)); }  // 0x70eb20
auto joan::orig::MiniMap::update_70ec50() -> void { joan::bind::member<&::game_b109_10::H::mini_update>(this); }  // 0x70ec50
auto joan::orig::Panel::clear_unit() -> void { joan::bind::member<&::game_ui8_3::H::clear_unit>(this); }  // 0x70f9b0
auto joan::orig::MiniMap::unit_to_map(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_08::H::to_screen>(this, a0, a1)); }  // 0x70f9e0
auto joan::orig::Screen::offset(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_06_2::H::screen_offset>(this, a0, a1, a2, a3)); }  // 0x70fa70
auto joan::orig::MiniMap::pos_to_map(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_08::H::random_offset>(this, a0, a1)); }  // 0x70fae0
auto joan::orig::MiniMap::track_unit(std::int16_t a0) -> void { joan::bind::member<&::game_b87_06::H::set_unit>(this, a0); }  // 0x70fb70
auto joan::orig::MiniMap::draw_unit(void* a0) -> void { joan::bind::member<&::game_b111_06::H::draw_unit_70fbe0>(this, a0); }  // 0x70fbe0
auto joan::orig::Marker::update() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_06_2::H::marker_update>(this)); }  // 0x714040
auto joan::orig::MiniMap::draw_tiles() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b110_16::H::draw_tiles_7141a0>(this)); }  // 0x7141a0
auto joan::orig::MiniRes::destruct() -> void { joan::bind::member<&::game_b82_15_2::H::view_dtor>(this); }  // 0x715d20
auto joan::orig::Panel::set_unit(std::int16_t a0) -> void { joan::bind::member<&::game_ui2::H::set_unit>(this, a0); }  // 0x715d50
auto joan::orig::MiniMap::draw_markers(std::int32_t a0) -> void { joan::bind::member<&::game_b115_10::H::draw_markers>(this, a0); }  // 0x716010
auto joan::orig::MiniMap::draw_view_716be0() -> void { joan::bind::member<&::game_b111_19::H::draw_view>(this); }  // 0x716be0
auto joan::orig::MiniMap::zoom_mode() -> std::uint16_t { return joan::bind::conv<std::uint16_t>(joan::bind::member<&::game_b91_04::H::get_f_717220>(this)); }  // 0x717220
auto joan::orig::MiniMap::handle_click() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b111_12::H::map_click>(this)); }  // 0x717240
auto joan::orig::Marker::visible() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b90_06_2::H::marker_visible>(this)); }  // 0x717650
auto joan::orig::MiniMap::to_screen(void* a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b111_13::H::to_screen>(this, a0)); }  // 0x717700
auto joan::orig::MiniMap::near_tracked_unit(float a0, float a1, std::uint32_t a2) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b87_06::H::in_range>(this, a0, a1, a2)); }  // 0x717a90
auto joan::orig::MiniMap::draw_clipped_line(float a0, float a1, std::uint32_t a2, float a3, float a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7) -> void { joan::bind::member<&::game_b99_10::H::draw_clipped>(this, a0, a1, a2, a3, a4, a5, a6, a7); }  // 0x717f40
auto joan::orig::MiniMap::set_waypoint_style(std::int16_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b100_16::H::set_slot_7180f0>(this, a0, a1, a2); }  // 0x7180f0
auto joan::orig::Map::attack_marker_718140(float a0, float a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b108_12::H::attack_marker_718140>(this, a0, a1, a2); }  // 0x718140
auto joan::orig::MiniMap::update_unit(void* a0) -> void { joan::bind::member<&::game_b104_14::H::update_unit_7190b0>(this, a0); }  // 0x7190b0
auto joan::orig::MiniMap::set_reveal_rect(std::int32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::set_slot>(this, a0, a1, a2, a3, a4)); }  // 0x7196e0
auto joan::orig::MiniMap::set_reveal_circle(std::int32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> void { joan::bind::member<&::game_b94_08::H::set_slot>(this, a0, a1, a2, a3); }  // 0x719770
auto joan::orig::MiniMap::set_reveal_quad(std::int32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_10::H::set_slot_7197f0>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x7197f0
auto joan::orig::MiniMap::clear_reveal(std::int32_t a0) -> void { joan::bind::member<&::game_b91_04::H::clear_slot>(this, a0); }  // 0x7198d0
auto joan::orig::MiniMap::reveal_rect(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> void { joan::bind::member<&::game_b111_07::H::fill_rect>(this, a0, a1, a2, a3); }  // 0x719900
auto joan::orig::MiniMap::reveal_circle(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::member<&::game_b101_15::H::reveal_circle>(this, a0, a1, a2); }  // 0x719b40
auto joan::orig::MiniMap::reveal_quad(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7) -> void { joan::bind::member<&::game_b104_14::H::reveal_quad_719e60>(this, a0, a1, a2, a3, a4, a5, a6, a7); }  // 0x719e60
auto joan::orig::MiniMap::draw_marker_71a3b0(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b115_11::H::draw_marker>(this, a0)); }  // 0x71a3b0
auto joan::orig::MiniMap::draw_gold_71aba0(void* a0) -> void { joan::bind::member<&::game_b111_19::H::draw_gold>(this, a0); }  // 0x71aba0
auto joan::orig::MiniRes::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_ui8_2::H::minires_ctor>(this)); }  // 0x71aff0
auto joan::orig::MiniRes::load(void* a0, std::int16_t a1) -> void { joan::bind::member<&::game_ui8_2::H::minires_load>(this, a0, a1); }  // 0x71b020
auto joan::orig::MiniRes::free() -> void { joan::bind::member<&::game_ui8::H::minires_free>(this); }  // 0x71b080
auto joan::orig::Overlay2D::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_15::H::item_delete>(this, a0)); }  // 0x71b330
auto joan::orig::MiniRes::find_frame(std::int16_t a0, std::int16_t a1, std::int16_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_ui8::H::minires_find>(this, a0, a1, a2)); }  // 0x71b3b0
auto joan::orig::Zones::find_71b520(std::int16_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_06::H::zones_find>(this, a0, a1, a2, a3)); }  // 0x71b520
auto joan::orig::MiniRes::read_frames(void* a0, std::int16_t a1) -> void { joan::bind::member<&::game_b82_15::H::minires_read_frames>(this, a0, a1); }  // 0x71b6a0
auto joan::orig::MiniRes::read_maps(std::int16_t a0) -> void { joan::bind::member<&::game_b82_15::H::minires_read_maps>(this, a0); }  // 0x71bf10
auto joan::orig::MiniRes::set_flag_by_name(const char* a0, std::int8_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_08_2::H::set_flag_by_name>(this, a0, a1)); }  // 0x71d150
auto joan::orig::Table::byte_by_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_08_2::H::byte_by_name>(this, a0)); }  // 0x71d1e0
