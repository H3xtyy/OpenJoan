// SPDX-License-Identifier: GPL-3.0-or-later
// CJK glyphs for the double-byte editions (cjk_glyphs.hpp, ADR-0004).
//
// A glyph is made in two steps: Font::glyph asks for it (measuring, layout) and gets its metrics at once - FreeType
// renders the character's coverage and a place in an atlas page is reserved; the first draw (texture_for, called with
// the font's own texture just loaded by the draw site) colours it like that font - fill and outline sampled from the
// font's capitals - and uploads the page.
#include "cjk_glyphs.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MULTIPLE_MASTERS_H
#include FT_SYNTHESIS_H

#include "../callconv.hpp"
#include "../d3d8/d3d8_types.hpp"
#include "../fn_names.hpp"
#include "../lib/crt/crt.hpp"
#include "../original/bind.hpp"
#include "../original/direct.hpp"
#include "../original/layout_access.hpp"
#include "../platform/dbcs.hpp"
#include "../registry.hpp"

namespace jo = joan::orig;
namespace d3d = joan::d3d8;

namespace joan::cjk {
namespace {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

constexpr int kPageSize = 1024;
constexpr u32 kFormatArgb = 0xb;  // Texture::create format of a 32-bit A8R8G8B8 texture (as the minimap's, minires.cpp)
constexpr d3d::DWORD kLockReadOnly = 0x10;  // D3DLOCK_READONLY

// ---------------------------------------------------------------- state

unsigned g_cp = 0;  // double-byte code page from CODEPAGE.RES, 0 = off

struct Page {
    void* texture = nullptr;  // a Texture2D of ours (made at the page's first draw), not in the texture manager's lists
    std::vector<u32> pixels = std::vector<u32>(static_cast<std::size_t>(kPageSize) * kPageSize, 0u);
    int x = 1, y = 1, row_h = 0;
    bool dirty = false;
};

// What we take from a game font: its baseline, the pixel size of a CJK character that matches its capitals, and (at
// the first draw) its colours.
struct Style {
    bool usable = false;
    int baseline = 0;  // rows from the top of a text line to the baseline
    int px = 0;        // FreeType pixel size
    bool bold = false;
    bool coloured = false;
    u32 fill = 0xffffffffu, edge = 0xff000000u;
    bool outline = false;
};

struct Entry {
    jo::FontGlyph rec{};  // first member: the pointer handed to the game is the record's
    Style* style = nullptr;
    int page = -1;
    int left = 0;           // column of the coverage inside the cell (1 pixel of padding all round for an outline)
    int cov_w = 0, cov_h = 0;
    std::vector<u8> coverage;  // FreeType's 8-bit coverage, until the first draw colours it
    bool painted = false;
};

FT_Library g_ft = nullptr;
FT_Face g_face = nullptr;
bool g_tried = false;
std::vector<std::unique_ptr<Page>> g_pages;
std::map<std::pair<const void*, u16>, std::unique_ptr<Entry>> g_glyphs;  // (Style, code)
std::unordered_set<const void*> g_records;
std::map<std::pair<const void*, std::string>, Style> g_styles;

u8 alpha(u32 c) { return static_cast<u8>(c >> 24); }
int luma(u32 c) { return static_cast<int>((((c >> 16) & 0xff) * 3 + ((c >> 8) & 0xff) * 6 + (c & 0xff)) / 10); }

const jo::FontGlyph* own_glyph(const jo::Font* f, u16 ch) {
    if (ch < f->first_char || ch > f->last_char) return nullptr;
    return reinterpret_cast<const jo::FontGlyph*>(reinterpret_cast<const u8*>(f->glyphs) + (ch - f->first_char) * 0xe);
}

// ---------------------------------------------------------------- FreeType

void set_weight(bool bold) {
    if (!(g_face->face_flags & FT_FACE_FLAG_MULTIPLE_MASTERS)) return;
    FT_MM_Var* mm = nullptr;
    if (FT_Get_MM_Var(g_face, &mm) != 0) return;
    std::vector<FT_Fixed> coords(mm->num_axis);
    for (FT_UInt i = 0; i < mm->num_axis; ++i) {
        coords[i] = mm->axis[i].def;
        if (mm->axis[i].tag == FT_MAKE_TAG('w', 'g', 'h', 't'))
            coords[i] = std::clamp(static_cast<FT_Fixed>(bold ? 700 : 400) * 65536, mm->axis[i].minimum, mm->axis[i].maximum);
    }
    FT_Set_Var_Design_Coordinates(g_face, mm->num_axis, coords.data());
    FT_Done_MM_Var(g_ft, mm);
}

bool open_font() {
    if (g_tried) return g_face != nullptr;
    g_tried = true;
    const std::string file = joan::platform::cjk_font_file(g_cp);
    if (file.empty()) {
        joan::hook::log_line("[Text] CJK: no CJK font found for code page %u (openjoan.ini [Platform] CjkFont=<path>)", g_cp);
        return false;
    }
    if (FT_Init_FreeType(&g_ft) != 0) return false;
    // A collection holds several faces: the one whose family names the code page's script (Noto Sans CJK SC ...).
    long faces = 1, pick = 0;
    if (FT_Face probe = nullptr; FT_New_Face(g_ft, file.c_str(), -1, &probe) == 0) {
        faces = probe->num_faces;
        FT_Done_Face(probe);
    }
    const std::string tag = std::string(" ") + joan::platform::cjk_face_tag(g_cp);
    for (long i = 0; i < faces; ++i) {
        FT_Face f = nullptr;
        if (FT_New_Face(g_ft, file.c_str(), i, &f) != 0) continue;
        const std::string family = f->family_name ? f->family_name : "";
        FT_Done_Face(f);
        if (family.size() >= tag.size() && family.compare(family.size() - tag.size(), tag.size(), tag) == 0) {
            pick = i;
            break;
        }
    }
    if (FT_New_Face(g_ft, file.c_str(), pick, &g_face) != 0) {
        joan::hook::log_line("[Text] CJK: cannot open %s", file.c_str());
        g_face = nullptr;
        return false;
    }
    FT_Select_Charmap(g_face, FT_ENCODING_UNICODE);
    joan::hook::log_line("[Text] CJK: code page %u, font %s (face %ld, %s)", g_cp, file.c_str(), pick,
                         g_face->family_name ? g_face->family_name : "?");
    return true;
}

// ---------------------------------------------------------------- the game font's look

Style& style_of(const jo::Font* f) {
    const char* const name = reinterpret_cast<const char*>(f) + 1;
    auto [it, added] = g_styles.try_emplace(std::make_pair(static_cast<const void*>(f), std::string(name, strnlen(name, 36))));
    Style& s = it->second;
    if (!added) return s;
    // The capital height of the font ('H', else a digit): Latin capitals fill about 0.72 of their em, so the em that
    // matches the font's capitals is cap / 0.72; CJK characters fill about 0.88 of theirs and look as tall as capitals.
    for (u16 ch : {u16{'H'}, u16{'E'}, u16{'0'}}) {
        const jo::FontGlyph* const g = own_glyph(f, ch);
        if (!g || g->width <= 0 || g->height <= 0) continue;
        s.baseline = g->top + g->height;
        s.px = std::clamp(static_cast<int>(std::lround(g->height / 0.72)), 8, 96);
        s.usable = true;
        break;
    }
    std::string lower = it->first.second;
    for (char& c : lower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    s.bold = lower.find("bold") != std::string::npos;
    return s;
}

// Colours of the font's own capitals in its texture: the inside of the strokes is the fill, their border the edge; an
// edge clearly darker or lighter than the fill is an outline (verdana_12_gold_black_outline ...).
void sample_colours(const jo::Font* f, Style& s, void* font_texture) {
    s.coloured = true;
    if (!font_texture) return;
    auto* const base = static_cast<d3d::IDirect3DBaseTexture8*>(jo::view<void*>(jo::as_texture(font_texture)->d3d_base));
    if (!base || base->GetType() != d3d::kRtTexture) return;
    auto* const tex = static_cast<d3d::IDirect3DTexture8*>(base);
    d3d::SurfaceDesc desc{};
    d3d::LockedRect lk{0, nullptr};
    if (tex->GetLevelDesc(0, &desc) < 0 || tex->LockRect(0, &lk, nullptr, kLockReadOnly) < 0 || !lk.bits) return;
    const u8* const bits = static_cast<const u8*>(lk.bits);
    auto px = [&](int x, int y) -> u32 {
        if (x < 0 || y < 0 || static_cast<u32>(x) >= desc.width || static_cast<u32>(y) >= desc.height) return 0;
        const u8* const row = bits + static_cast<std::ptrdiff_t>(y) * lk.pitch;
        u32 v = 0;
        switch (desc.format) {
            case 21: std::memcpy(&v, row + x * 4, 4); return v;                                  // A8R8G8B8
            case 22: std::memcpy(&v, row + x * 4, 4); return v | 0xff000000u;                    // X8R8G8B8
            case 26:                                                                             // A4R4G4B4
                std::memcpy(&v, row + x * 2, 2);
                return ((v >> 12) * 0x11u) << 24 | (((v >> 8) & 0xf) * 0x11u) << 16 | (((v >> 4) & 0xf) * 0x11u) << 8 | (v & 0xf) * 0x11u;
            case 25:                                                                             // A1R5G5B5
                std::memcpy(&v, row + x * 2, 2);
                return ((v & 0x8000) ? 0xff000000u : 0u) | (((v >> 10) & 0x1f) * 255 / 31) << 16 | (((v >> 5) & 0x1f) * 255 / 31) << 8 | (v & 0x1f) * 255 / 31;
            case 23:                                                                             // R5G6B5
                std::memcpy(&v, row + x * 2, 2);
                return 0xff000000u | ((v >> 11) * 255 / 31) << 16 | (((v >> 5) & 0x3f) * 255 / 63) << 8 | (v & 0x1f) * 255 / 31;
            default: return 0;
        }
    };
    u32 in_sum[3] = {}, in_n = 0, edge_sum[3] = {}, edge_n = 0;
    for (u16 ch : {u16{'H'}, u16{'O'}, u16{'M'}, u16{'B'}, u16{'E'}, u16{'W'}}) {
        const jo::FontGlyph* const g = own_glyph(f, ch);
        if (!g) continue;
        for (int y = g->y0; y <= g->y1; ++y)
            for (int x = g->x0; x <= g->x1; ++x) {
                const u32 c = px(x, y);
                if (alpha(c) < 0x80) continue;
                const bool inner = alpha(px(x - 1, y)) >= 0x80 && alpha(px(x + 1, y)) >= 0x80 && alpha(px(x, y - 1)) >= 0x80 &&
                                   alpha(px(x, y + 1)) >= 0x80;
                u32* const sum = inner ? in_sum : edge_sum;
                sum[0] += (c >> 16) & 0xff;
                sum[1] += (c >> 8) & 0xff;
                sum[2] += c & 0xff;
                ++(inner ? in_n : edge_n);
            }
    }
    tex->UnlockRect(0);
    auto avg = [](const u32* sum, u32 n) { return 0xff000000u | (sum[0] / n) << 16 | (sum[1] / n) << 8 | sum[2] / n; };
    if (in_n > 0) s.fill = avg(in_sum, in_n);
    if (edge_n > 0) s.edge = avg(edge_sum, edge_n);
    if (in_n == 0 && edge_n > 0) s.fill = s.edge;
    s.outline = in_n > 0 && edge_n > 0 && std::abs(luma(s.fill) - luma(s.edge)) > 90;
}

// ---------------------------------------------------------------- atlas

d3d::IDirect3DTexture8* d3d_of(const Page& p) {
    return static_cast<d3d::IDirect3DTexture8*>(jo::view<void*>(static_cast<jo::Texture2D*>(p.texture)->d3d_texture));
}

Page* page_with_room(int w, int h, int& page_index) {
    auto fits = [&](Page& p) {
        if (p.x + w + 1 > kPageSize) {
            p.x = 1;
            p.y += p.row_h + 1;
            p.row_h = 0;
        }
        return p.y + h + 1 <= kPageSize;
    };
    if (g_pages.empty() || !fits(*g_pages.back())) g_pages.push_back(std::make_unique<Page>());  // texture at the first draw
    page_index = static_cast<int>(g_pages.size()) - 1;
    return g_pages.back().get();
}

// The page's texture, made at its first draw: TextureManager::create_2d (0x006094c0) without adding it to the
// manager's list, the manager taken from the font texture the draw site just loaded.
bool make_texture(Page& p, void* font_texture) {
    if (p.texture) return true;
    if (!font_texture) return false;
    void* const mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::Texture2D)));
    if (!mem) return false;
    void* const t = joan::orig::as_texture2d(mem)->construct();  // 0x00605ce0
    jo::view<void*>(jo::as_texture(t)->manager) = jo::view<void*>(jo::as_texture(font_texture)->manager);
    joan::orig::as_texture(t)->create(static_cast<u32>(kPageSize), static_cast<u32>(kPageSize), 1u, kFormatArgb);  // 0x00605e20
    p.texture = t;
    return d3d_of(p) != nullptr;
}

void upload(Page& p) {
    d3d::IDirect3DTexture8* const tex = d3d_of(p);
    d3d::LockedRect lk{0, nullptr};
    if (tex->LockRect(0, &lk, nullptr, 0) < 0 || !lk.bits) return;
    for (int y = 0; y < kPageSize; ++y)
        std::memcpy(static_cast<u8*>(lk.bits) + static_cast<std::ptrdiff_t>(y) * lk.pitch, &p.pixels[static_cast<std::size_t>(y) * kPageSize], kPageSize * 4);
    tex->UnlockRect(0);
    p.dirty = false;
}

u32 over(u32 under, u32 colour, int a) {  // `colour` at coverage a (0..255) over `under`
    if (a <= 0) return under;
    const int ua = alpha(under);
    const int oa = a + ua * (255 - a) / 255;
    if (oa == 0) return 0;
    auto mix = [&](int sh) {
        const int c = static_cast<int>((colour >> sh) & 0xff), u = static_cast<int>((under >> sh) & 0xff);
        return static_cast<u32>((c * a + u * ua * (255 - a) / 255) / oa);
    };
    return static_cast<u32>(oa) << 24 | mix(16) << 16 | mix(8) << 8 | mix(0);
}

void paint(Entry& e) {
    const Style& s = *e.style;
    Page& p = *g_pages[static_cast<std::size_t>(e.page)];
    auto cov = [&](int x, int y) -> int {
        if (x < 0 || y < 0 || x >= e.cov_w || y >= e.cov_h) return 0;
        return e.coverage[static_cast<std::size_t>(y) * static_cast<std::size_t>(e.cov_w) + static_cast<std::size_t>(x)];
    };
    for (int y = 0; y < e.rec.height; ++y)
        for (int x = 0; x < e.rec.width; ++x) {
            const int bx = x - e.left, by = y - 1;
            u32 c = 0;
            if (s.outline) {
                int a = 0;
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) a = std::max(a, cov(bx + dx, by + dy));
                c = over(c, s.edge, a);
            }
            c = over(c, s.fill, cov(bx, by));
            p.pixels[static_cast<std::size_t>(e.rec.y0 + y) * kPageSize + static_cast<std::size_t>(e.rec.x0 + x)] = c;
        }
    e.coverage.clear();
    e.coverage.shrink_to_fit();
    e.painted = true;
    p.dirty = true;
}

Entry* rasterise(Style& s, u16 code) {
    if (!s.usable) return nullptr;
    const char32_t uc = joan::platform::dbcs_to_unicode(g_cp, code);
    if (uc == 0 || !open_font()) return nullptr;
    set_weight(s.bold);
    if (FT_Set_Pixel_Sizes(g_face, 0, static_cast<FT_UInt>(s.px)) != 0) return nullptr;
    const FT_UInt index = FT_Get_Char_Index(g_face, uc);
    if (index == 0 || FT_Load_Glyph(g_face, index, FT_LOAD_DEFAULT) != 0) return nullptr;
    if (s.bold && !(g_face->face_flags & FT_FACE_FLAG_MULTIPLE_MASTERS)) FT_GlyphSlot_Embolden(g_face->glyph);
    if (FT_Render_Glyph(g_face->glyph, FT_RENDER_MODE_NORMAL) != 0) return nullptr;
    const FT_Bitmap& bm = g_face->glyph->bitmap;
    auto e = std::make_unique<Entry>();
    e->style = &s;
    e->cov_w = static_cast<int>(bm.width);
    e->cov_h = static_cast<int>(bm.rows);
    e->coverage.resize(static_cast<std::size_t>(e->cov_w) * static_cast<std::size_t>(e->cov_h));
    for (int y = 0; y < e->cov_h; ++y)
        std::memcpy(&e->coverage[static_cast<std::size_t>(y) * static_cast<std::size_t>(e->cov_w)], bm.buffer + static_cast<std::ptrdiff_t>(y) * bm.pitch,
                    static_cast<std::size_t>(e->cov_w));
    // The cell: the advance wide (at least the coverage plus a pixel each side for an outline), the coverage plus a
    // pixel above and below high; the record's width is also the advance the game moves the pen by.
    e->left = std::max(1, static_cast<int>(g_face->glyph->bitmap_left));
    const int w = std::max(static_cast<int>((g_face->glyph->advance.x + 32) >> 6), e->left + e->cov_w + 1);
    const int h = e->cov_h + 2;
    int pi = -1;
    Page* const p = page_with_room(w, h, pi);
    if (!p) return nullptr;
    e->page = pi;
    e->rec.top = static_cast<std::int16_t>(s.baseline - g_face->glyph->bitmap_top - 1);
    e->rec.x0 = static_cast<std::int16_t>(p->x);
    e->rec.y0 = static_cast<std::int16_t>(p->y);
    e->rec.x1 = static_cast<std::int16_t>(p->x + w - 1);
    e->rec.y1 = static_cast<std::int16_t>(p->y + h - 1);
    e->rec.width = static_cast<std::int16_t>(w);
    e->rec.height = static_cast<std::int16_t>(h);
    p->x += w + 1;
    p->row_h = std::max(p->row_h, h);
    return e.release();
}

}  // namespace

void set_code_page(unsigned cp) {
    g_cp = joan::platform::dbcs_code_page(cp) ? cp : 0;
    if (g_cp) joan::hook::log_line("[Text] CJK: CODEPAGE.RES selects code page %u; characters the fonts lack are drawn from a system font", g_cp);
}

std::uint8_t* glyph(void* font, std::uint16_t code) {
    if (g_cp == 0 || code <= 0xff) return nullptr;
    Style& style = style_of(static_cast<const jo::Font*>(font));  // per font object and name: a re-initialised font gets new glyphs
    auto key = std::make_pair(static_cast<const void*>(&style), code);
    auto it = g_glyphs.find(key);
    if (it == g_glyphs.end()) {
        Entry* const e = rasterise(style, code);  // null: remembered as "no such character"
        it = g_glyphs.emplace(key, std::unique_ptr<Entry>(e)).first;
        if (e) g_records.insert(&e->rec);
    }
    return it->second ? reinterpret_cast<std::uint8_t*>(&it->second->rec) : nullptr;
}

void* texture_for(const void* glyph, void* font_texture) {
    if (g_records.empty() || !glyph || g_records.find(glyph) == g_records.end()) return font_texture;
    Entry& e = *const_cast<Entry*>(reinterpret_cast<const Entry*>(glyph));
    if (!e.painted) {
        if (!e.style->coloured) {
            // The font the style belongs to is the key's first half (Style objects live in g_styles, keyed by font).
            for (const auto& [k, s] : g_styles)
                if (&s == e.style) sample_colours(static_cast<const jo::Font*>(k.first), *e.style, font_texture);
        }
        paint(e);
    }
    Page& p = *g_pages[static_cast<std::size_t>(e.page)];
    if (!make_texture(p, font_texture)) return font_texture;
    if (p.dirty) upload(p);
    return p.texture;
}

}  // namespace joan::cjk
