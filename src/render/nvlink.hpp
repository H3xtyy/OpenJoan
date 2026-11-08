// Vertex shader fragment linker, token-for-token compatible with NVIDIA NVLink 2.0
// (SHADER\nvlinker.dll, 2001) as the game uses it - stage 3 N3, notes NVLINK.md.
//
// The game ships pre-assembled fragments (*.nvo, `nvasm -l`); the linker concatenates the
// requested fragments, binds the symbolic registers (`r_*` temporaries, `c_*` constants) to real
// registers, runs nvlinker's `mov` optimizer and emits a vs.1.x token stream. This file is the
// portable core (no Windows headers): .nvo parsing (NVLINK.md 3), fragment / symbol ids, the
// constant / vertex reservations and the link itself (4.1 - 4.6: constant instances, the LIFO
// temporary allocator with its event-list liveness, the mov optimizer, the 128-instruction
// limit). Its output equals re/nvlink/nvlink.py, which equals the DLL (harness re/nvlink/harness)
// on all 1810 fragment lists the game builds and on random lists, including failures and the
// GetConstantSlot answers. The INVLink COM object around it is src/render/nvlink_com.cpp.
//
// Ids: fragments are numbered 1, 2, ... in the order they were added (GetFragmentID returns the
// first fragment of that name). Symbols are global by name across all added files, numbered in
// order of first appearance in the files' symbol tables, and their id is 125 + that index (the
// DLL numbers the 125 physical registers c0..c95, r0..r11, v0..v15, a0 first): for the game's
// seven files c_point_light_position = 0x87 ... c_clip_plane = 0xac, exactly nvlinker's ids.
//
// Shader fixes ([Fixes] Shaders=1, set_fix_shaders): off by default, and then nothing here
// differs from nvlinker. On, link() uses corrected copies of the fragments (instruction-level
// edits made once after loading, see fixes() in nvlink.cpp and NVLINK.md 13) and one list-level
// correction (r_object_position is computed before its first reader); register allocation, the
// mov optimizer and the 128-instruction limit run unchanged on the corrected program. A list
// whose corrected program does not link is linked from the original fragments instead
// (LinkResult::fix_fallback says why).
#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <vector>

/// Vertex shader fragment linker compatible with the original linker DLL (.nvo parsing, register binding, token stream).
namespace joan::render::nvlink {

// HRESULTs, as nvlinker returns them.
/// Success (S_OK).
inline constexpr std::int32_t kOk = 0;
inline constexpr std::int32_t kFail = static_cast<std::int32_t>(0x80004005u);  ///< E_FAIL: unknown name / no shader yet
inline constexpr std::int32_t kInvalidArg = static_cast<std::int32_t>(0x80070057u);  ///< E_INVALIDARG
inline constexpr std::int32_t kNotImpl = static_cast<std::int32_t>(0x80004001u);  ///< E_NOTIMPL (our stubs only)
inline constexpr std::int32_t kNoInterface = static_cast<std::int32_t>(0x80004002u);  ///< E_NOINTERFACE
/// HRESULT 0x80040400: the linked code exceeds kMaxInstructions.
inline constexpr std::int32_t kTooManyInstructions = static_cast<std::int32_t>(0x80040400u);
/// HRESULT 0x80040401: no constant register left.
inline constexpr std::int32_t kOutOfConstants = static_cast<std::int32_t>(0x80040401u);
/// HRESULT 0x80040402: out of vertex registers. TODO(re): not returned by render/nvlink.cpp; the condition in nvlinker.dll is not established.
inline constexpr std::int32_t kOutOfVertexRegisters = static_cast<std::int32_t>(0x80040402u);
// Temporary exhaustion (random fragment lists reach it; the game's never do). Checked against the DLL.
/// HRESULT 0x80040403: no temporary register left (see the comment above).
inline constexpr std::int32_t kOutOfTemporaries = static_cast<std::int32_t>(0x80040403u);

inline constexpr std::uint32_t kSymbolIdBase = 125;  ///< c0..c95 + r0..r11 + v0..v15 + a0
/// Number of constant registers (c0..c95).
inline constexpr int kConstantRegisters = 96 /**< c0..c95 */, kTemporaryRegisters = 12 /**< r0..r11 */;
/// Maximum number of instructions of a linked shader (128).
inline constexpr std::size_t kMaxInstructions = 128;

/// Symbol pools, from the name prefix (`c_` constant, `r_` temporary, `v_` input, `a_` address).
enum class SymbolKind : std::uint8_t { Temporary, Constant, Vertex, Address, Other };

/// One instruction operand as stored in a .nvo file.
struct Operand {
    std::uint32_t token = 0;  ///< D3D8 register token as in the file (number 0xfff when symbolic)
    std::uint32_t extra = 0;  ///< nvasm's second DWORD: literal number, or 0x40000 | symbol (+0x10000 = advance)
    std::int32_t symbol = -1; ///< global symbol index (Linker::symbols()), -1 = literal register
};

/// One vs.1.x instruction of a fragment.
struct Instruction {
    std::uint32_t opcode = 0;  ///< D3DSIO_* (token & 0xffff)
    std::uint8_t count = 0;    ///< number of operands used: destination first
    Operand operands[4];       ///< the operands (first `count` valid)
};

/// One pre-assembled shader fragment of a .nvo file.
struct Fragment {
    std::string name;           ///< fragment name (GetFragmentID looks it up)
    std::uint32_t version = 0;  ///< version token of its file (0xfffe0100 / 0xfffe0101)
    std::vector<Instruction> code;  ///< its instructions, symbolic registers unresolved
};

/// A symbolic register name (`c_*`, `r_*`, `v_*`, `a_*`), global across all added files.
struct Symbol {
    std::string name;                     ///< the name, prefix included
    SymbolKind kind = SymbolKind::Other;  ///< pool, from the prefix
};

/// What a link produces (nvlinker's CreateShader before the device call).
struct LinkResult {
    std::int32_t hr = kOk;              ///< HRESULT: kOk or one of the failure codes above
    std::vector<std::uint32_t> tokens;  ///< version, instructions, 0x0000ffff (empty on failure)
    std::size_t instructions = 0;       ///< instruction count after optimization
    /// Constant registers per constant symbol (global index), slot k = instance k.
    std::map<std::int32_t, std::vector<std::int32_t>> slots;
    std::string error;  ///< failure detail for logs
    bool fixed = false;        ///< the corrected fragments were linked (fix_shaders on)
    std::string fix_fallback;  ///< fix_shaders on but the corrected program failed (this reason): original linked
};

/// The portable core of the NVLink 2.0 replacement: holds the added fragments and symbols and links
/// fragment lists into vs.1.x token streams. No GL or Windows calls; failures are HRESULT-style codes.
class Linker {
public:
    /// `optimize`: run nvlinker's `mov` optimizer (on by default; off is for tests).
    explicit Linker(bool optimize = true) : optimize_(optimize) {}

    /// INVLink::AddFragments: parses one .nvo image (NVLINK.md 3) and appends its fragments.
    /// kInvalidArg on a malformed file (nothing is added then).
    std::int32_t add_fragments(const void* data, std::size_t bytes);

    /// GetFragmentID / GetConstantID / GetVertexID. kInvalidArg for null arguments, kFail for an
    /// unknown name (or a symbol of another kind).
    std::int32_t fragment_id(const char* name, std::uint32_t* id) const;
    /// See fragment_id(): the id of the constant symbol `name`.
    std::int32_t constant_id(const char* name, std::uint32_t* id) const;
    /// See fragment_id(): the id of the vertex-input symbol `name`.
    std::int32_t vertex_id(const char* name, std::uint32_t* id) const;

    /// ReserveConstantRange / ReserveVertexRange (inclusive; kInvalidArg when last < first) and
    /// their Clear* (nothing reserved; also the initial state). nvlinker keeps the constant range
    /// in its unified register numbering (c0..c95 = 0..95, r0..r11 = 96..107), so a range that
    /// reaches past 95 also removes temporaries from the pool (85..101 leaves r6..r11) - checked
    /// against the DLL. The vertex range only concerns `v_*` symbols (none in the game's files).
    std::int32_t reserve_constants(std::uint32_t first, std::uint32_t last);
    /// See reserve_constants(): the same for vertex input registers (v0..v15).
    std::int32_t reserve_vertices(std::uint32_t first, std::uint32_t last);
    /// Clears the constant reservation (nothing reserved).
    void clear_constant_reservation() { const_lo_ = const_hi_ = 0xffffffffu; }
    /// Clears the vertex reservation (nothing reserved).
    void clear_vertex_reservation() { vertex_lo_ = vertex_hi_ = 0xffffffffu; }
    /// First reserved constant register; 0xffffffff = none.
    std::uint32_t constant_reservation_first() const { return const_lo_; }
    /// Last reserved constant register (inclusive); 0xffffffff = none.
    std::uint32_t constant_reservation_last() const { return const_hi_; }
    /// First reserved vertex register; 0xffffffff = none.
    std::uint32_t vertex_reservation_first() const { return vertex_lo_; }
    /// Last reserved vertex register (inclusive); 0xffffffff = none.
    std::uint32_t vertex_reservation_last() const { return vertex_hi_; }

    /// Links fragments `ids` (1-based fragment ids, in order; a fragment listed twice is emitted
    /// twice) with the current reservations. Unknown id or an empty list -> kInvalidArg.
    LinkResult link(const std::uint32_t* ids, std::size_t count) const;
    /// Same with a 0-terminated id list (CreateShader's argument).
    LinkResult link(const std::uint32_t* ids_zero_terminated) const;
    /// The original fragments whatever fix_shaders() says (nvlinker's exact output).
    LinkResult link_exact(const std::uint32_t* ids, std::size_t count) const { return link_with(ids, count, false); }
    /// link_exact() with a 0-terminated id list.
    LinkResult link_exact(const std::uint32_t* ids_zero_terminated) const;

    /// [Fixes] Shaders: link() uses the corrected fragments (falls back to the original ones for
    /// a list whose corrected program does not link). Off = nvlinker's exact output.
    void set_fix_shaders(bool on) { fix_shaders_ = on; }
    /// Whether set_fix_shaders is on.
    bool fix_shaders() const { return fix_shaders_; }
    /// The corrected fragments (parallel to fragments(); rebuilt by every add_fragments) and one
    /// line per fix edit: "applied ..." or "skipped ..." (the fragment did not look as expected).
    const std::vector<Fragment>& fixed_fragments() const { return fixed_; }
    /// See fixed_fragments(): the "applied ..." / "skipped ..." lines.
    const std::vector<std::string>& fix_report() const { return fix_report_; }
    /// The symbolic program link() / link_exact() would allocate (fragments concatenated, with
    /// `fixes` the corrected one); empty for a bad id list. For tests and diagnostics.
    std::vector<Instruction> program(const std::uint32_t* ids, std::size_t count, bool fixes) const;

    /// Fragment-listing syntax (NVLINK.md 6.1): `mad r_diffuse.xyz, c_point_light_diffuse^,
    /// r_light_factor.y, r_diffuse` - symbols by name, `^` = advance the constant's instance.
    std::string disassemble(const Instruction& ins) const;
    /// The reverse (symbols must be known); false with `error` set on bad syntax.
    bool assemble(std::string_view text, Instruction& out, std::string* error = nullptr) const;

    /// Whether the `mov` optimizer is on (constructor argument).
    bool optimize() const { return optimize_; }
    /// The added fragments in id order (fragment id = index + 1).
    const std::vector<Fragment>& fragments() const { return fragments_; }
    /// The global symbols in order of first appearance (symbol id = kSymbolIdBase + index).
    const std::vector<Symbol>& symbols() const { return symbols_; }
    /// Global index of a symbol name, -1 if unknown.
    std::int32_t symbol_index(std::string_view name) const;

private:
    std::int32_t symbol_id(const char* name, SymbolKind kind, std::uint32_t* id) const;
    LinkResult link_with(const std::uint32_t* ids, std::size_t count, bool fixes) const;
    std::uint32_t build_program(const std::uint32_t* ids, std::size_t count, bool fixes,
                                std::vector<const Instruction*>& prog) const;
    LinkResult link_program(const std::vector<const Instruction*>& prog, std::uint32_t version) const;
    void build_fixed_fragments();

    bool optimize_;
    bool fix_shaders_ = false;
    std::vector<Fragment> fragments_;
    std::vector<Fragment> fixed_;
    std::vector<std::string> fix_report_;
    std::vector<Symbol> symbols_;
    std::map<std::string, std::int32_t, std::less<>> symbol_index_;
    std::uint32_t const_lo_ = 0xffffffffu, const_hi_ = 0xffffffffu;
    std::uint32_t vertex_lo_ = 0xffffffffu, vertex_hi_ = 0xffffffffu;
};

/// The constant register of `slots[symbol][instance]`, -1 when the shader has none
/// (GetConstantSlot's answer for a created shader).
std::int32_t constant_slot(const LinkResult& result, std::uint32_t constant_id, std::uint32_t instance);

}  // namespace joan::render::nvlink
