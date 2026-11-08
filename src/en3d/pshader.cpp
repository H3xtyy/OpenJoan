// en3d/pshader.cpp: functions of the original d:\ww\en3d\pshader.cpp (re/attribution.csv).
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
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::cdecl_call;
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

void* const kHeap = obj(d::g_mem);   // debug allocator

using joan::call::thiscall;

}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
//
//      0x005e5960 PShaderList::release              (thiscall, ret 0) - device->DeletePixelShader
//                 (vtable +0x168, stdcall COM) for each, delete, debug free.
//
//  - Device / COM calls (0x005e5960 DeletePixelShader, Release in 0x00606f90 / 0x0060f700) are
//    vtable calls exactly as in the original.
namespace {
namespace game_b96_06 {

// COM (stdcall, this pushed) call through vtable slot (slot index = x86 byte offset / 4).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t slot, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[slot])(object, args...);
}

using u8 = std::uint8_t;

void heap_free(void* p, img::Addr file, i32 line) {
    joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}
using joan::orig::helpers::op_delete;

JOAN_COUNTER(c_ps_release, "PShaderList::release");

struct H {
    // ------------------------------------------------------------------ 0x005e5960
    void ps_release() {
        c_ps_release.hit();
        if (jo::self<jo::PShaderList>(this)->entries != nullptr) {
            for (i32 i = 0; i < jo::self<jo::PShaderList>(this)->count; ++i) {
                const u32 handle = jo::self<jo::PShaderList>(this)->entries[i].p->handle;
                void* const dev = global<void*>(d::g_game_device);
                com_call<i32>(dev, 0x168 / 4, handle);  // DeletePixelShader
                op_delete(jo::view<void*>(jo::self<jo::PShaderList>(this)->entries[i].p));
            }
            heap_free(jo::view<void*>(jo::self<jo::PShaderList>(this)->entries), d::s_d_ww_en3d_pshader_cpp, static_cast<i32>(global<i16>(d::g_a173f8)) + 8);
            jo::self<jo::PShaderList>(this)->entries = nullptr;
        }
        jo::view<u32>(jo::self<jo::PShaderList>(this)->count) = 0;
        jo::view<u32>(jo::self<jo::PShaderList>(this)->capacity) = 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("PShaderList::release", 0x005e5960, &H::ps_release);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/en3d_helpers4.cpp (en3d/helpers4)
namespace {
namespace en3d_helpers4 {

using Device = joan::d3d8::IDirect3DDevice8;

Device* game_device() { return global<Device*>(d::g_game_device); }

struct H {
    // --- Pixel shader cache (en3d/pshader.cpp), this = { entries**, capacity, count } ---------------
    // 0x005e5a50: the clip-plane pixel shader for (vertex type 7, key), loaded from
    // SHADER\clip_at.pso on first use. Only with PS >= 1.0 (caps 0xa5253c), no software VP
    // (0xa52514) and user clip planes enabled. The creating call itself returns null (as the
    // original); clip_am.pso is unreachable there (`(key == 0x20000) | 0x20` is never 0).
    void* ps_lookup(std::int32_t vertex_type, std::uint32_t key) {
        if (global<std::uint32_t>(d::g_a5253c) < 0xffff0100u) return nullptr;
        if (static_cast<std::int8_t>(vertex_type) != 7) return nullptr;
        if (global<std::uint32_t>(d::g_a52514) != 0) return nullptr;
        joan::d3d8::DWORD clip_planes = 0;
        game_device()->GetRenderState(152, &clip_planes);  // CLIPPLANEENABLE
        if (clip_planes == 0) return nullptr;
        void** const entries = jo::view<void**>(jo::self<jo::PixelShaderCache>(this)->entries);
        for (std::int32_t i = 0; i < jo::self<jo::PixelShaderCache>(this)->count; ++i)
            if (jo::as_pixel_shader_entry(entries[i])->key == key) return entries[i];

        alignas(8) std::uint8_t path[0x1a0];
        alignas(8) std::uint8_t file[((sizeof(jo::FileMem) > 0x120 ? sizeof(jo::FileMem) : 0x120) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // 0x120 on x86; FileMem holds pointers
        joan::orig::as_string(path)->construct();                                           // 0x005a1170
        joan::orig::as_string(path)->assign(joan::orig::arg::str(&img::global<const char>(d::s_SHADER_clip_at_pso)));  // "SHADER\clip_at.pso"
        joan::orig::as_file_mem(file)->construct();                                                  // 0x00587f80
        joan::orig::as_file_mem(file)->open_588000(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str())), 1, static_cast<char>(joan::orig::arg::i32(0)));  // 0x00588000 open
        const std::uint32_t size = joan::orig::arg::as<std::uint32_t>(joan::orig::as_file_mem(file)->file_size());              // 0x00588830
        void* const code = joan::crt::operator_new(joan::orig::arg::u32((size >> 2) << 2));               // operator new
        joan::orig::as_file_mem(file)->file_read(joan::orig::arg::mptr(code), static_cast<std::uint32_t>(joan::orig::as_file_mem(file)->file_size()));  // 0x00588a80 read
        joan::d3d8::DWORD handle = 0;
        if (game_device()->CreatePixelShader(static_cast<const joan::d3d8::DWORD*>(code), &handle) == 0) {
            std::uint32_t* const entry = joan::orig::arg::as<std::uint32_t*>(joan::crt::operator_new(8u));
            entry[0] = key;
            entry[1] = handle;
            if (jo::self<jo::PixelShaderCache>(this)->capacity <= jo::self<jo::PixelShaderCache>(this)->count) {
                jo::self<jo::PixelShaderCache>(this)->capacity = jo::self<jo::PixelShaderCache>(this)->capacity + 2;
                const std::int32_t line = global<std::int16_t>(d::g_a173fc) + 0x4f;
                jo::view<void**>(jo::self<jo::PixelShaderCache>(this)->entries) = joan::orig::arg::as<void**>(joan::orig::as_mem(img::obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::view<void**>(jo::self<jo::PixelShaderCache>(this)->entries)), joan::orig::arg::u32(static_cast<std::uint32_t>(jo::self<jo::PixelShaderCache>(this)->capacity * static_cast<std::int32_t>(sizeof(void*)))), joan::orig::arg::str(&img::global<const char>(d::s_d_ww_en3d_pshader_cpp)), joan::orig::arg::i32(line)));  // 0x0058eb30 resize
            }
            jo::view<void**>(jo::self<jo::PixelShaderCache>(this)->entries)[jo::self<jo::PixelShaderCache>(this)->count] = entry;
            jo::self<jo::PixelShaderCache>(this)->count = jo::self<jo::PixelShaderCache>(this)->count + 1;
        }
        joan::crt::operator_delete(joan::orig::arg::mptr(code));  // operator delete
        joan::orig::as_file_mem(file)->destruct();    // 0x00587fd0
        return nullptr;
    }

    // 0x005e5de0: pixel shader constant c0 = the texture factor's bytes (B, G, B, A as 0..255 -
    // the original reads the blue byte twice) when tinted, else white.
    void ps_apply(void* entry, std::uint32_t tint) {
        if (entry == nullptr) return;
        if ((tint & 0xff) != 0) {
            joan::d3d8::DWORD factor = 0;
            game_device()->GetRenderState(60, &factor);  // TEXTUREFACTOR
            std::uint8_t c[4];
            joan::orig::as_colour(c)->set(joan::orig::arg::u32(static_cast<std::uint32_t>(factor)));  // 0x005e9610
            const float k[4] = {static_cast<float>(c[0]), static_cast<float>(c[1]), static_cast<float>(c[0]),
                                static_cast<float>(c[3])};
            game_device()->SetPixelShaderConstant(0, k, 1);
        } else {
            const float k[4] = {1.0f, 1.0f, 1.0f, 1.0f};
            game_device()->SetPixelShaderConstant(0, k, 1);
        }
    }
};

#define HELPER4(name, addr, fn) JOAN_REPLACE("en3d/helpers4", name, addr, fn)
HELPER4("PixelShaderFactory::get_shader", 0x005e5a50, &H::ps_lookup);
HELPER4("PixelShaderCache::apply", 0x005e5de0, &H::ps_apply);
#undef HELPER4

}  // namespace en3d_helpers4
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
//
//    - PShader::free_all 0x005e5f10 (ret 0): DeletePixelShader (device vtable +0x168) and
//      delete for each entry, heap free of the array ("pshader.cpp"), clears +0 / +4 / +8.
namespace {
namespace game_b100_11 {

// COM method (stdcall, object pushed first) through the object's vtable (slot index).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

using joan::orig::helpers::op_delete;
i32 line_at(img::Addr a, i32 add_) { return static_cast<i32>(global<i16>(a)) + add_; }
void heap_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

JOAN_COUNTER(c_ps_free, "PShader::free_all");

struct H {
    // ------------------------------------------------------------------ 0x005e5f10
    void ps_free_all() {
        c_ps_free.hit();
        if (jo::view<void*>(jo::self<jo::PShader>(this)->entries) != nullptr) {
            for (i32 i = 0; i < jo::self<jo::PShader>(this)->count; ++i) {
                void* const dev = global<void*>(d::g_game_device);
                com_call<i32>(dev, 0x168 / 4, (jo::as_pixel_shader_entry(jo::view<void**>(jo::self<jo::PShader>(this)->entries)[i]))->handle);  // DeletePixelShader
                op_delete(jo::view<void**>(jo::self<jo::PShader>(this)->entries)[i]);
            }
            heap_free(jo::view<void*>(jo::self<jo::PShader>(this)->entries), str(d::s_d_ww_en3d_pshader_cpp), line_at(d::g_a17400, 8));  // "pshader.cpp"
            jo::self<jo::PShader>(this)->entries = nullptr;
        }
        jo::view<u32>(jo::self<jo::PShader>(this)->count) = 0;
        jo::view<u32>(jo::self<jo::PShader>(this)->capacity) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("PShader::free_all", 0x005e5f10, &H::ps_free_all);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::PixelShaderCache::destruct() -> void { joan::bind::member<&::game_b96_06::H::ps_release>(this); }  // 0x5e5960
auto joan::orig::PixelShaderCache::lookup(std::int32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers4::H::ps_lookup>(this, a0, a1)); }  // 0x5e5a50
auto joan::orig::PixelShaderCache::apply(void* a0, std::uint32_t a1) -> void { joan::bind::member<&::en3d_helpers4::H::ps_apply>(this, a0, a1); }  // 0x5e5de0
auto joan::orig::PixelShaderCache::free_all() -> void { joan::bind::member<&::game_b100_11::H::ps_free_all>(this); }  // 0x5e5f10
