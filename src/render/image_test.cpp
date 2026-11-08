// Test driver for image.cpp (not part of the hook DLL): decodes texture files, plans and builds the
// texture as D3DX would for a device described on the command line, prints formats / sizes /
// levels / checksums and optionally writes a dump in the format of the D3DX reference harness
// (32-bit freestanding runner of joan.exe's own D3DXCreateTextureFromFileInMemoryEx), so the two
// can be compared byte for byte.
//
//   c++ -std=c++20 -O2 -o image_test src/render/image_test.cpp src/render/image.cpp
//   image_test [key=value ...] file...      (out=DIR writes DIR/<basename>.d per file)
//   keys: pc=24|53 caps= maxw= maxh= aspect= w= h= mips= usage= fmt= pool= filter= mipfilter=
//         colorkey= fmts=gl|all|nodxt|no8888x out=DIR quiet=1
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "image.hpp"

using namespace joan::render;

namespace {

std::uint32_t crc32(const std::uint8_t* p, std::size_t n) {
    std::uint32_t c = 0xffffffffu;
    for (std::size_t i = 0; i < n; ++i) {
        c ^= p[i];
        for (int k = 0; k < 8; ++k) c = (c >> 1) ^ (0xedb88320u & (0u - (c & 1)));
    }
    return ~c;
}

std::uint32_t number(const char* s) {
    if (!std::strcmp(s, "-1")) return 0xffffffffu;
    return static_cast<std::uint32_t>(std::strtoul(s, nullptr, 0));
}

std::string fmt_name(std::uint32_t f) {
    switch (f) {
        case d3dfmt::kR8G8B8: return "R8G8B8";
        case d3dfmt::kA8R8G8B8: return "A8R8G8B8";
        case d3dfmt::kX8R8G8B8: return "X8R8G8B8";
        case d3dfmt::kR5G6B5: return "R5G6B5";
        case d3dfmt::kX1R5G5B5: return "X1R5G5B5";
        case d3dfmt::kA1R5G5B5: return "A1R5G5B5";
        case d3dfmt::kA4R4G4B4: return "A4R4G4B4";
        case d3dfmt::kP8: return "P8";
        case d3dfmt::kL8: return "L8";
        case d3dfmt::kA8L8: return "A8L8";
        case d3dfmt::kDXT1: return "DXT1";
        case d3dfmt::kDXT2: return "DXT2";
        case d3dfmt::kDXT3: return "DXT3";
        case d3dfmt::kDXT4: return "DXT4";
        case d3dfmt::kDXT5: return "DXT5";
        default: return std::to_string(f);
    }
}

bool format_ok(const std::string& set, std::uint32_t f) {
    static const std::uint32_t gl[] = {21, 22, 23, 24, 25, 26, 28, 30, 50, 51, 60, d3dfmt::kDXT1, d3dfmt::kDXT2,
                                       d3dfmt::kDXT3, d3dfmt::kDXT4, d3dfmt::kDXT5};
    static const std::uint32_t all[] = {20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 40, 41, 50, 51, 52, 60, 61,
                                        62, 63, 64, 65, d3dfmt::kDXT1, d3dfmt::kDXT2, d3dfmt::kDXT3, d3dfmt::kDXT4,
                                        d3dfmt::kDXT5};
    if (set == "nodxt" && d3dfmt::is_dxt(f)) return false;
    if (set == "no8888x" && f == 22) return false;
    if (set == "all") {
        for (std::uint32_t x : all)
            if (x == f) return true;
        return false;
    }
    for (std::uint32_t x : gl)
        if (x == f) return true;
    return false;
}

}  // namespace

int main(int argc, char** argv) {
    LoadArgs args;
    args.mip_levels = 0;
    DeviceQuery dev;
    dev.texture_caps = 0x4000;
    dev.max_texture_width = dev.max_texture_height = 4096;
    std::string fmts = "gl", out_dir;
    Precision precision = Precision::Single;
    bool quiet = false;
    std::vector<std::string> files;
    for (int i = 1; i < argc; ++i) {
        const char* a = argv[i];
        const char* eq = std::strchr(a, '=');
        if (!eq) {
            files.emplace_back(a);
            continue;
        }
        const std::string k(a, eq);
        const char* v = eq + 1;
        if (k == "pc") precision = number(v) == 53 ? Precision::Double : Precision::Single;
        else if (k == "caps") dev.texture_caps = number(v);
        else if (k == "maxw") dev.max_texture_width = number(v);
        else if (k == "maxh") dev.max_texture_height = number(v);
        else if (k == "aspect") dev.max_texture_aspect_ratio = number(v);
        else if (k == "w") args.width = number(v);
        else if (k == "h") args.height = number(v);
        else if (k == "mips") args.mip_levels = number(v);
        else if (k == "usage") args.usage = number(v);
        else if (k == "fmt") args.format = number(v);
        else if (k == "pool") args.pool = number(v);
        else if (k == "filter") args.filter = number(v);
        else if (k == "mipfilter") args.mip_filter = number(v);
        else if (k == "colorkey") args.color_key = number(v);
        else if (k == "fmts") fmts = v;
        else if (k == "out") out_dir = v;
        else if (k == "quiet") quiet = number(v) != 0;
        else {
            std::fprintf(stderr, "unknown option %s\n", a);
            return 2;
        }
    }
    dev.texture_format_ok = [&](std::uint32_t f) { return format_ok(fmts, f); };
    int failures = 0;
    for (const std::string& path : files) {
        std::ifstream in(path, std::ios::binary);
        const std::vector<std::uint8_t> data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        const std::string base = path.substr(path.find_last_of('/') + 1);
        Image img;
        std::string why;
        TexturePlan plan;
        std::vector<ImageLevel> levels;
        const bool decoded = !data.empty() && decode_image(data.data(), data.size(), img, &why);
        bool built = false;
        if (decoded) {
            std::uint32_t src_crc = 0;
            for (const ImageLevel& l : img.levels) src_crc ^= crc32(l.data.data(), l.data.size());
            if (!quiet)
                std::printf("%s: %s %ux%u, %zu level(s)%s, crc %08x\n", base.c_str(), fmt_name(img.format).c_str(),
                            img.width(), img.height(), img.levels.size(), img.has_palette ? ", palette" : "", src_crc);
            built = plan_texture(img, args, dev, plan, &why) && build_texture(img, plan, precision, levels, &why);
        }
        if (!built) {
            std::printf("%s: FALLBACK (%s)\n", base.c_str(), why.c_str());
            ++failures;
        } else if (!quiet) {
            std::printf("  -> %s %ux%u, %u level(s):", fmt_name(plan.format).c_str(), plan.width, plan.height,
                        plan.levels);
            for (const ImageLevel& l : levels) std::printf(" %08x", crc32(l.data.data(), l.data.size()));
            std::printf("\n");
        }
        if (!out_dir.empty()) {
            std::ofstream o(out_dir + "/" + base + ".d", std::ios::binary);
            const std::uint32_t hdr[6] = {0x31445854u, built ? 0u : 0xffffffffu, built ? plan.format : 0u,
                                          built ? plan.width : 0u, built ? plan.height : 0u, built ? plan.levels : 0u};
            o.write(reinterpret_cast<const char*>(hdr), sizeof hdr);
            if (built)
                for (const ImageLevel& l : levels) {
                    const std::uint32_t lh[4] = {l.width, l.height, l.pitch, row_count(plan.format, l.height)};
                    o.write(reinterpret_cast<const char*>(lh), sizeof lh);
                    o.write(reinterpret_cast<const char*>(l.data.data()), static_cast<std::streamsize>(l.data.size()));
                }
        }
    }
    return failures ? 1 : 0;
}
