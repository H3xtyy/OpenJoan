// jres - list, extract and pack Joan of Arc resource archives (.RES/.SET).
//
//   jres list    <archive>
//   jres extract <archive> <out-dir>          (dBase tables also get a .csv)
//   jres pack    <short|long> <out-archive> <file>...
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "core/file_util.hpp"
#include "ilib/database.hpp"
#include "ilib/res_archive.hpp"

using namespace joan;
using namespace joan::ilib;

namespace {

int usage() {
    std::cerr << "usage:\n"
                 "  jres list    <archive>\n"
                 "  jres extract <archive> <out-dir>\n"
                 "  jres pack    <short|long> <out-archive> <file>...\n";
    return 2;
}

std::string guess_extension(std::span<const std::byte> d) {
    auto starts = [&](std::string_view magic) {
        return d.size() >= magic.size() && std::equal(magic.begin(), magic.end(), reinterpret_cast<const char*>(d.data()));
    };
    if (starts("RIFF")) return ".wav";
    if (starts("DDS ")) return ".dds";
    if (starts("\x89PNG")) return ".png";
    if (starts("\xFF\xD8")) return ".jpg";
    if (!d.empty() && d[0] == std::byte{0x03}) {
        try {
            (void)Database::parse(d);
            return ".dbf";
        } catch (const std::exception&) {
        }
    }
    return ".bin";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 3) return usage();
    const std::string cmd = argv[1];
    try {
        if (cmd == "list") {
            const auto a = ResArchive::parse(read_file(argv[2]));
            std::printf("format: %s, %zu entries\n", a.format() == ResFormat::ShortIndex ? "short" : "long", a.size());
            for (const auto& e : a.entries())
                std::printf("%-34s %10u %10u  %s\n", e.name.c_str(), e.offset, e.size, guess_extension(a.data(e)).c_str());
            return 0;
        }
        if (cmd == "extract" && argc == 4) {
            const auto a = ResArchive::parse(read_file(argv[2]));
            const std::filesystem::path out = argv[3];
            for (const auto& e : a.entries()) {
                const auto bytes = a.data(e);
                const std::string ext = guess_extension(bytes);
                write_file(out / (e.name + ext), bytes.data(), bytes.size());
                if (ext == ".dbf") {
                    const std::string csv = Database::parse(bytes).to_csv();
                    write_file(out / (e.name + ".csv"), csv.data(), csv.size());
                }
            }
            std::printf("extracted %zu entries to %s\n", a.size(), out.string().c_str());
            return 0;
        }
        if (cmd == "pack" && argc >= 4) {
            const std::string kind = argv[2];
            if (kind != "short" && kind != "long") return usage();
            std::vector<std::pair<std::string, std::vector<std::byte>>> files;
            for (int i = 4; i < argc; ++i) {
                const std::filesystem::path p = argv[i];
                files.emplace_back(p.stem().string(), read_file(p));
            }
            const auto blob = ResArchive::build(kind == "short" ? ResFormat::ShortIndex : ResFormat::LongIndex, files);
            write_file(argv[3], blob.data(), blob.size());
            std::printf("packed %zu entries into %s\n", files.size(), argv[3]);
            return 0;
        }
    } catch (const std::exception& e) {
        std::cerr << "jres: " << e.what() << '\n';
        return 1;
    }
    return usage();
}
