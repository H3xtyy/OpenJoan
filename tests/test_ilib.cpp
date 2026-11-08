#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>

#include "core/binary_reader.hpp"
#include "core/file_util.hpp"
#include "core/vfs.hpp"
#include "ilib/database.hpp"
#include "ilib/game_set.hpp"
#include "ilib/kv_text.hpp"
#include "ilib/res_archive.hpp"
#include "test.hpp"

using namespace joan;
using namespace joan::ilib;

namespace {

std::vector<std::byte> bytes_of(std::string_view s) {
    std::vector<std::byte> v(s.size());
    std::memcpy(v.data(), s.data(), s.size());
    return v;
}

// Builds a dBase III image with the given character fields.
std::vector<std::byte> make_dbf(const std::vector<std::pair<std::string, int>>& fields,
                                const std::vector<std::string>& rows) {
    std::string h(32, '\0');
    h[0] = 0x03;
    int rec_size = 1;
    for (auto& f : fields) rec_size += f.second;
    const int hdr = 32 + 32 * static_cast<int>(fields.size()) + 1;
    auto put16 = [&](std::size_t at, int v) { h[at] = char(v & 0xff); h[at + 1] = char((v >> 8) & 0xff); };
    h[4] = char(rows.size());
    put16(8, hdr);
    put16(10, rec_size);
    for (auto& [name, len] : fields) {
        std::string d(32, '\0');
        std::memcpy(d.data(), name.data(), name.size());
        d[11] = 'C';
        d[16] = char(len);
        h += d;
    }
    h += '\x0d';
    for (auto& r : rows) h += " " + r;
    h += '\x1a';
    return bytes_of(h);
}

std::optional<std::filesystem::path> game_dir() {
    if (const char* p = std::getenv("JOAN_GAME_DIR"); p && looks_like_game_dir(p)) return std::filesystem::path(p);
    return std::nullopt;
}

}  // namespace

TEST("binary reader is bounds checked") {
    const auto d = bytes_of("\x01\x02\x03");
    BinaryReader r(d);
    CHECK(r.u16() == 0x0201);
    CHECK_THROWS(r.u16());
}

TEST("res archive short round trip") {
    std::vector<std::pair<std::string, std::vector<std::byte>>> files = {
        {"HEADER", bytes_of("abc")}, {"EMPTY", {}}, {"MODEL", bytes_of("hello world")}};
    const auto blob = ResArchive::build(ResFormat::ShortIndex, files);
    CHECK(blob.size() == 2 + 4 * 13 + 3 + 11);
    CHECK(ResArchive::detect(blob) == ResFormat::ShortIndex);
    const auto a = ResArchive::parse(blob);
    CHECK(a.size() == 3);
    CHECK(a.find("model") != nullptr);  // case-insensitive
    CHECK(as_string_view(*a.data("MODEL")) == "hello world");
    CHECK(a.data("EMPTY")->empty());
    CHECK(!a.data("MISSING"));
}

TEST("res archive long round trip") {
    std::vector<std::pair<std::string, std::vector<std::byte>>> files = {
        {"HUM_GEN_HOR_ATK_A_VERY_LONG_NAME", bytes_of("VERSION 1.0")}};
    const auto blob = ResArchive::build(ResFormat::LongIndex, files);
    CHECK(ResArchive::detect(blob) == ResFormat::LongIndex);
    const auto a = ResArchive::parse(blob);
    CHECK(a.entries()[0].name == "HUM_GEN_HOR_ATK_A_VERY_LONG_NAME");
    CHECK(as_string_view(a.data(a.entries()[0])) == "VERSION 1.0");
}

TEST("res archive rejects garbage") {
    CHECK(!ResArchive::detect(bytes_of("not an archive at all")));
    CHECK_THROWS(ResArchive::parse(bytes_of("\x05\x00garbage")));
}

TEST("dbf parses fields and records") {
    const auto img = make_dbf({{"CODE", 8}, {"COUNT", 4}}, {"ENGLISH   12", "FRENCH     7"});
    const auto db = Database::parse(img);
    CHECK(db.fields().size() == 2);
    CHECK(db.record_count() == 2);
    CHECK(db.record(0).str("CODE") == "ENGLISH");
    CHECK(db.record(1).num("count") == 7);
    CHECK(db.record(0).raw(std::size_t{0}) == "ENGLISH ");
    CHECK(db.to_csv() == "\"CODE\",\"COUNT\"\n\"ENGLISH\",\"12\"\n\"FRENCH\",\"7\"\n");
}

TEST("kv text parses blocks") {
    const auto kv = KvText::parse(
        "code : SCENARIO_01\r\nStepNo : 1\r\n\r\n# Day\r\nRayleigh\t: 0.68\r\nDescriptionFileName\t: S1L1_des\r\n");
    CHECK(kv.blocks().size() == 2);
    CHECK(kv.get("CODE") == "SCENARIO_01");
    CHECK(kv.blocks()[1].heading == "Day");
    CHECK(kv.get_number("rayleigh") == 0.68);
    CHECK(kv.get("descriptionfilename") == "S1L1_des");
}

TEST("vfs resolves paths case-insensitively") {
    const auto root = std::filesystem::temp_directory_path() / "joan_vfs_test";
    std::filesystem::remove_all(root);
    write_file(root / "Resource" / "STD.SET", "x", 1);
    write_file(root / "model" / "!alen_H.SMO", "y", 1);
    Vfs vfs(root);
    CHECK(vfs.exists("resource\\std.set"));
    CHECK(vfs.exists("RESOURCE/STD.SET"));
    CHECK(vfs.exists("Model\\!ALEN_h.smo"));
    CHECK(!vfs.exists("model/missing.smo"));
    std::filesystem::remove_all(root);
}

// ---------------------------------------------------------------------------
// Tests against the real game data (set JOAN_GAME_DIR to run them)
// ---------------------------------------------------------------------------

TEST("game: STD.SET opens and has expected tables") {
    auto dir = game_dir();
    if (!dir) SKIP("JOAN_GAME_DIR not set");
    Vfs vfs(*dir);
    const auto set = GameSet::open(vfs, "Resource/STD.SET");
    CHECK(set.code() == "STD");
    CHECK(set.has("NATION"));
    bool english = false;
    set.db("NATION").for_each([&](const DbRecord& r) { english |= r.str("NATIONCODE") == "ENGLISH"; });
    CHECK(english);
    // Every entry of every set is a valid dBase table.
    for (const char* path : {"Resource/STD.SET", "Resource/INTERFAC.SET", "Resource/RESOURCE"}) {
        const auto s = GameSet::open(vfs, path);
        for (const auto& e : s.archive().entries()) CHECK(s.db(e.name).record_size() > 0);
    }
}

TEST("game: every archive in Resource/ parses") {
    auto dir = game_dir();
    if (!dir) SKIP("JOAN_GAME_DIR not set");
    Vfs vfs(*dir);
    int archives = 0;
    for (const auto& entry : std::filesystem::directory_iterator(*vfs.resolve("Resource"))) {
        if (!entry.is_regular_file() || entry.file_size() == 0) continue;
        const auto data = read_file(entry.path());
        if (ResArchive::detect(data)) {
            const auto a = ResArchive::parse(data);
            CHECK(a.size() > 0);
            ++archives;
        }
    }
    CHECK(archives >= 5);
}

int main() { return jt::run_all(); }
