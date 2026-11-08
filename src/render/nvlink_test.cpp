// Test driver for nvlink.cpp (not part of the hook DLL): replays a harness transcript - the output
// of re/nvlink/harness (the real nvlinker.dll) or of `gen_n3.py model` (re/nvlink/nvlink.py) for
// a script from `gen_n3.py script` - with the C++ linker and compares every fragment id, link
// result (HRESULT and all tokens), constant id and GetConstantSlot answer.
//
//   c++ -std=c++20 -O2 -o nvlink_test src/render/nvlink_test.cpp src/render/nvlink.cpp
//   nvlink_test SHADER_DIR transcript.txt [transcript2.txt ...]
//   nvlink_test --fixed SHADER_DIR script.txt     ([Fixes] Shaders=1 transcript, see dump_fixed;
//   nvlink_test --exact SHADER_DIR script.txt      --exact: the same without the fixes)
//
// Transcript lines used (others are ignored):
//   CASE <n> opt=<0|1> reserve=<lo>,<hi>|none      (echo from the script; starts a case)
//   LINK name=id ...
//   CreateShader hr=<hex> handle=<hex>
//   TOKENS <hex> ...
//   SLOT <name> <instance> id=<hex>(hr <hex>) reg=<int>(hr <hex>)
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "nvlink.hpp"

using namespace joan::render::nvlink;

namespace {

const char* const kFiles[] = {"default_shader.nvo", "transform.nvo", "lighting.nvo", "fog.nvo",
                              "scatter.nvo", "hardware_skin.nvo", "clip_plane.nvo"};

std::unique_ptr<Linker> load(const std::string& dir, bool optimize) {
    auto l = std::make_unique<Linker>(optimize);
    for (const char* f : kFiles) {
        std::ifstream in(dir + "/" + f, std::ios::binary);
        if (!in) {
            std::fprintf(stderr, "cannot open %s/%s\n", dir.c_str(), f);
            std::exit(2);
        }
        const std::vector<char> data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        if (l->add_fragments(data.data(), data.size()) != kOk) {
            std::fprintf(stderr, "AddFragments %s failed\n", f);
            std::exit(2);
        }
    }
    return l;
}

int failures = 0;
void fail(int line, const std::string& what) {
    if (++failures <= 20) std::printf("  line %d: %s\n", line, what.c_str());
}

std::uint32_t hex(const std::string& s) { return static_cast<std::uint32_t>(std::strtoul(s.c_str(), nullptr, 16)); }

/// Fixed checks of the API edges, against what the DLL was observed to answer (harness).
void api_checks(Linker& l) {
    std::uint32_t id = 0xdead;
    const auto expect = [](bool ok, const char* what) {
        if (!ok) fail(0, what);
    };
    expect(l.constant_id("c_point_light_position", &id) == kOk && id == 0x87, "c_point_light_position id 0x87");
    expect(l.constant_id("c_fog_constant", &id) == kOk && id == 0x9c, "c_fog_constant id 0x9c");
    expect(l.constant_id("c_clip_plane", &id) == kOk && id == 0xac, "c_clip_plane id 0xac");
    expect(l.constant_id("r_temp", &id) == kFail, "GetConstantID(r_temp) fails");
    expect(l.constant_id("nonexistent", &id) == kFail, "GetConstantID(unknown) fails");
    expect(l.constant_id(nullptr, &id) == kInvalidArg, "GetConstantID(null)");
    expect(l.fragment_id("clip_plane", &id) == kOk && id == 47, "clip_plane id 47");
    expect(l.fragment_id("bogus", &id) == kFail, "GetFragmentID(unknown) fails");
    expect(l.reserve_constants(5, 4) == kInvalidArg, "ReserveConstantRange(last < first)");
    const std::uint32_t bad[] = {0x12345678, 0x14, 0, 0xfffe0100, 5, 0xffff};
    expect(l.add_fragments(bad, sizeof bad) == kInvalidArg, "AddFragments(bad magic)");
    const std::uint32_t none[] = {99, 0};
    expect(l.link(none).hr == kInvalidArg, "link(unknown id)");

    // Listing syntax: every loaded instruction survives disassemble -> assemble.
    for (const Fragment& f : l.fragments())
        for (const Instruction& ins : f.code) {
            bool relative = false;  // c[a0.x + n]: listing only, not assembled
            for (int k = 1; k < ins.count; ++k)
                if (((ins.operands[k].token >> 28) & 7) == 2 && (ins.operands[k].token & (1u << 13))) relative = true;
            if (relative) continue;
            Instruction back;
            std::string err;
            const std::string text = l.disassemble(ins);
            bool same = l.assemble(text, back, &err) && back.opcode == ins.opcode && back.count == ins.count;
            for (int k = 0; same && k < ins.count; ++k) {
                const Operand &a = ins.operands[k], &b = back.operands[k];
                same = a.symbol == b.symbol && a.token == b.token &&
                       (a.symbol >= 0 ? (a.extra & 0x10000) == (b.extra & 0x10000) : a.extra == b.extra);
            }
            if (!same) fail(0, "assemble(disassemble) differs: " + f.name + ": " + text + " " + err);
        }
    // Every shader fix found its instructions in the game's fragments.
    for (const std::string& line : l.fix_report())
        if (line.rfind("applied", 0) != 0) fail(0, line);
}

/// Components an operand (k >= 1) really reads (D3D semantics, unlike the linker's liveness mask).
std::uint32_t components_read(const Instruction& ins, int k) {
    const std::uint32_t sw = (ins.operands[k].token >> 16) & 0xff;
    const auto comp = [&](int c) { return 1u << ((sw >> (2 * c)) & 3); };
    const auto first = [&](int n) {
        std::uint32_t m = 0;
        for (int c = 0; c < n; ++c) m |= comp(c);
        return m;
    };
    switch (ins.opcode) {
        case 8: return first(3);                              // dp3
        case 9: case 20: case 21: return k == 1 ? first(4) : 0xf;  // dp4 m4x4 m4x3 (rows: constants)
        case 22: case 23: case 24: return k == 1 ? first(3) : 0xf; // m3x4 m3x3 m3x2
        case 6: case 7: case 14: case 15: case 0x4e: case 0x4f: return comp(3);  // scalar: replicated .w
        case 16: return comp(0) | comp(1) | comp(3);          // lit
        case 17: return k == 1 ? comp(1) | comp(2) : comp(1) | comp(3);  // dst
        default: {
            std::uint32_t m = 0;
            const std::uint32_t wm = (ins.operands[0].token >> 16) & 0xf;
            for (int c = 0; c < 4; ++c)
                if (wm >> c & 1) m |= comp(c);
            return m;
        }
    }
}

/// Temporaries read (component-wise) before anything wrote them: one line per instruction.
std::vector<std::string> uninitialised_reads(const Linker& l, const std::vector<Instruction>& prog) {
    std::vector<std::string> out;
    std::vector<std::uint32_t> written(l.symbols().size(), 0);
    for (const Instruction& ins : prog) {
        for (int k = 1; k < ins.count; ++k) {
            const Operand& o = ins.operands[k];
            if (o.symbol < 0 || l.symbols()[static_cast<std::size_t>(o.symbol)].kind != SymbolKind::Temporary) continue;
            const std::uint32_t missing = components_read(ins, k) & ~written[static_cast<std::size_t>(o.symbol)];
            if (missing) {
                std::string c;
                for (int i = 0; i < 4; ++i)
                    if (missing >> i & 1) c += "xyzw"[i];
                out.push_back(l.symbols()[static_cast<std::size_t>(o.symbol)].name + "." + c + " in '" + l.disassemble(ins) + "'");
            }
        }
        const Operand& d = ins.operands[0];
        if (d.symbol >= 0) written[static_cast<std::size_t>(d.symbol)] |= (d.token >> 16) & 0xf;
    }
    return out;
}

/// `nvlink_test --fixed SHADER_DIR script`: links every `link` line of a harness script (gen_n3.py
/// format: create / new / reserve / decl / echo / link / slot) with [Fixes] Shaders=1 and prints a
/// harness-style transcript (+ `DECL`, `FIXED 0|1`, `FALLBACK reason`, `UNINIT` = a temporary read
/// before it was written) for re/nvlink/fixcheck.py.
int dump_fixed(const char* dir, const char* script, bool fixes) {
    std::unique_ptr<Linker> linkers[2] = {load(dir, false), load(dir, true)};
    for (auto& l : linkers) l->set_fix_shaders(fixes);
    for (const std::string& line : linkers[1]->fix_report()) std::printf("# %s\n", line.c_str());
    std::ifstream in(script);
    if (!in) {
        std::fprintf(stderr, "cannot open %s\n", script);
        return 2;
    }
    Linker* l = linkers[1].get();
    LinkResult result;
    bool have = false;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream s(line);
        std::string word;
        s >> word;
        if (word == "create") {
            unsigned a = 0, opt = 1;
            s >> a >> opt;
            l = linkers[opt ? 1 : 0].get();
        } else if (word == "new") {
            l->clear_constant_reservation();
            l->clear_vertex_reservation();
            have = false;
        } else if (word == "reserve") {
            unsigned lo = 0, hi = 0;
            s >> lo >> hi;
            l->reserve_constants(lo, hi);
        } else if (word == "decl") {
            std::printf("DECL%s\n", line.substr(4).c_str());
        } else if (word == "echo") {
            std::printf("%s\n", line.size() > 5 ? line.substr(5).c_str() : "");
        } else if (word == "link") {
            std::vector<std::uint32_t> ids;
            std::string names = "LINK";
            while (s >> word) {
                std::uint32_t id = 0;
                l->fragment_id(word.c_str(), &id);
                ids.push_back(id);
                names += " " + word + "=" + std::to_string(id);
            }
            std::printf("%s\n", names.c_str());
            result = l->link(ids.data(), ids.size());
            have = true;
            if (result.hr == kOk)
                for (const std::string& u : uninitialised_reads(*l, l->program(ids.data(), ids.size(), result.fixed)))
                    std::printf("UNINIT %s\n", u.c_str());
            std::printf("CreateShader hr=%x handle=%s\n", static_cast<unsigned>(result.hr), result.hr == kOk ? "1000" : "0");
            std::printf("FIXED %d\n", result.fixed ? 1 : 0);
            if (!result.fix_fallback.empty()) std::printf("FALLBACK %s\n", result.fix_fallback.c_str());
            if (result.hr == kOk) {
                std::string t = "TOKENS";
                char b[16];
                for (const std::uint32_t x : result.tokens) {
                    std::snprintf(b, sizeof b, " %08x", static_cast<unsigned>(x));
                    t += b;
                }
                std::printf("%s\n", t.c_str());
            }
        } else if (word == "slot" && have) {
            std::string name;
            unsigned instance = 0;
            s >> name >> instance;
            std::uint32_t id = 0;
            const std::int32_t hr = l->constant_id(name.c_str(), &id);
            if (result.hr == kOk)
                std::printf("SLOT %s %u id=%x(hr %x) reg=%d(hr 0)\n", name.c_str(), instance, static_cast<unsigned>(id),
                            static_cast<unsigned>(hr), static_cast<int>(constant_slot(result, id, instance)));
        }
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc == 4 && std::strcmp(argv[1], "--fixed") == 0) return dump_fixed(argv[2], argv[3], true);
    if (argc == 4 && std::strcmp(argv[1], "--exact") == 0) return dump_fixed(argv[2], argv[3], false);
    if (argc < 3) {
        std::fprintf(stderr, "usage: nvlink_test SHADER_DIR transcript...\n       nvlink_test --fixed SHADER_DIR script\n");
        return 2;
    }
    std::unique_ptr<Linker> linkers[2] = {load(argv[1], false), load(argv[1], true)};
    api_checks(*linkers[1]);
    for (int a = 2; a < argc; ++a) {
        std::ifstream in(argv[a]);
        if (!in) {
            std::fprintf(stderr, "cannot open %s\n", argv[a]);
            return 2;
        }
        int cases = 0, linked = 0, failed_links = 0, slots = 0, line_no = 0, case_failures = 0;
        Linker* l = nullptr;
        LinkResult result;
        bool have_result = false, case_bad = false;
        std::string line;
        const auto end_case = [&] {
            if (case_bad) ++case_failures;
            case_bad = false;
        };
        const auto bad = [&](const std::string& what) {
            case_bad = true;
            fail(line_no, what);
        };
        while (std::getline(in, line)) {
            ++line_no;
            std::istringstream s(line);
            std::string word;
            s >> word;
            if (word == "CASE") {
                end_case();
                ++cases;
                std::string n, opt, res;
                s >> n >> opt >> res;
                l = linkers[opt == "opt=1" ? 1 : 0].get();
                l->clear_constant_reservation();
                l->clear_vertex_reservation();
                if (res != "reserve=none") {
                    unsigned lo = 0, hi = 0;
                    std::sscanf(res.c_str(), "reserve=%u,%u", &lo, &hi);
                    if (l->reserve_constants(lo, hi) != kOk) bad("reserve failed");
                }
                have_result = false;
            } else if (word == "LINK" && l) {
                std::vector<std::uint32_t> ids;
                while (s >> word) {
                    const auto eq = word.find('=');
                    const std::string name = word.substr(0, eq);
                    std::uint32_t id = 0;
                    if (l->fragment_id(name.c_str(), &id) != kOk || id != std::strtoul(word.c_str() + eq + 1, nullptr, 10))
                        bad("fragment id of " + name);
                    ids.push_back(id);
                }
                result = l->link(ids.data(), ids.size());
                have_result = true;
            } else if (word == "CreateShader" && have_result) {
                s >> word;  // hr=...
                const std::uint32_t hr = hex(word.substr(3));
                if (hr != static_cast<std::uint32_t>(result.hr)) {
                    char buf[96];
                    std::snprintf(buf, sizeof buf, "hr %08x, ours %08x (%s)", hr, static_cast<unsigned>(result.hr), result.error.c_str());
                    bad(buf);
                }
                if (hr == 0) ++linked; else ++failed_links;
            } else if (word == "TOKENS" && have_result) {
                std::vector<std::uint32_t> expected;
                while (s >> word) expected.push_back(hex(word));
                if (expected != result.tokens) {
                    std::size_t i = 0;
                    while (i < expected.size() && i < result.tokens.size() && expected[i] == result.tokens[i]) ++i;
                    char buf[128];
                    std::snprintf(buf, sizeof buf, "tokens differ at %zu (%zu vs ours %zu tokens)", i, expected.size(), result.tokens.size());
                    bad(buf);
                }
            } else if (word == "SLOT" && l) {
                ++slots;
                std::string name, rest;
                unsigned instance = 0;
                s >> name >> instance;
                std::getline(s, rest);
                unsigned eid = 0, ehr1 = 0, ehr2 = 0;
                int ereg = 0;
                if (std::sscanf(rest.c_str(), " id=%x(hr %x) reg=%d(hr %x)", &eid, &ehr1, &ereg, &ehr2) != 4) {
                    bad("unparsed SLOT line");
                    continue;
                }
                std::uint32_t id = 0xdead;
                const std::int32_t hr1 = l->constant_id(name.c_str(), &id);
                if (static_cast<unsigned>(hr1) != ehr1 || id != eid) bad("constant id of " + name);
                // GetConstantSlot: E_FAIL (reg untouched) before the linker created any shader.
                const bool created = have_result && result.hr == kOk;
                const std::int32_t hr2 = created ? kOk : kFail;
                const int reg = created ? constant_slot(result, id, instance) : -2;
                if (static_cast<unsigned>(hr2) != ehr2 || reg != ereg)
                    bad("slot " + name + "[" + std::to_string(instance) + "]: expected " + std::to_string(ereg) + ", ours " +
                        std::to_string(reg));
            }
        }
        end_case();
        std::printf("%s: %d cases (%d linked, %d failed as expected or not), %d slot answers: %d case(s) differ\n", argv[a],
                    cases, linked, failed_links, slots, case_failures);
    }
    std::printf(failures == 0 ? "ALL EQUAL\n" : "%d MISMATCH(ES)\n", failures);
    return failures == 0 ? 0 : 1;
}
