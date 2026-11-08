// en3d objects that game code calls members of directly (refactor R7c): the texture manager (its
// load / release / mode), Material::set_texture, Bone::matrix and Billboard::item. Only the fields these members
// touch are laid out; the rest of each object is opaque here.
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

namespace joan::orig {

#pragma pack(push, 1)

struct TexturePtr;

/// The texture manager (0xa403a0 and the scene's texture banks): four count/capacity/array lists of textures, the load
/// mode and the requeue settings. Only the members its callers touch are laid out.
struct TextureManager {
    std::int32_t named_count;  ///< 0x0
    std::int32_t named_cap;  ///< 0x4
    TexturePtr* named;  ///< 0x8
    std::int32_t anon_count;  ///< 0xc
    std::int32_t anon_cap;  ///< 0x10
    TexturePtr* anon;  ///< 0x14
    std::int32_t released_count;  ///< 0x18
    std::int32_t released_cap;  ///< 0x1c
    TexturePtr* released;  ///< 0x20
    std::int32_t loading_count;  ///< 0x24
    std::int32_t loading_cap;  ///< 0x28
    TexturePtr* loading;  ///< 0x2c
    std::uint32_t mode_;  ///< 0x30 D3DX mip / load mode (0 = plain D3DXCreate*FromFile)
    std::uint32_t requeue;  ///< 0x34 nonzero: a texture at its last reference is requeued (0x0060b1b0) first
    std::uint32_t stamp;  ///< 0x38 stored into the requeued texture's +0x10

    /// 0x006099c0 TextureManager::load(name, kind, deferred) (en3d/texturea.cpp).
    void* load(const char* texture_name, std::int32_t kind, std::uint32_t deferred);
    /// 0x0060a550 TextureManager::release(texture) (en3d/texturea.cpp).
    std::int32_t release(void* texture);
    /// 0x0060a720 Textures::mode() (+0x30).
    [[nodiscard]] std::uint32_t mode() const noexcept { return mode_; }
    /// 0x0060a740 Textures::set_mode(v) (+0x30).
    TextureManager* set_mode(std::uint32_t v) noexcept {  // returns this (the original leaves it in eax)
        mode_ = v;
        return this;
    }

    // The other TextureManager functions the game code calls (typed, through their slots).
#include "gen/texture_manager_methods.inc"
};

/// Material, 0x54 bytes: 4 texture stages at +0x24.
struct Material {
    std::uint32_t diffuse;  ///< 0x0
    std::uint32_t ambient;  ///< 0x4
    std::uint32_t specular;  ///< 0x8
    std::uint32_t emissive;  ///< 0xc
    std::uint32_t power;  ///< 0x10
    std::uint32_t u32_14;  ///< 0x14
    std::uint32_t u32_18;  ///< 0x18
    std::uint32_t colour;  ///< 0x1c
    std::uint32_t raster;  ///< 0x20
    void* textures[4];  ///< 0x24
    // --- members from re/layout/layouts.json (gen_types.py) ---
    std::uint32_t texture_mode;  ///< 0x34
    std::uint32_t blend;  ///< 0x38
    std::uint32_t filter;  ///< 0x3c
    std::uint32_t pass_mask;  ///< 0x40
    std::uint32_t runtime[4];  ///< 0x44

    /// 0x005d5650 Material::set_texture(stage, texture) (en3d/plane.cpp).
    void set_texture(std::int32_t stage, void* texture);

    // The other Material functions the game code calls (typed, through their slots).
#include "gen/material_methods.inc"
};

/// Bone: world matrix at +0x58.
struct Bone {
    float local[16];  ///< 0x0
    Bone* parent;  ///< 0x40
    Bone* prev;  ///< 0x44
    Bone* next;  ///< 0x48
    Bone* first_child;  ///< 0x4c
    Bone* last_child;  ///< 0x50
    std::uint32_t dirty;  ///< 0x54
    std::uint8_t matrix_[0x40];  ///< 0x58
    // --- members from re/layout/layouts.json (gen_types.py) ---
    void* callback;  ///< 0x98
    void* owner;  ///< 0x9c

    /// 0x0060ef80 Bone::matrix(): updates the bone (0x0060ee60) and returns its matrix.
    std::uint8_t* matrix();

    // The other Bone functions the game code calls (typed, through their slots).
#include "gen/bone_methods.inc"
};

struct Plane;

/// Billboard: the item slot array at +0x24. Packed: most members sit at odd offsets in the original.
struct Billboard {
    void* vtable;  ///< 0x0
    std::uint8_t owned;  ///< 0x4
    Plane* plane_;  ///< 0x5
    float pos[3];  ///< 0x9
    std::uint32_t u32_15;  ///< 0x15
    std::uint32_t u32_19;  ///< 0x19
    std::int16_t i16_1d;  ///< 0x1d
    float offset;  ///< 0x1f
    std::uint8_t gap_23[0x1];  ///< bytes not used by this view
    std::uint32_t* items;  ///< 0x24
    // --- members from re/layout/layouts.json (gen_types.py) ---
    std::uint8_t gap_28[0x7];  ///< bytes not used by this view
    float width;  ///< 0x2f
    float height;  ///< 0x33

    /// 0x00637da0 BlockedArray::get(index): 0x00637920 of items[index - 1].
    void* item(std::int32_t index);

    // The other Billboard functions the game code calls (typed, through their slots).
#include "gen/billboard_methods.inc"
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(TextureManager, mode_) == 0x30);
static_assert(offsetof(TextureManager, requeue) == 0x34);
static_assert(offsetof(TextureManager, stamp) == 0x38);
static_assert(offsetof(Material, textures) == 0x24);
static_assert(offsetof(Bone, matrix_) == 0x58);
static_assert(offsetof(Billboard, items) == 0x24);
#endif

}  // namespace joan::orig
