// jdbf - inspect the dBase tables inside game sets.
//
//   jdbf <game-dir> <set-path> [table]
//     without [table]: lists tables with their record counts and fields
//     with    [table]: prints the table as CSV
//   e.g. jdbf "C:/Games/Joan of Arc" Resource/STD.SET NATION
#include <cstdio>
#include <iostream>

#include "core/vfs.hpp"
#include "ilib/game_set.hpp"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: jdbf <game-dir> <set-path> [table]\n";
        return 2;
    }
    try {
        const joan::Vfs vfs(argv[1]);
        const auto set = joan::ilib::GameSet::open(vfs, argv[2]);
        if (argc >= 4) {
            std::fputs(set.db(argv[3]).to_csv().c_str(), stdout);
            return 0;
        }
        std::printf("set %s: %s\n", set.code().c_str(), set.description().c_str());
        for (const auto& e : set.archive().entries()) {
            const auto& db = set.db(e.name);
            std::printf("%-10s %5zu records |", e.name.c_str(), db.record_count());
            for (const auto& f : db.fields()) std::printf(" %s:%c%u", f.name.c_str(), f.type, unsigned(f.length));
            std::printf("\n");
        }
    } catch (const std::exception& e) {
        std::cerr << "jdbf: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
