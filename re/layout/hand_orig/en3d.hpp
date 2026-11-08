// en3d objects that game code calls members of directly (refactor R7c, docs/RUNTIME.md): the texture manager (its
// load / release / mode), Material::set_texture, Bone::matrix and Billboard::item. Only the fields these members
// touch are laid out; the rest of each object is opaque here.
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

namespace joan::orig {

#pragma pack(push, 1)

/// The texture manager (0xa403a0 and the scene's texture banks).
struct TextureManager {
    std::uint8_t head[0x30];
    std::uint32_t mode_;  // 0x30 D3DX mip / load mode (0 = plain D3DXCreate*FromFile)
    std::uint32_t requeue;  // 0x34 nonzero: a texture at its last reference is requeued (0x0060b1b0) first
    std::uint32_t stamp;  // 0x38 stored into the requeued texture's +0x10

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
    std::uint8_t head[0x24];
    void* textures[4];  // 0x24

    /// 0x005d5650 Material::set_texture(stage, texture) (en3d/plane.cpp).
    void set_texture(std::int32_t stage, void* texture);

    // The other Material functions the game code calls (typed, through their slots).
#include "gen/material_methods.inc"
};

/// Bone: world matrix at +0x58.
struct Bone {
    std::uint8_t head[0x58];
    std::uint8_t matrix_[0x40];  // 0x58

    /// 0x0060ef80 Bone::matrix(): updates the bone (0x0060ee60) and returns its matrix (game/_unplaced/after_en3d_texturea.cpp).
    std::uint8_t* matrix();

    // The other Bone functions the game code calls (typed, through their slots).
#include "gen/bone_methods.inc"
};

/// Billboard: the item slot array at +0x24.
struct Billboard {
    std::uint8_t head[0x24];
    std::uint32_t* items;  // 0x24

    /// 0x00637da0 Billboard::item(index): 0x00637920 of items[index - 1] (game/_unplaced/after_uti3d_blkarrb.cpp).
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
