// game/t_scrres.cpp: functions of the original d:\ww\t_scrres.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cstdarg>
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
//
//      0x004dea50 ScrRes::release      (thiscall, ret 0) - if byte +6 == 1: 0x004deb10, frees
//                 +0 (0x0058ed70, "d:\ww\t_scrres.cpp"), clears +0 / word +4, byte +6 = 0.
//
//  - Wrappers return the eax the originals leave (0x005a3090's result, the last appended
//    string call for 0x004e0680, this / 0x004deb10's result for 0x004dea50).
namespace {
namespace game_b98_04 {

using joan::img::str;

using i8 = std::int8_t;
using u16 = std::uint16_t;

void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_release, "ScrRes::release");

struct H {
    // ------------------------------------------------------------------------ 0x004dea50
    std::uintptr_t release() {  // eax: this or 0
        c_release.hit();
        if (static_cast<i32>(static_cast<i8>(jo::self<jo::AnimDb>(this)->loaded)) != 1) return reinterpret_cast<std::uintptr_t>(this);
        joan::orig::as_anim_db(this)->touch_all();  // 0x004deb10
        std::uintptr_t r = 0;  // touch_all returns nothing; every caller of release discards its result
        if (jo::self<jo::AnimDb>(this)->records != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_a049b4)) + 7;
            joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(jo::self<jo::AnimDb>(this)->records), str(d::s_d_ww_t_scrres_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::AnimDb>(this)->records = nullptr;
            jo::view<u16>(jo::self<jo::AnimDb>(this)->count) = 0;
            r = reinterpret_cast<std::uintptr_t>(this);
        }
        jo::self<jo::AnimDb>(this)->loaded = 0;
        return r;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("TextureEffectRes::deinit", 0x004dea50, &H::release);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
//
//    - ScrRes::init_4deae0 0x004deae0 (ret 0): 0x004deb70, byte +6 = 1, returns this.
namespace {
namespace game_b108_01 {

struct H {
    // -------------------------------------------------------------- 0x004deae0
    void* scrres_init() {
        joan::orig::as_anim_db(this)->load_text_effects();  // 0x004deb70
        jo::self<jo::AnimDb>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("ScrRes::init_4deae0", 0x004deae0, &H::scrres_init);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
//
//      0x004deb10 ScrRes::touch_all       (ret 0) - for i = 1..short +4: 0x004b27b0(i), result
//                                          dropped.
namespace {
namespace game_b97_04 {

JOAN_COUNTER(c_touch_all, "ScrRes::touch_all");

struct H {
    // ------------------------------------------------------------------------ 0x004deb10
    void touch_all() {
        c_touch_all.hit();
        for (i16 i = 1; static_cast<i32>(i) <= static_cast<i32>(jo::self<jo::AnimDb>(this)->count); i = static_cast<i16>(i + 1)) {
            joan::orig::as_anim_db(this)->entry(joan::orig::arg::u32(static_cast<i32>(i)));  // 0x004b27b0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("ScrRes::touch_all", 0x004deb10, &H::touch_all);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b107_06.cpp (game/b107_06)
//
//    - TextEffects::load 0x004deb70 (thiscall, ret 0; t_scrres.cpp): reads RESOURCE\texteff.res
//      with a resource reader on the stack (0x004179f0 / 0x0059b920 / 0x00417a30), allocates
//      count * 0xd51 bytes (0x0058e7b0 on 0xa48838), zeroes them, and for each entry 1..count
//      stores the index word and parses it from a text buffer (0x00589780 / 0x004ded90 /
//      0x00589890). Caller (0x004deaef) ignores eax. Stack objects are one typed object each
//      (in the original's address order); the path object has no destructor (as in the original). SEH frame: normal path only.
namespace {
namespace game_b107_06 {

JOAN_COUNTER(c_texteff_load, "TextEffects::load");

// ---- 0x004deb70 ----
// Original frame: esp = ebp-0x4c0 after the prologue. Objects (ebp-relative):
// reader -0x148 (fields up to +0x135), path -0x2dc (0x191 bytes), text -0x480 (0x19a bytes); one object per
// local here (FileTxt / ResourceIdxL hold pointers: larger on x64).
struct Frame {
    jo::FileTxt text;
    jo::String path;
    jo::ResourceIdxL reader;
};
struct H {
    void texteff_load() {
        c_texteff_load.hit();
        constexpr i32 kRec = static_cast<i32>(sizeof(jo::TexScriptRec));
        alignas(8) Frame frame;
        u8* const reader = reinterpret_cast<u8*>(&frame.reader);
        u8* const path = reinterpret_cast<u8*>(&frame.path);
        u8* const text = reinterpret_cast<u8*>(&frame.text);

        joan::orig::as_res_idx(reader)->construct();                                                // 0x004179f0
        joan::orig::as_string(path)->construct();                                           // 0x005a1170
        joan::orig::as_string(path)->assign(joan::orig::arg::str(&img::global<const char>(d::s_RESOURCE)));  // 0x005a1440
        joan::orig::as_string(path)->append(joan::orig::arg::str(&img::global<const char>(d::s_texteff_res)));  // 0x005a1550
        void* const name = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());      // 0x00417d90
        joan::orig::as_res_idx_l(reader)->init(joan::orig::arg::str(name), static_cast<char>(1), static_cast<char>(0));  // 0x0059b920

        jo::self<jo::AnimDb>(this)->count = *reinterpret_cast<const i16*>(&jo::as_res_idx_l(reader)->rec_count);  // the low word (movsx word)
        {
            const i32 line = static_cast<i32>(global<i16>(d::g_a049b8)) + 0xb;
            const i32 size = static_cast<i32>(jo::self<jo::AnimDb>(this)->count) * kRec;
            jo::view<u8*>(jo::self<jo::AnimDb>(this)->records) = joan::orig::arg::as<u8*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size), joan::orig::arg::str(&img::global<const char>(d::s_d_ww_t_scrres_cpp)), joan::orig::arg::i32(line)));  // 0x0058e7b0
        }
        std::memset(joan::orig::arg::mptr(jo::view<u8*>(jo::self<jo::AnimDb>(this)->records)), 0, joan::orig::arg::u32(static_cast<i32>(jo::self<jo::AnimDb>(this)->count) * kRec));  // memset

        for (i32 i = 1; i <= jo::as_res_idx_l(reader)->rec_count; ++i) {
            jo::TexScriptRec* const rec = jo::as_tex_script_rec(jo::view<u8*>(jo::self<jo::AnimDb>(this)->records) + i * kRec - kRec);
            rec->id = static_cast<i16>(i);
            const i32 len = joan::orig::arg::as<i32>(joan::orig::as_res_idx_l(reader)->get_data_size(joan::orig::arg::i32(i)));  // 0x0059bdd0
            void* const src = joan::orig::arg::as<void*>(joan::orig::as_res_idx_l(reader)->read_rec(joan::orig::arg::i32(i)));  // 0x0059bf70
            joan::orig::as_file_txt(text)->construct_mem(joan::orig::arg::str(src), joan::orig::arg::i32(len));                 // 0x00589780
            joan::orig::as_formation(this)->parse_4ded90(joan::orig::arg::mptr(text), joan::orig::arg::mptr(rec));                 // 0x004ded90
            joan::orig::as_file_txt(text)->destruct();                            // 0x00589890
        }
        joan::orig::as_archive(reader)->dtor();  // 0x00417a30
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_06", name, addr, fn)
B("TextureEffectRes::init_all_info", 0x004deb70, &H::texteff_load);
#undef B

}  // namespace game_b107_06
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
//
//      0x004dea20 Obj::forward_4dea50     (thiscall, ret 0) - returns 0x004dea50(this).
namespace {
namespace game_b99_05 {

using joan::call::thiscall;

using i32 = std::int32_t;

struct H {
    // ------------------------------------------------------------------------ 0x004dea20
    i32 forward_4dea50() { return joan::orig::arg::as<i32>(joan::orig::as_anim_db(this)->release()); }  // 0x004dea50
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Obj::forward_4dea50", 0x004dea20, &H::forward_4dea50);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b106_17.cpp (game/b106_17)
//
//    - Formation::parse_4ded90 0x004ded90 (thiscall(reader, rec), ret 8, void - the caller
namespace {
namespace game_b106_17 {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_parse, "Formation::parse_4ded90");

// ---- CRT -------------------------------------------------------------------------------
using joan::orig::helpers::crt_atoi;
void crt_strcpy(char* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
double crt_atof(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }  // st0 double
using joan::orig::helpers::crt_strcmp;
i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

// Element of a 24-entry stage table at byte offset k (the original's own address arithmetic).
template <class T>
T& slot(T (&table)[24], i32 k) { return *reinterpret_cast<T*>(reinterpret_cast<u8*>(table) + k); }

struct H {
    // 0x004ded90
    char* token(void* reader) { return joan::orig::as_file_txt(reader)->get_token(i32{1}); }  // 0x00589c70
    char* token2(void* reader) {
        token(reader);
        return token(reader);
    }
    float tok_float(void* reader) { return static_cast<float>(crt_atof(token2(reader))); }
    u32 tok_ref(void* reader) { return joan::orig::arg::as<u32>(joan::orig::as_anim_db(this)->parse_tex_op(joan::orig::arg::str(token2(reader)))); }  // 0x004df9f0
    void parse_4ded90(void* reader, u8* rec_bytes) {
        c_parse.hit();
        jo::TexScriptRec* const rec = jo::as_tex_script_rec(rec_bytes);
        crt_atoi(token2(reader));  // [ebp-0xc], unused
        rec->layer_count = static_cast<u16>(crt_atoi(token2(reader)));
        crt_strcpy(rec->name, token2(reader));
        for (i32 i = 0; i < static_cast<i32>(jo::view<i16>(rec->layer_count)); ++i) {
            rec->material_indices[i] = static_cast<u16>(crt_atoi(token2(reader)));
            rec->stage_count[i] = static_cast<u16>(crt_atoi(token2(reader)));
            rec->tfactor_alphas[i] = static_cast<u16>(crt_atoi(token2(reader)));
            rec->f32_49[i] = tok_float(reader);
            for (i32 j = 0; j < static_cast<i32>(jo::view<i16>(rec->stage_count[i])); ++j) {
                const i32 k = i * 4 * static_cast<i32>(sizeof(u32)) + j * static_cast<i32>(sizeof(u32));  // byte offset of stage slot i * 4 + j in the 24-entry tables
                slot(rec->tex_op_61, k) = tok_ref(reader);
                slot(rec->tex_op_c1, k) = tok_ref(reader);
                slot(rec->tex_op_121, k) = tok_ref(reader);
                slot(rec->tex_op_181, k) = tok_ref(reader);
                slot(rec->tex_op_1e1, k) = tok_ref(reader);
                slot(rec->tex_op_241, k) = tok_ref(reader);
                slot(rec->tex_op_2a1, k) = tok_ref(reader);
                slot(rec->f32_301, k) = tok_float(reader);
                slot(rec->i32_361, k) = crt_atoi(token2(reader));
                crt_strcpy(rec->stage_names + i * 0x84 + j * 0x21, token2(reader));
                slot(rec->scroll_speeds_u, k) = tok_float(reader);
                slot(rec->address_modes_u, k) = crt_strcmp(token2(reader), str(d::s_true)) != 0 ? 3u : 1u;  // "true"
                slot(rec->scroll_speeds_v, k) = tok_float(reader);
                slot(rec->address_modes_v, k) = crt_strcmp(token2(reader), str(d::s_true)) != 0 ? 3u : 1u;
                slot(rec->has_extra, k) = crt_strcmp(token2(reader), str(d::s_true)) != 0 ? 0u : 1u;
                if (slot(rec->has_extra, k) != 0) {
                    slot(rec->bump_env_mat00s, k) = tok_float(reader);
                    slot(rec->bump_env_mat01s, k) = tok_float(reader);
                    slot(rec->bump_env_mat10s, k) = tok_float(reader);
                    slot(rec->bump_env_mat11s, k) = tok_float(reader);
                    slot(rec->bump_env_lscales, k) = tok_float(reader);
                    slot(rec->bump_env_loffsets, k) = tok_float(reader);
                }
                slot(rec->sphere_map_flags, k) = crt_strcmp(token2(reader), str(d::s_true)) != 0 ? 0u : 1u;
                slot(rec->projected_flags, k) = crt_strcmp(token2(reader), str(d::s_true)) != 0 ? 0u : 1u;
                slot(rec->scroll_offsets_u, k) = tok_float(reader);
                slot(rec->scroll_offsets_v, k) = tok_float(reader);
                slot(rec->scales_u, k) = tok_float(reader);
                slot(rec->scales_v, k) = tok_float(reader);
                const char* const mode = token2(reader);
                if (crt_stricmp(mode, str(d::s_NORMAL)) == 0) {  // "NORMAL"
                    *(rec->object_mode + i * 4 + j) = 0;
                } else if (crt_stricmp(mode, str(d::s_OBJECT)) == 0) {  // "OBJECT"
                    *(rec->object_mode + i * 4 + j) = 1;
                }
            }
        }
        for (i32 i = global<i32>(d::g_model_res_mgr); i > 0; --i) {
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(i)));  // 0x0041a180
            if (crt_stricmp(rec->name, jo::view<const char*>(jo::as_model_entry(e)->file)) == 0) {
                jo::as_model_entry(e)->anim_db_id = static_cast<i32>(rec->id);
                break;
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_17", name, addr, fn)
B("TextureEffectRes::read_one_texture_effect_info_100", 0x004ded90, &H::parse_4ded90);
#undef B

}  // namespace game_b106_17
}  // namespace

// ---------------------------------------- from modules/game_b104_05.cpp (game/b104_05)
namespace {
namespace game_b104_05 {

using joan::call::cdecl_call;
using joan::img::str;

using i32 = std::int32_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_texop, "TexOp::parse");

using joan::orig::helpers::stricmp_;  // _stricmp

struct TexOpName {
    img::Addr name;
    u32 value;
};
const TexOpName kTexOps[36] = {
    {d::s_disable, 0x1},         // "disable"
    {d::s_arg1, 0x2},         // "arg1"
    {d::s_arg2, 0x3},         // "arg2"
    {d::s_modulate, 0x4},         // "modulate"
    {d::s_modulate2, 0x5},         // "modulate2"
    {d::s_modulate4, 0x6},         // "modulate4"
    {d::s_add, 0x7},         // "add"
    {d::s_addsigned, 0x8},         // "addsigned"
    {d::s_addsigned2, 0x9},         // "addsigned2"
    {d::s_subtract, 0xa},         // "subtract"
    {d::s_addsmooth, 0xb},         // "addsmooth"
    {d::s_blenddiffusealpha, 0xc},         // "blenddiffusealpha"
    {d::s_blendtexturealpha, 0xd},         // "blendtexturealpha"
    {d::s_blendfactoralpha, 0xe},         // "blendfactoralpha"
    {d::s_blendtexturealphapm, 0xf},         // "blendtexturealphapm"
    {d::s_blendcurrentalpha, 0x10},        // "blendcurrentalpha"
    {d::s_premodulate, 0x11},        // "premodulate"
    {d::s_modulatealpha_addcolor, 0x12},        // "modulatealpha_addcolor"
    {d::s_modulatecolor_addalpha, 0x13},        // "modulatecolor_addalpha"
    {d::s_modulateinvalpha_addcolor, 0x14},        // "modulateinvalpha_addcolor"
    {d::s_modulateinvcolor_addalpha, 0x15},        // "modulateinvcolor_addalpha"
    {d::s_bumpenvmap, 0x16},        // "bumpenvmap"
    {d::s_bumpenvmapluminance, 0x17},        // "bumpenvmapluminance"
    {d::s_dotproduct3, 0x18},        // "dotproduct3"
    {d::s_multiplyadd, 0x19},        // "multiplyadd"
    {d::s_lerp, 0x1a},        // "lerp"
    {d::s_force_dword, 0x7fffffff},  // "force_dword"
    {d::s_current, 0x1},         // "current"
    {d::s_diffuse, 0x0},         // "diffuse"
    {d::s_selectmask, 0xf},         // "selectmask"
    {d::s_specular, 0x4},         // "specular"
    {d::s_temp, 0x5},         // "temp"
    {d::s_texture, 0x2},         // "texture"
    {d::s_tfactor, 0x3},         // "tfactor"
    {d::s_complement, 0x12},        // "complement"
    {d::s_alphareplicate, 0x20},        // "alphareplicate"
};

struct H {
    // ------------------------------------------------------------------------ 0x004df9f0
    u32 texop_parse(const char* name) {
        c_texop.hit();
        for (const TexOpName& e : kTexOps) {
            if (stricmp_(name, str(e.name)) == 0) return e.value;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_05", name, addr, fn)
B("TextureEffectRes::code_to_D3D", 0x004df9f0, &H::texop_parse);
#undef B

}  // namespace game_b104_05
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

using joan::call::cdecl_call;

JOAN_COUNTER(c_tex_find, "TextureList::find");

using joan::orig::helpers::stricmp_;

struct H {
    // 0x004dff70 TextureList::find(name): 1-based index of the 0xd51-byte entry (name at +2).
    std::int32_t tex_find(const char* name) {
        c_tex_find.hit();
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::AnimDb>(this)->count); ++i) {
            const char* const entry = jo::as_tex_script_rec(jo::self<jo::AnimDb>(this)->records)[i].name;  // 0xd51-byte records, name at +2
            if (stricmp_(entry, name) == 0) return i + 1;
        }
        return 0;
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("TextureList::find", 0x004dff70, &H::tex_find);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::AnimDb::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_04::H::release>(this)); }  // 0x4dea50
auto joan::orig::AnimDb::touch_all() -> void { joan::bind::member<&::game_b97_04::H::touch_all>(this); }  // 0x4deb10
auto joan::orig::AnimDb::load_text_effects() -> void { joan::bind::member<&::game_b107_06::H::texteff_load>(this); }  // 0x4deb70
auto joan::orig::Formation::parse_4ded90(void* a0, void* a1) -> void { joan::bind::member<&::game_b106_17::H::parse_4ded90>(this, a0, a1); }  // 0x4ded90
auto joan::orig::AnimDb::parse_tex_op(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_05::H::texop_parse>(this, a0)); }  // 0x4df9f0
auto joan::orig::AnimDb::find(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves82b::H::tex_find>(this, a0)); }  // 0x4dff70
