// Vertex shader fragment linker - see nvlink.hpp and notes NVLINK.md (sections 3 and 4).
// Follows re/nvlink/nvlink.py (the reference model, token-identical to nvlinker.dll) step by step.
#include "nvlink.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <string>

namespace joan::render::nvlink {
namespace {

constexpr std::uint32_t kMagic = 0x4f46564e;  // 'NVFO'
constexpr std::uint32_t kEnd = 0x0000ffff;
constexpr std::uint32_t kSymbolic = 0x40000, kAdvance = 0x10000;
constexpr std::uint32_t kRelative = 1u << 13;  // D3DVS_ADDRMODE_RELATIVE (c[a0.x + n])

// Register types (token bits 28..30) of vs.1.x.
constexpr std::uint32_t kTemp = 0, kInput = 1, kConst = 2, kAddr = 3;
std::uint32_t reg_type(std::uint32_t token) { return (token >> 28) & 7; }

/// Source operands of the vs.1.x opcodes the fragments can use (-1 = not an instruction we know).
int source_count(std::uint32_t op) {
    switch (op) {
        case 1: case 6: case 7: case 14: case 15: case 16: case 19: case 0x4e: case 0x4f:  // mov rcp rsq exp log lit frc expp logp
            return 1;
        case 2: case 3: case 5: case 8: case 9: case 10: case 11: case 12: case 13: case 17:  // add sub mul dp3 dp4 min max slt sge dst
        case 20: case 21: case 22: case 23: case 24:                                          // m4x4 m4x3 m3x4 m3x3 m3x2
            return 2;
        case 4: case 18:  // mad lrp
            return 3;
        default:
            return -1;
    }
}

SymbolKind kind_of(std::string_view name) {
    if (name.size() >= 2 && name[1] == '_') {
        switch (name[0]) {
            case 'r': return SymbolKind::Temporary;
            case 'c': return SymbolKind::Constant;
            case 'v': return SymbolKind::Vertex;
            case 'a': return SymbolKind::Address;
            default: break;
        }
    }
    return SymbolKind::Other;
}

std::size_t bounded_length(const char* s, std::size_t max) {
    std::size_t n = 0;
    while (n < max && s[n] != 0) ++n;
    return n;
}

std::uint32_t write_mask(std::uint32_t token) { return (token >> 16) & 0xf; }
/// Components a source reads: the identity swizzle counts as all four (NVLINK.md 4.4).
std::uint32_t read_mask(std::uint32_t token) {
    const std::uint32_t s = (token >> 16) & 0xff;
    if (s == 0xe4) return 0xf;
    std::uint32_t m = 0;
    for (int c = 0; c < 4; ++c) m |= 1u << ((s >> (2 * c)) & 3);
    return m;
}

// --- optimizer records (nvlink.py: [token, sym, type]) ------------------------------------------
constexpr std::int64_t kNoSymbol = std::numeric_limits<std::int64_t>::min();  // Python None
enum OpType : int { kTypeNone = -1, kTypeConst = 0, kTypeInput = 1, kTypeTemp = 2, kTypeAddr = 3 };
struct Rec {
    std::uint32_t token;
    std::int64_t sym;  // >= 0: global symbol; < 0 (not kNoSymbol): a literal register ('h', type, number)
    int type;
};
struct Ins {
    std::uint32_t opcode;
    std::uint8_t count;
    Rec ops[4];
};
std::int64_t literal_key(std::uint32_t type, std::uint32_t number) {
    return -1 - static_cast<std::int64_t>((type << 16) | number);
}

void rewrite(Rec& use, const Rec& src) {
    const std::uint32_t us = (use.token >> 16) & 0xff, ss = (src.token >> 16) & 0xff;
    std::uint32_t sw = 0;
    for (int c = 0; c < 4; ++c) sw |= ((ss >> (2 * ((us >> (2 * c)) & 3))) & 3) << (2 * c);
    const std::uint32_t neg = ((use.token >> 24) & 1) ^ ((src.token >> 24) & 1);
    use.token = (src.token & 0xf000ffffu) | (sw << 16) | (neg << 24);
    use.sym = src.sym;
    use.type = src.type;
}

/// nvlinker 0x1001c280: one forward pass over the movs (NVLINK.md 4.6).
void optimize_movs(std::vector<Ins>& code) {
    std::size_t p = 0;
    while (p < code.size()) {
        Ins& mov = code[p];
        if (mov.opcode != 1) {
            ++p;
            continue;
        }
        Rec& dst = mov.ops[0];
        const Rec src = mov.ops[1];
        if (dst.sym == kNoSymbol) {
            // A: mov <output>, x -> retarget earlier producers of x to the output.
            if (p == 0) {
                ++p;
                continue;
            }
            const std::uint32_t full = write_mask(dst.token);
            std::uint32_t rm = full;
            bool done = false;
            for (std::size_t q = p; q-- > 0;) {
                bool seen_use = false;
                Ins& ins = code[q];
                for (int k = 0; k < ins.count; ++k) {
                    Rec& o = ins.ops[k];
                    if (o.sym == kNoSymbol || o.sym != src.sym) continue;
                    if (k != 0) {
                        seen_use = true;
                        continue;
                    }
                    const std::uint32_t pm = write_mask(o.token);
                    if (pm & rm) {
                        if (pm & ~rm) break;  // abandon this instruction
                        o.token = (dst.token & 0xfff0ffffu) | (pm << 16);
                        o.sym = dst.sym;
                        o.type = dst.type;
                    }
                    rm &= ~pm;
                    if (rm == 0) {
                        done = true;
                        break;
                    }
                }
                if (done || seen_use) break;
            }
            if (done) {
                code.erase(code.begin() + static_cast<std::ptrdiff_t>(p));
                continue;
            }
            if (rm != full) dst.token = (dst.token & 0xfff0ffffu) | (rm << 16);
            ++p;
            continue;
        }
        if (dst.type != kTypeTemp) {
            ++p;
            continue;
        }
        // B: mov rA, x -> rewrite later reads of rA that read only what the mov wrote.
        std::uint32_t wm = write_mask(dst.token);
        const std::int64_t a = dst.sym;
        bool removable = true;
        for (std::size_t q = p + 1; q < code.size(); ++q) {
            Ins& ins = code[q];
            bool has_c = false, has_v = false, stop = false;
            for (int k = ins.count - 1; k >= 0; --k) {
                Rec& o = ins.ops[k];
                if (o.sym == kNoSymbol || o.sym != a) continue;
                if (k == 0) {
                    wm &= ~write_mask(o.token);
                    if (wm == 0) {
                        stop = true;
                        break;
                    }
                    continue;
                }
                const std::uint32_t rd = read_mask(o.token);
                if (!(rd & wm)) continue;
                if (rd & ~wm) {
                    removable = false;
                    continue;
                }
                if (src.type == kTypeTemp) rewrite(o, src);
                for (int j = 1; j < ins.count; ++j) {
                    if (ins.ops[j].type == kTypeConst) has_c = true;
                    if (ins.ops[j].type == kTypeInput) has_v = true;
                }
                if (src.type == kTypeConst) {
                    if (has_c) {
                        removable = false;
                    } else {
                        rewrite(o, src);
                        has_c = true;
                    }
                } else if (src.type == kTypeInput) {
                    if (has_v) {
                        removable = false;
                    } else {
                        rewrite(o, src);
                        has_v = true;
                    }
                }
            }
            if (stop) break;
        }
        if (removable) {
            code.erase(code.begin() + static_cast<std::ptrdiff_t>(p));
            continue;
        }
        ++p;
    }
}

LinkResult fail(std::int32_t hr, std::string why) {
    LinkResult r;
    r.hr = hr;
    r.error = std::move(why);
    return r;
}

}  // namespace

// --- .nvo files (NVLINK.md 3) --------------------------------------------------------------------
std::int32_t Linker::add_fragments(const void* data, std::size_t bytes) {
    if (data == nullptr || bytes < 0x14) return kInvalidArg;
    const std::size_t count = bytes / 4;
    std::vector<std::uint32_t> w(count);
    std::memcpy(w.data(), data, count * 4);
    if (w[0] != kMagic || w[1] != 0x14) return kInvalidArg;
    const std::uint32_t version = w[3];
    const std::size_t table = w[4];
    if (table < 5 || table > count) return kInvalidArg;

    // Symbol table: { n; char name[n * 4] } until the end of the file.
    std::vector<std::string> names;
    for (std::size_t i = table; i < count;) {
        const std::size_t n = w[i];
        if (n > count - i - 1) return kInvalidArg;
        const char* const s = reinterpret_cast<const char*>(&w[i + 1]);
        names.emplace_back(s, bounded_length(s, n * 4));
        i += 1 + n;
    }

    // Code: fragments start with an 'NVFN' comment; instructions carry two DWORDs per operand.
    std::vector<Fragment> frags;
    std::vector<std::vector<std::uint32_t>> local_syms;  // per fragment per operand: file symbol (resolved below)
    bool ended = false;
    for (std::size_t i = 5; i < table;) {
        const std::uint32_t t = w[i];
        if (t == kEnd) {
            ended = true;
            break;
        }
        if ((t & 0xffff) == 0xfffe) {
            const std::size_t n = t >> 16;
            if (n > table - i - 1) return kInvalidArg;
            const char* const body = reinterpret_cast<const char*>(&w[i + 1]);
            if (n >= 1 && std::memcmp(body, "NVFN", 4) == 0) {
                Fragment f;
                f.name.assign(body + 4, bounded_length(body + 4, n * 4 - 4));
                f.version = version;
                frags.push_back(std::move(f));
            }
            i += 1 + n;
            continue;
        }
        const int sources = source_count(t & 0xffff);
        if (sources < 0 || frags.empty()) return kInvalidArg;
        const std::size_t operands = static_cast<std::size_t>(sources) + 1;
        if (i + 1 + operands * 2 > table) return kInvalidArg;
        Instruction ins;
        ins.opcode = t & 0xffff;
        ins.count = static_cast<std::uint8_t>(operands);
        for (std::size_t k = 0; k < operands; ++k) {
            Operand& o = ins.operands[k];
            o.token = w[i + 1 + k * 2];
            o.extra = w[i + 2 + k * 2];
            if (o.extra & kSymbolic) {
                const std::uint32_t local = o.extra & 0xffff;
                if (local >= names.size()) return kInvalidArg;
                o.symbol = static_cast<std::int32_t>(local);  // file-local for now
            }
        }
        frags.back().code.push_back(ins);
        i += 1 + operands * 2;
    }
    if (!ended) return kInvalidArg;

    // File symbols -> global symbols (by name, first appearance order).
    std::vector<std::int32_t> global(names.size());
    for (std::size_t k = 0; k < names.size(); ++k) {
        const auto it = symbol_index_.find(names[k]);
        if (it != symbol_index_.end()) {
            global[k] = it->second;
            continue;
        }
        const auto index = static_cast<std::int32_t>(symbols_.size());
        symbols_.push_back({names[k], kind_of(names[k])});
        symbol_index_.emplace(names[k], index);
        global[k] = index;
    }
    for (Fragment& f : frags)
        for (Instruction& ins : f.code)
            for (int k = 0; k < ins.count; ++k)
                if (ins.operands[k].symbol >= 0) ins.operands[k].symbol = global[static_cast<std::size_t>(ins.operands[k].symbol)];
    for (Fragment& f : frags) fragments_.push_back(std::move(f));
    build_fixed_fragments();
    return kOk;
}

// --- fragment listing syntax (NVLINK.md 6.1) ----------------------------------------------------
namespace {

struct OpName {
    const char* name;
    std::uint32_t opcode;
};
constexpr OpName kOpNames[] = {{"mov", 1},   {"add", 2},   {"sub", 3},   {"mad", 4},   {"mul", 5},   {"rcp", 6},
                               {"rsq", 7},   {"dp3", 8},   {"dp4", 9},   {"min", 10},  {"max", 11},  {"slt", 12},
                               {"sge", 13},  {"exp", 14},  {"log", 15},  {"lit", 16},  {"dst", 17},  {"lrp", 18},
                               {"frc", 19},  {"m4x4", 20}, {"m4x3", 21}, {"m3x4", 22}, {"m3x3", 23}, {"m3x2", 24},
                               {"expp", 0x4e}, {"logp", 0x4f}};

std::string_view trim_view(std::string_view s) {
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.remove_prefix(1);
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) s.remove_suffix(1);
    return s;
}

bool parse_number(std::string_view s, std::uint32_t& n) {
    if (s.empty() || s.size() > 4) return false;
    n = 0;
    for (const char c : s) {
        if (c < '0' || c > '9') return false;
        n = n * 10 + static_cast<std::uint32_t>(c - '0');
    }
    return true;
}

int component(char c) {
    switch (c) {
        case 'x': return 0;
        case 'y': return 1;
        case 'z': return 2;
        case 'w': return 3;
        default: return -1;
    }
}

}  // namespace

std::string Linker::disassemble(const Instruction& ins) const {
    std::string s = "op" + std::to_string(ins.opcode);
    for (const OpName& n : kOpNames)
        if (n.opcode == ins.opcode) s = n.name;
    for (int k = 0; k < ins.count; ++k) {
        const Operand& o = ins.operands[k];
        s += k == 0 ? " " : ", ";
        const std::uint32_t t = o.token;
        if (k != 0 && ((t >> 24) & 0xf) == 1) s += '-';
        const std::uint32_t type = reg_type(t), number = o.symbol >= 0 ? 0 : (o.extra & 0x7ff);
        if (o.symbol >= 0) {
            s += symbols_[static_cast<std::size_t>(o.symbol)].name;
            if (o.extra & kAdvance) s += '^';
        } else if (type == kConst && (t & kRelative)) {
            s += "c[a0.x + " + std::to_string(number) + "]";
        } else {
            switch (type) {
                case kTemp: s += "r" + std::to_string(number); break;
                case kInput: s += "v" + std::to_string(number); break;
                case kConst: s += "c" + std::to_string(number); break;
                case kAddr: s += "a" + std::to_string(number); break;
                case 4: s += number == 0 ? "oPos" : number == 1 ? "oFog" : "oPts"; break;
                case 5: s += "oD" + std::to_string(number); break;
                case 6: s += "oT" + std::to_string(number); break;
                default: s += "?" + std::to_string(type); break;
            }
        }
        if (k == 0) {
            const std::uint32_t m = write_mask(t);
            if (m != 0xf) {
                s += '.';
                for (int c = 0; c < 4; ++c)
                    if (m >> c & 1) s += "xyzw"[c];
            }
        } else {
            const std::uint32_t sw = (t >> 16) & 0xff;
            if (sw != 0xe4) {
                s += '.';
                const bool replicate = sw == 0x00 || sw == 0x55 || sw == 0xaa || sw == 0xff;
                for (int c = 0; c < (replicate ? 1 : 4); ++c) s += "xyzw"[(sw >> (2 * c)) & 3];
            }
        }
    }
    return s;
}

bool Linker::assemble(std::string_view text, Instruction& out, std::string* error) const {
    const auto bad = [&](std::string why) {
        if (error) *error = std::move(why) + " in '" + std::string(text) + "'";
        return false;
    };
    text = trim_view(text);
    const std::size_t sp = text.find(' ');
    const std::string_view op = text.substr(0, sp);
    Instruction ins;
    for (const OpName& n : kOpNames)
        if (op == n.name) ins.opcode = n.opcode;
    const int sources = source_count(ins.opcode);
    if (ins.opcode == 0 || sources < 0) return bad("unknown opcode");
    std::vector<std::string_view> parts;
    for (std::string_view rest = sp == std::string_view::npos ? std::string_view() : text.substr(sp + 1); !rest.empty();) {
        const std::size_t comma = rest.find(',');
        parts.push_back(trim_view(rest.substr(0, comma)));
        if (comma == std::string_view::npos) break;
        rest = rest.substr(comma + 1);
    }
    if (parts.size() != static_cast<std::size_t>(sources) + 1) return bad("wrong operand count");
    ins.count = static_cast<std::uint8_t>(parts.size());
    for (std::size_t k = 0; k < parts.size(); ++k) {
        std::string_view p = parts[k];
        Operand& o = ins.operands[k];
        bool negate = false;
        if (!p.empty() && p.front() == '-') {
            if (k == 0) return bad("negated destination");
            negate = true;
            p.remove_prefix(1);
        }
        const std::size_t dot = p.find('.');
        std::string_view name = p.substr(0, dot), suffix = dot == std::string_view::npos ? std::string_view() : p.substr(dot + 1);
        bool advance = false;
        if (!name.empty() && name.back() == '^') {
            advance = true;
            name.remove_suffix(1);
        }
        std::uint32_t type = 0, number = 0;
        if (name.size() > 2 && name[1] == '_') {
            const std::int32_t sym = symbol_index(name);
            if (sym < 0) return bad("unknown symbol " + std::string(name));
            const SymbolKind kind = symbols_[static_cast<std::size_t>(sym)].kind;
            if (kind != SymbolKind::Temporary && kind != SymbolKind::Constant) return bad("unsupported symbol kind");
            type = kind == SymbolKind::Temporary ? kTemp : kConst;
            if (advance && type != kConst) return bad("'^' on a temporary");
            o.symbol = sym;
            o.extra = kSymbolic | (advance ? kAdvance : 0u);
            number = 0xfff;
        } else {
            if (advance) return bad("'^' on a literal register");
            if (name == "oPos" || name == "oFog" || name == "oPts") {
                type = 4;
                number = name == "oPos" ? 0 : name == "oFog" ? 1 : 2;
            } else if (name.size() >= 3 && name[0] == 'o' && (name[1] == 'D' || name[1] == 'T') && parse_number(name.substr(2), number)) {
                type = name[1] == 'D' ? 5 : 6;
            } else if (name.size() >= 2 && parse_number(name.substr(1), number) &&
                       (name[0] == 'r' || name[0] == 'v' || name[0] == 'c' || name[0] == 'a')) {
                type = name[0] == 'r' ? kTemp : name[0] == 'v' ? kInput : name[0] == 'c' ? kConst : kAddr;
            } else {
                return bad("unknown register " + std::string(name));
            }
            o.symbol = -1;
            o.extra = number;
        }
        std::uint32_t bits = 0;
        if (k == 0) {
            if (suffix.empty()) {
                bits = 0xf;
            } else {
                int last = -1;
                for (const char c : suffix) {
                    const int n = component(c);
                    if (n <= last) return bad("bad write mask");
                    bits |= 1u << n;
                    last = n;
                }
            }
        } else if (suffix.empty()) {
            bits = 0xe4;
        } else {
            if (suffix.size() > 4) return bad("bad swizzle");
            for (std::size_t c = 0; c < 4; ++c) {
                const int n = component(suffix[std::min(c, suffix.size() - 1)]);
                if (n < 0) return bad("bad swizzle");
                bits |= static_cast<std::uint32_t>(n) << (2 * c);
            }
        }
        o.token = 0x80000000u | (type << 28) | (bits << 16) | (negate ? 1u << 24 : 0u) | number;
    }
    out = ins;
    return true;
}

// --- shader fixes ([Fixes] Shaders=1; NVLINK.md 13) ----------------------------------------------
namespace {

/// One edit of one fragment: Replace `match` by `text`, Erase `match`, or Insert `text` after `match`.
struct Edit {
    enum Kind { Replace, Erase, InsertAfter } kind;
    const char* fragment;
    const char* match;
    const char* text;
};
struct Fix {
    const char* name;
    std::vector<Edit> edits;  // applied all-or-nothing per fragment
};

// The fixes, numbered as in NVLINK.md 13. (Fix 5, the object position, is list-level: link_with.)
const std::vector<Fix>& fixes() {
    static const std::vector<Fix> table = {
        // 1. `max r.yz, r.yzzz, 0`: D3D swizzles select per destination component, so y = max(z, 0)
        //    - the diffuse factor became the clamped specular factor. Identity swizzle: y = max(y, 0),
        //    z = max(z, 0) as intended.
        {"1 diffuse factor (max .yzzz)",
         {{Edit::Replace, "add_point_light", "max r_light_factor.yz, r_light_factor.yzzz, c0.x", "max r_light_factor.yz, r_light_factor, c0.x"},
          {Edit::Replace, "add_directional_light", "max r_light_factor.yz, r_light_factor.yzzz, c0.x", "max r_light_factor.yz, r_light_factor, c0.x"},
          {Edit::Replace, "add_spot_light", "max r_light_factor.yz, r_light_factor.yzzz, c0.x", "max r_light_factor.yz, r_light_factor, c0.x"}}},
        // 2. Directional specular: N.H read r_normal_eyespace (never computed: an alias of the half
        //    vector, N.H = 1) and H was normalize(eye position + light direction). Now
        //    H = normalize(vertex-to-eye + vertex-to-light) = normalize(-eye_to_vertex - direction)
        //    and N.H uses the world-space normal like N.L.
        {"2 directional specular (N.H, half vector)",
         {{Edit::Erase, "add_directional_light", "mov r_half_vector, c10", nullptr},
          {Edit::Replace, "add_directional_light", "add r_half_vector, r_half_vector, c_directional_light_direction^",
           "add r_half_vector, -r_normalized_eye_to_vertex, -c_directional_light_direction^"},
          {Edit::Replace, "add_directional_light", "dp3 r_light_coefficient.yz, r_normal_eyespace, r_half_vector",
           "dp3 r_light_coefficient.yz, r_object_normal, r_half_vector"}}},
        // 3. Point / spot half vector used eye-to-vertex + vertex-to-light (highlights on the wrong side).
        {"3 point / spot half vector (vertex-to-eye)",
         {{Edit::Replace, "add_point_light", "add r_half_vector, r_normalized_eye_to_vertex, r_vertex_to_light",
           "add r_half_vector, -r_normalized_eye_to_vertex, r_vertex_to_light"},
          {Edit::Replace, "add_spot_light", "add r_half_vector, r_normalized_eye_to_vertex, r_vertex_to_light",
           "add r_half_vector, -r_normalized_eye_to_vertex, r_vertex_to_light"}}},
        // 4. Object normal transformed with dp4: v3 is FLOAT3 (w = 1), so the world translation was
        //    added to the normal before normalising. dp3 = the 3x3 part only.
        {"4 normal transform (dp4 -> dp3)",
         {{Edit::Replace, "calculate_objectspace_normal", "dp4 r_object_normal.x, v3, c5", "dp3 r_object_normal.x, v3, c5"},
          {Edit::Replace, "calculate_objectspace_normal", "dp4 r_object_normal.y, v3, c6", "dp3 r_object_normal.y, v3, c6"},
          {Edit::Replace, "calculate_objectspace_normal", "dp4 r_object_normal.z, v3, c7", "dp3 r_object_normal.z, v3, c7"}}},
        // 6. add_spot_light advanced c_spot_light_attenuation's instance at its first reference too:
        //    two slots per light (dp3 slot 2i, range test slot 2i+1), while the game uploads light i's
        //    attenuation + range to slot i only. One slot per light, as add_spot_light_no_specular.
        {"6 spot attenuation slot",
         {{Edit::Replace, "add_spot_light", "dp3 r_temp.z, r_temp, c_spot_light_attenuation^",
           "dp3 r_temp.z, r_temp, c_spot_light_attenuation"}}},
        // 7. Linear fog to a texture coordinate wrote oTn.y from r_fog.y, which only height fog sets
        //    (an uninitialised register). Linear fog has no second coordinate: 0. (Dropped by the mov
        //    optimizer when nothing reads it.)
        {"7 linear fog texture y",
         {{Edit::InsertAfter, "calculate_linear_normal_fog_factor", "add r_fog.x, c0.z, -r_fog.x", "mov r_fog.y, c0.x"}}},
        // 8. Diffuse alpha: the light fragments accumulated the lights' diffuse alpha into
        //    r_diffuse.w (alpha grew with every light), and the material-diffuse writer added the
        //    ambient alpha (c9.w * c11.w). D3D: lit alpha = the diffuse material (or vertex) alpha.
        //    Lights add diffuse colour only (.xyz); write_diffuse_color_with_material_diffuse clears
        //    the ambient alpha like write_diffuse_color_with_vertex_diffuse does. (r_specular.w is
        //    left alone: specular alpha is not displayed, and masking it costs an instruction.)
        {"8 lit alpha",
         {{Edit::Replace, "add_point_light", "mad r_diffuse, c_point_light_diffuse^, r_light_factor.y, r_diffuse",
           "mad r_diffuse.xyz, c_point_light_diffuse^, r_light_factor.y, r_diffuse"},
          {Edit::Replace, "add_directional_light", "mad r_diffuse, r_light_factor.y, c_directional_light_diffuse^, r_diffuse",
           "mad r_diffuse.xyz, r_light_factor.y, c_directional_light_diffuse^, r_diffuse"},
          {Edit::Replace, "add_spot_light", "mad r_diffuse, c_spot_light_diffuse^, r_light_factor.y, r_diffuse",
           "mad r_diffuse.xyz, c_spot_light_diffuse^, r_light_factor.y, r_diffuse"},
          {Edit::Replace, "add_point_light_no_specular", "mad r_diffuse, c_point_light_diffuse^, r_light_factor.y, r_diffuse",
           "mad r_diffuse.xyz, c_point_light_diffuse^, r_light_factor.y, r_diffuse"},
          {Edit::Replace, "add_directional_light_no_specular", "mad r_diffuse, r_light_factor.y, c_directional_light_diffuse^, r_diffuse",
           "mad r_diffuse.xyz, r_light_factor.y, c_directional_light_diffuse^, r_diffuse"},
          {Edit::Replace, "add_spot_light_no_specular", "mad r_diffuse, c_spot_light_diffuse^, r_light_factor.y, r_diffuse",
           "mad r_diffuse.xyz, c_spot_light_diffuse^, r_light_factor.y, r_diffuse"},
          {Edit::InsertAfter, "write_diffuse_color_with_material_diffuse", "mul r_temp, c11, r_temp", "mov r_temp.w, c0.x"}}},
    };
    return table;
}

bool same_operand(const Operand& a, const Operand& b) {
    if (a.symbol != b.symbol) return false;
    if (a.symbol >= 0) return (a.token & ~0xfffu) == (b.token & ~0xfffu) && (a.extra & kAdvance) == (b.extra & kAdvance);
    return a.token == b.token && (a.extra & 0xffff) == (b.extra & 0xffff);
}
bool same_instruction(const Instruction& a, const Instruction& b) {
    if (a.opcode != b.opcode || a.count != b.count) return false;
    for (int k = 0; k < a.count; ++k)
        if (!same_operand(a.operands[k], b.operands[k])) return false;
    return true;
}

}  // namespace

void Linker::build_fixed_fragments() {
    fixed_ = fragments_;
    fix_report_.clear();
    for (const Fix& fix : fixes()) {
        // group the edits by fragment, apply each group to a copy, commit if all matched
        std::vector<std::string> names;
        for (const Edit& e : fix.edits)
            if (std::find(names.begin(), names.end(), e.fragment) == names.end()) names.emplace_back(e.fragment);
        for (const std::string& name : names) {
            std::uint32_t id = 0;
            if (fragment_id(name.c_str(), &id) != kOk) continue;  // not loaded (yet)
            std::vector<Instruction> code = fixed_[id - 1].code;
            std::string why;
            for (const Edit& e : fix.edits) {
                if (name != e.fragment) continue;
                Instruction match, text;
                if (!assemble(e.match, match, &why) || (e.text && !assemble(e.text, text, &why))) break;
                std::size_t at = code.size(), hits = 0;
                for (std::size_t i = 0; i < code.size(); ++i)
                    if (same_instruction(code[i], match)) {
                        at = i;
                        ++hits;
                    }
                if (hits != 1) {
                    why = std::string(hits ? "ambiguous" : "not found") + ": " + e.match;
                    break;
                }
                switch (e.kind) {
                    case Edit::Replace: code[at] = text; break;
                    case Edit::Erase: code.erase(code.begin() + static_cast<std::ptrdiff_t>(at)); break;
                    case Edit::InsertAfter: code.insert(code.begin() + static_cast<std::ptrdiff_t>(at + 1), text); break;
                }
            }
            if (why.empty()) {
                fixed_[id - 1].code = std::move(code);
                fix_report_.push_back("applied fix " + std::string(fix.name) + " to " + name);
            } else {
                fix_report_.push_back("skipped fix " + std::string(fix.name) + " in " + name + " (" + why + ")");
            }
        }
    }
}

// --- ids and reservations ------------------------------------------------------------------------
std::int32_t Linker::fragment_id(const char* name, std::uint32_t* id) const {
    if (name == nullptr || id == nullptr) return kInvalidArg;
    for (std::size_t i = 0; i < fragments_.size(); ++i)
        if (fragments_[i].name == name) {
            *id = static_cast<std::uint32_t>(i + 1);
            return kOk;
        }
    return kFail;
}

std::int32_t Linker::symbol_index(std::string_view name) const {
    const auto it = symbol_index_.find(name);
    return it == symbol_index_.end() ? -1 : it->second;
}

std::int32_t Linker::symbol_id(const char* name, SymbolKind kind, std::uint32_t* id) const {
    if (name == nullptr || id == nullptr) return kInvalidArg;
    const std::int32_t index = symbol_index(name);
    if (index < 0 || symbols_[static_cast<std::size_t>(index)].kind != kind) return kFail;
    *id = kSymbolIdBase + static_cast<std::uint32_t>(index);
    return kOk;
}
std::int32_t Linker::constant_id(const char* name, std::uint32_t* id) const { return symbol_id(name, SymbolKind::Constant, id); }
std::int32_t Linker::vertex_id(const char* name, std::uint32_t* id) const { return symbol_id(name, SymbolKind::Vertex, id); }

std::int32_t Linker::reserve_constants(std::uint32_t first, std::uint32_t last) {
    if (last < first) return kInvalidArg;
    const_lo_ = first;
    const_hi_ = last;
    return kOk;
}
std::int32_t Linker::reserve_vertices(std::uint32_t first, std::uint32_t last) {
    if (last < first) return kInvalidArg;
    vertex_lo_ = first;
    vertex_hi_ = last;
    return kOk;
}

std::int32_t constant_slot(const LinkResult& result, std::uint32_t constant_id, std::uint32_t instance) {
    if (constant_id < kSymbolIdBase) return -1;
    const auto it = result.slots.find(static_cast<std::int32_t>(constant_id - kSymbolIdBase));
    if (it == result.slots.end() || instance >= it->second.size()) return -1;
    return it->second[instance];
}

// --- link (NVLINK.md 4) --------------------------------------------------------------------------
LinkResult Linker::link(const std::uint32_t* ids_zero_terminated) const {
    if (ids_zero_terminated == nullptr) return fail(kInvalidArg, "no fragment list");
    std::size_t n = 0;
    while (ids_zero_terminated[n] != 0) ++n;
    return link(ids_zero_terminated, n);
}

LinkResult Linker::link_exact(const std::uint32_t* ids_zero_terminated) const {
    if (ids_zero_terminated == nullptr) return fail(kInvalidArg, "no fragment list");
    std::size_t n = 0;
    while (ids_zero_terminated[n] != 0) ++n;
    return link_exact(ids_zero_terminated, n);
}

LinkResult Linker::link(const std::uint32_t* ids, std::size_t count) const {
    if (!fix_shaders_) return link_with(ids, count, false);
    LinkResult r = link_with(ids, count, true);
    if (r.hr == kOk || r.hr == kInvalidArg) return r;
    // The corrected program does not fit (instructions / registers): the original one.
    LinkResult o = link_with(ids, count, false);
    if (o.hr == kOk) o.fix_fallback = r.error;
    return o;
}

LinkResult Linker::link_with(const std::uint32_t* ids, std::size_t count, bool fixes) const {
    if (ids == nullptr || count == 0) return fail(kInvalidArg, "empty fragment list");
    for (std::size_t i = 0; i < count; ++i)
        if (ids[i] == 0 || ids[i] > fragments_.size()) return fail(kInvalidArg, "unknown fragment id " + std::to_string(ids[i]));
    std::vector<const Instruction*> prog;
    const std::uint32_t version = build_program(ids, count, fixes, prog);
    LinkResult r = link_program(prog, version);
    r.fixed = fixes && r.hr == kOk;
    return r;
}

std::vector<Instruction> Linker::program(const std::uint32_t* ids, std::size_t count, bool fixes) const {
    std::vector<Instruction> out;
    if (ids == nullptr) return out;
    for (std::size_t i = 0; i < count; ++i)
        if (ids[i] == 0 || ids[i] > fragments_.size()) return out;
    std::vector<const Instruction*> prog;
    build_program(ids, count, fixes, prog);
    for (const Instruction* ins : prog) out.push_back(*ins);
    return out;
}

std::uint32_t Linker::build_program(const std::uint32_t* ids, std::size_t count, bool fixes,
                                    std::vector<const Instruction*>& prog) const {
    std::uint32_t version = 0;
    if (!fixes) {
        for (std::size_t i = 0; i < count; ++i) {
            const Fragment& f = fragments_[ids[i] - 1];
            version = std::max(version, f.version);
            for (const Instruction& ins : f.code) prog.push_back(&ins);
        }
        return version;
    }

    // Corrected fragments, plus the list-level fix "object position" (NVLINK.md 13, fix 5):
    // r_object_position (world position) is computed by calculate_objectspace_position or
    // calculate_skin_vertex. A fragment that reads it before either ran (the fog blocks of
    // unlit / pre-lit lists, height fog's eye distance) gets calculate_objectspace_position
    // inserted in front of it, and a later calculate_objectspace_position is dropped (it would
    // only recompute the same value - or, after skinning, overwrite the skinned position with
    // the unskinned one).
    const std::int32_t pos = symbol_index("r_object_position");
    std::uint32_t pos_fragment = 0;
    if (fragment_id("calculate_objectspace_position", &pos_fragment) != kOk) pos_fragment = 0;
    bool defined = false;
    const auto append = [&](std::uint32_t id) {
        const Fragment& f = fixed_[id - 1];
        version = std::max(version, f.version);
        for (const Instruction& ins : f.code) prog.push_back(&ins);
    };
    for (std::size_t i = 0; i < count; ++i) {
        const Fragment& f = fixed_[ids[i] - 1];
        bool reads_first = false, writes = false;
        for (const Instruction& ins : f.code) {
            if (pos < 0) break;
            for (int k = 1; k < ins.count; ++k)
                if (ins.operands[k].symbol == pos && !writes) reads_first = true;
            if (ins.operands[0].symbol == pos) writes = true;
        }
        if (pos_fragment != 0 && ids[i] == pos_fragment && defined) continue;
        if (pos_fragment != 0 && reads_first && !defined) {
            append(pos_fragment);
            defined = true;
        }
        append(ids[i]);
        if (writes) defined = true;
    }
    return version;
}

LinkResult Linker::link_program(const std::vector<const Instruction*>& prog, std::uint32_t version) const {
    // 4.1 constant pool: c0..c95 minus the reservation minus every literally named constant.
    bool literal_const[kConstantRegisters] = {};
    for (const Instruction* ins : prog)
        for (int k = 0; k < ins->count; ++k) {
            const Operand& o = ins->operands[k];
            // (a relative c[a0.x + n] does not count: hardware_skin's c[a0.x + 17] leaves c17 free)
            if (o.symbol < 0 && reg_type(o.token) == kConst && !(o.token & kRelative) && (o.extra & 0xfff) < kConstantRegisters)
                literal_const[o.extra & 0xfff] = true;
            if (o.symbol >= 0 && reg_type(o.token) != kTemp && reg_type(o.token) != kConst)
                return fail(kInvalidArg, "symbolic operand of register type " + std::to_string(reg_type(o.token)) +
                                             " (" + symbols_[static_cast<std::size_t>(o.symbol)].name + ") is not supported");
        }
    std::vector<std::int32_t> cpool;
    for (std::uint32_t c = 0; c < static_cast<std::uint32_t>(kConstantRegisters); ++c)
        if (!(const_lo_ <= c && c <= const_hi_) && !literal_const[c]) cpool.push_back(static_cast<std::int32_t>(c));
    std::size_t cnext = 0;  // allocation takes the lowest free register
    // Temporaries: LIFO stack, r0 first; released registers are handed out next.
    std::vector<std::int32_t> tfree;
    // The constant reservation is in nvlinker's unified register numbering (c0..c95 = 0..95,
    // r0..r11 = 96..107, ...): a range reaching past 95 also takes temporaries.
    for (int r = kTemporaryRegisters - 1; r >= 0; --r) {
        const std::uint32_t unified = static_cast<std::uint32_t>(kConstantRegisters + r);
        if (!(const_lo_ <= unified && unified <= const_hi_)) tfree.push_back(r);
    }

    // 4.4 per-temporary event lists: instructions forward, operands last to first.
    struct Event {
        bool def;
        std::uint32_t mask;
    };
    const std::size_t nsym = symbols_.size();
    std::vector<std::vector<Event>> events(nsym);
    std::vector<std::array<std::int32_t, 4>> evpos(prog.size(), {-1, -1, -1, -1});
    for (std::size_t i = 0; i < prog.size(); ++i)
        for (int k = prog[i]->count - 1; k >= 0; --k) {
            const Operand& o = prog[i]->operands[k];
            if (o.symbol < 0 || reg_type(o.token) != kTemp) continue;
            auto& list = events[static_cast<std::size_t>(o.symbol)];
            evpos[i][static_cast<std::size_t>(k)] = static_cast<std::int32_t>(list.size());
            list.push_back({k == 0, k == 0 ? write_mask(o.token) : read_mask(o.token)});
        }
    const auto dead_after = [&](std::int32_t sym, std::int32_t pos) {
        std::uint32_t m = 0xf;
        const auto& list = events[static_cast<std::size_t>(sym)];
        for (std::size_t e = static_cast<std::size_t>(pos) + 1; e < list.size(); ++e) {
            if (list[e].def)
                m &= ~list[e].mask;
            else if (m & list[e].mask)
                return false;
        }
        return true;
    };

    // 4.2 - 4.4 one pass, operands last to first.
    LinkResult result;
    std::vector<std::uint32_t> instance(nsym, 0);
    std::vector<std::int32_t> temp_reg(nsym, -1);
    std::vector<std::array<std::int32_t, 4>> out(prog.size(), {-1, -1, -1, -1});
    for (std::size_t i = 0; i < prog.size(); ++i)
        for (int k = prog[i]->count - 1; k >= 0; --k) {
            const Operand& o = prog[i]->operands[k];
            if (o.symbol < 0) continue;
            const auto sym = static_cast<std::size_t>(o.symbol);
            if (reg_type(o.token) == kConst) {
                auto& slots = result.slots[o.symbol];
                const std::uint32_t n = instance[sym];
                if (n >= slots.size()) {
                    if (cnext == cpool.size()) return fail(kOutOfConstants, "out of constant registers");
                    slots.push_back(cpool[cnext++]);
                }
                out[i][static_cast<std::size_t>(k)] = slots[n];
                if (o.extra & kAdvance) instance[sym] = n + 1;
            } else {  // temporary
                if (temp_reg[sym] < 0) {
                    if (tfree.empty()) return fail(kOutOfTemporaries, "out of temporary registers");
                    temp_reg[sym] = tfree.back();
                    tfree.pop_back();
                }
                const std::int32_t r = temp_reg[sym];
                out[i][static_cast<std::size_t>(k)] = r;
                if (dead_after(o.symbol, evpos[i][static_cast<std::size_t>(k)])) {
                    tfree.push_back(r);
                    temp_reg[sym] = -1;
                }
            }
        }

    // Operand records for the optimizer.
    std::vector<Ins> code(prog.size());
    for (std::size_t i = 0; i < prog.size(); ++i) {
        const Instruction& src = *prog[i];
        Ins& ins = code[i];
        ins.opcode = src.opcode;
        ins.count = src.count;
        for (int k = 0; k < src.count; ++k) {
            const Operand& o = src.operands[k];
            Rec& rec = ins.ops[k];
            std::uint32_t t = o.token;
            if (out[i][static_cast<std::size_t>(k)] >= 0)
                t = (t & ~0xfffu) | static_cast<std::uint32_t>(out[i][static_cast<std::size_t>(k)]);
            else if (o.symbol < 0)
                t = (t & ~0xfffu) | (o.extra & 0xfff);
            rec.token = t;
            const std::uint32_t type = reg_type(o.token);
            if (o.symbol >= 0) {
                rec.sym = o.symbol;
                rec.type = type == kTemp ? kTypeTemp : type == kConst ? kTypeConst : kTypeInput;
            } else if (type == kInput || type == kConst) {
                rec.sym = literal_key(type, t & 0xfff);
                rec.type = type == kConst ? kTypeConst : kTypeInput;
            } else if (type == kAddr) {
                rec.sym = literal_key(type, t & 0xfff);
                rec.type = kTypeAddr;
            } else {
                rec.sym = kNoSymbol;
                rec.type = kTypeNone;
            }
        }
    }
    if (optimize_) optimize_movs(code);
    if (code.size() > kMaxInstructions) {
        LinkResult r = fail(kTooManyInstructions, "too many instructions (" + std::to_string(code.size()) + ")");
        r.instructions = code.size();
        return r;
    }

    result.instructions = code.size();
    result.tokens.reserve(2 + code.size() * 4);
    result.tokens.push_back(version);
    for (const Ins& ins : code) {
        result.tokens.push_back(ins.opcode);
        for (int k = 0; k < ins.count; ++k) result.tokens.push_back(ins.ops[k].token);
    }
    result.tokens.push_back(kEnd);
    return result;
}

}  // namespace joan::render::nvlink
