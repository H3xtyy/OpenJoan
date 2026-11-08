#include "en3d/dds.hpp"

#include <algorithm>
#include <bit>

#include "core/binary_reader.hpp"

namespace joan::en3d {

namespace {

constexpr std::uint32_t fourcc(char a, char b, char c, char d) {
    return static_cast<std::uint32_t>(static_cast<unsigned char>(a)) |
           static_cast<std::uint32_t>(static_cast<unsigned char>(b)) << 8 |
           static_cast<std::uint32_t>(static_cast<unsigned char>(c)) << 16 |
           static_cast<std::uint32_t>(static_cast<unsigned char>(d)) << 24;
}

constexpr std::uint32_t kDDPF_ALPHAPIXELS = 0x1;
constexpr std::uint32_t kDDPF_FOURCC = 0x4;
constexpr std::uint32_t kDDPF_RGB = 0x40;

struct Rgba {
    std::uint8_t r, g, b, a;
};

Rgba unpack565(std::uint16_t c) {
    const auto r = static_cast<std::uint8_t>(((c >> 11) & 31) * 255 / 31);
    const auto g = static_cast<std::uint8_t>(((c >> 5) & 63) * 255 / 63);
    const auto b = static_cast<std::uint8_t>((c & 31) * 255 / 31);
    return {r, g, b, 255};
}

std::uint8_t mix(int a, int b, int wa, int wb, int div) { return static_cast<std::uint8_t>((a * wa + b * wb) / div); }

// Decodes the colour part of a DXT block into 16 texels. DXT1 blocks with c0 <= c1
// use 3-colour + transparent mode; DXT3/5 always use 4-colour mode.
void decode_color_block(const std::uint8_t* blk, Rgba out[16], bool dxt1) {
    const auto c0 = static_cast<std::uint16_t>(blk[0] | blk[1] << 8);
    const auto c1 = static_cast<std::uint16_t>(blk[2] | blk[3] << 8);
    Rgba pal[4];
    pal[0] = unpack565(c0);
    pal[1] = unpack565(c1);
    if (c0 > c1 || !dxt1) {
        pal[2] = {mix(pal[0].r, pal[1].r, 2, 1, 3), mix(pal[0].g, pal[1].g, 2, 1, 3), mix(pal[0].b, pal[1].b, 2, 1, 3), 255};
        pal[3] = {mix(pal[0].r, pal[1].r, 1, 2, 3), mix(pal[0].g, pal[1].g, 1, 2, 3), mix(pal[0].b, pal[1].b, 1, 2, 3), 255};
    } else {
        pal[2] = {mix(pal[0].r, pal[1].r, 1, 1, 2), mix(pal[0].g, pal[1].g, 1, 1, 2), mix(pal[0].b, pal[1].b, 1, 1, 2), 255};
        pal[3] = {0, 0, 0, 0};
    }
    const std::uint32_t bits = static_cast<std::uint32_t>(blk[4]) | static_cast<std::uint32_t>(blk[5]) << 8 |
                               static_cast<std::uint32_t>(blk[6]) << 16 | static_cast<std::uint32_t>(blk[7]) << 24;
    for (int i = 0; i < 16; ++i) out[i] = pal[(bits >> (2 * i)) & 3];
}

void decode_dxt3_alpha(const std::uint8_t* blk, Rgba out[16]) {
    for (int i = 0; i < 16; ++i) {
        const int nibble = (blk[i / 2] >> ((i & 1) * 4)) & 15;
        out[i].a = static_cast<std::uint8_t>(nibble * 17);
    }
}

void decode_dxt5_alpha(const std::uint8_t* blk, Rgba out[16]) {
    int a[8];
    a[0] = blk[0];
    a[1] = blk[1];
    if (a[0] > a[1]) {
        for (int i = 1; i < 7; ++i) a[i + 1] = ((7 - i) * a[0] + i * a[1]) / 7;
    } else {
        for (int i = 1; i < 5; ++i) a[i + 1] = ((5 - i) * a[0] + i * a[1]) / 5;
        a[6] = 0;
        a[7] = 255;
    }
    std::uint64_t bits = 0;
    for (int i = 0; i < 6; ++i) bits |= static_cast<std::uint64_t>(blk[2 + i]) << (8 * i);
    for (int i = 0; i < 16; ++i) out[i].a = static_cast<std::uint8_t>(a[(bits >> (3 * i)) & 7]);
}

int shift_of(std::uint32_t mask) { return mask ? std::countr_zero(mask) : 0; }

std::uint8_t extract(std::uint32_t px, std::uint32_t mask) {
    if (!mask) return 255;
    const std::uint32_t v = (px & mask) >> shift_of(mask);
    const std::uint32_t max = mask >> shift_of(mask);
    return static_cast<std::uint8_t>(v * 255 / max);
}

}  // namespace

Image load_dds(std::span<const std::byte> data) {
    BinaryReader r(data);
    if (r.u32() != fourcc('D', 'D', 'S', ' ')) throw FormatError("not a DDS file");
    if (r.u32() != 124) throw FormatError("bad DDS header size");
    r.u32();  // flags
    Image img;
    img.height = r.u32();
    img.width = r.u32();
    r.u32();  // pitch / linear size
    r.u32();  // depth
    img.mip_count = std::max<std::uint32_t>(1, r.u32());
    r.skip(11 * 4);
    // DDS_PIXELFORMAT
    r.u32();  // size (32)
    const std::uint32_t pf_flags = r.u32();
    const std::uint32_t pf_fourcc = r.u32();
    const std::uint32_t bit_count = r.u32();
    const std::uint32_t rmask = r.u32(), gmask = r.u32(), bmask = r.u32(), amask = r.u32();
    r.seek(128);

    if (img.width == 0 || img.height == 0 || img.width > 16384 || img.height > 16384)
        throw FormatError("bad DDS dimensions");
    const std::uint32_t w = img.width, h = img.height;
    img.rgba.assign(static_cast<std::size_t>(w) * h * 4, 0);

    if (pf_flags & kDDPF_FOURCC) {
        int block_bytes = 0;
        if (pf_fourcc == fourcc('D', 'X', 'T', '1')) block_bytes = 8, img.format = "DXT1";
        else if (pf_fourcc == fourcc('D', 'X', 'T', '3')) block_bytes = 16, img.format = "DXT3";
        else if (pf_fourcc == fourcc('D', 'X', 'T', '5')) block_bytes = 16, img.format = "DXT5";
        else throw FormatError("unsupported DDS FourCC");
        const std::uint32_t bw = (w + 3) / 4, bh = (h + 3) / 4;
        const auto blocks = r.bytes(static_cast<std::size_t>(bw) * bh * static_cast<std::size_t>(block_bytes));
        const auto* p = reinterpret_cast<const std::uint8_t*>(blocks.data());
        Rgba texels[16];
        for (std::uint32_t by = 0; by < bh; ++by)
            for (std::uint32_t bx = 0; bx < bw; ++bx, p += block_bytes) {
                if (img.format == "DXT1") {
                    decode_color_block(p, texels, true);
                } else {
                    decode_color_block(p + 8, texels, false);
                    if (img.format == "DXT3") decode_dxt3_alpha(p, texels);
                    else decode_dxt5_alpha(p, texels);
                }
                for (std::uint32_t y = 0; y < 4; ++y)
                    for (std::uint32_t x = 0; x < 4; ++x) {
                        const std::uint32_t px = bx * 4 + x, py = by * 4 + y;
                        if (px >= w || py >= h) continue;
                        const Rgba& t = texels[y * 4 + x];
                        auto* d = &img.rgba[(static_cast<std::size_t>(py) * w + px) * 4];
                        d[0] = t.r, d[1] = t.g, d[2] = t.b, d[3] = t.a;
                    }
            }
        return img;
    }

    if (!(pf_flags & kDDPF_RGB) || (bit_count != 16 && bit_count != 24 && bit_count != 32))
        throw FormatError("unsupported DDS pixel format");
    img.format = bit_count == 32 ? (amask && (pf_flags & kDDPF_ALPHAPIXELS) ? "A8R8G8B8" : "X8R8G8B8")
                                 : (bit_count == 24 ? "R8G8B8" : "16-bit RGB");
    const std::uint32_t bpp = bit_count / 8;
    const auto pixels = r.bytes(static_cast<std::size_t>(w) * h * bpp);
    const auto* p = reinterpret_cast<const std::uint8_t*>(pixels.data());
    const std::uint32_t used_amask = (pf_flags & kDDPF_ALPHAPIXELS) ? amask : 0;
    for (std::size_t i = 0; i < static_cast<std::size_t>(w) * h; ++i, p += bpp) {
        std::uint32_t px = 0;
        for (std::uint32_t b = 0; b < bpp; ++b) px |= static_cast<std::uint32_t>(p[b]) << (8 * b);
        auto* d = &img.rgba[i * 4];
        d[0] = extract(px, rmask), d[1] = extract(px, gmask), d[2] = extract(px, bmask), d[3] = extract(px, used_amask);
    }
    return img;
}

}  // namespace joan::en3d
