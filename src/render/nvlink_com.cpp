// module render/nvlink (on by default; [Hooks] Disable=render/nvlink loads SHADER\nvlinker.dll
// instead) - stage 3 N3, notes NVLINK.md.
//
// An INVLink-compatible COM object around src/render/nvlink.cpp, returned by
// VsCache::create_linker (0x00618990, C++ in en3d_helpers22.cpp - so en3d/helpers22 must be
// enabled too) instead of SHADER\nvlinker.dll's CreateLinker(100, 1). nvlinker.dll is then not
// loaded at all (VsCache +0x1c stays null, so the cache clear skips its FreeLibrary). The
// fragment files are still read by VsCache::load_fragment (0x00618ab0) from the game's paths and
// handed to AddFragments (3), exactly as before.
//
// Vtable (nvlinker 0x1003d3a0), every entry __stdcall with `this` first except 19:
//    0 QueryInterface(iid, out)      IUnknown / INVLink {561e7d82-...}, as nvlinker
//    1 AddRef  2 Release             Release at 0: drop the last-shader and cache references
//    3 AddFragments(nvo, bytes)      4 GetFragmentID(name, id*)   5 GetConstantID(name, id*)
//    6 GetVertexID(name, id*)        7 ReserveConstantRange(first, last)
//    8 ReserveVertexRange(first, last)   9 ClearConstantReservation   10 ClearVertexReservation
//   11 (3 args, link to a buffer)    stub: E_NOTIMPL, logged
//   12 CreateShader(device, declaration, fragment ids (0-terminated), handle*, usage, flags):
//      links, then device->CreateVertexShader(declaration, tokens, handle, usage) and AddRef
//   13 DeleteShader(device, handle)  14 (1 arg, listing) stub   15 GetLinkStats(stats*) stub
//   16 GetConstantSlot(id, instance, reg*)  (of the last created shader; -1 = none)
//   17 GetVertexSlot(id, instance, reg*)    18 flush (internal)
//   19 scalar deleting destructor (__thiscall this, flags; called by nvlinker's own Release only)
// Shader lifetime follows what the harness showed for the DLL: a created shader holds a user, a
// cache and a last-shader reference; DeleteShader drops the user one, Release drops the other two,
// so the game's DeleteShader + Release deletes the device shader on Release, and a shader never
// passed to DeleteShader stays alive on the device (as with nvlinker). A repeated CreateShader with
// the same fragments, declaration and reservations returns the cached handle.
//
// (Until stage S7c a verify mode ran nvlinker.dll next to this object and compared every answer,
// including the whole token stream of each shader; it went with joan.exe's code.)
//
// [Fixes] Shaders=1 (Settings::fix_shaders): the linker links corrected fragments (render/
// nvlink.cpp, NVLINK.md 13); the first CreateShader logs which fixes were applied, and a list whose
// corrected program does not fit is linked from the original fragments (logged once per list).
#include "../callconv.hpp"
#include "nvlink_com.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "../registry.hpp"
#include "nvlink.hpp"

namespace joan::hook::nvlink {
namespace {

namespace core = joan::render::nvlink;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr char kModule[] = "render/nvlink";
constexpr int kVtableSize = 20;

// IDirect3DDevice8 vtable indices.
constexpr int kDevAddRef = 1, kDevRelease = 2, kCreateVertexShader = 75, kDeleteVertexShader = 78;

JOAN_COUNTER(c_linker, "nvlink::CreateLinker");
JOAN_COUNTER(c_create, "nvlink::CreateShader");
JOAN_COUNTER(c_slot, "nvlink::GetConstantSlot");

template <class R = i32, class... A>
R com_call(void* object, int index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

struct Shader {
    std::vector<u32> key;
    core::LinkResult result;
    void* device = nullptr;  // AddRef'd
    u32 handle = 0;
    int refs = 0;
};

struct Object {
    void** vtable;  // must stay first: the game calls through it
    u32 refs = 1;
    core::Linker linker;
    std::map<std::vector<u32>, Shader*> cache;
    std::map<u32, Shader*> by_handle;
    Shader* last = nullptr;
    std::set<int> unused_logged;

    explicit Object(void** table, bool optimize) : vtable(table), linker(optimize) {
        linker.set_fix_shaders(settings().fix_shaders);
    }
};

Object* self_of(void* p) { return static_cast<Object*>(p); }

std::string fragment_names(const core::Linker& linker, const u32* ids) {
    std::string s;
    for (const u32* f = ids; f && *f; ++f) {
        s += ' ';
        if (*f >= 1 && *f <= linker.fragments().size())
            s += linker.fragments()[*f - 1].name;
        else
            s += "#" + std::to_string(*f);
    }
    return s;
}

std::vector<u32> read_list(const u32* p, u32 terminator, std::size_t limit = 4096) {
    std::vector<u32> v;
    for (; p && v.size() < limit; ++p) {
        if (*p == terminator) break;
        v.push_back(*p);
    }
    return v;
}

void unused_called(Object* o, int index) {
    if (o->unused_logged.insert(index).second)
        log_line("nvlink: INVLink method %d called (not used by the game; %s)", index, "stub, returns E_NOTIMPL");
}

// --- shader lifetime (ours only) -----------------------------------------------------------------
void unref(Object* o, Shader* s) {
    if (s == nullptr || --s->refs > 0) return;
    if (s->device) {
        com_call(s->device, kDeleteVertexShader, s->handle);
        com_call<u32>(s->device, kDevRelease);
    }
    o->cache.erase(s->key);
    o->by_handle.erase(s->handle);
    if (o->last == s) o->last = nullptr;
    delete s;
}

/// nvlinker 0x100022e0: drop the last-shader reference and the cache's references. Shaders the
/// game still holds (no DeleteShader) keep their device shader, as with nvlinker.
void flush(Object* o) {
    Shader* const last = o->last;
    o->last = nullptr;
    unref(o, last);
    std::vector<Shader*> entries;
    for (auto& [key, s] : o->cache) entries.push_back(s);
    o->cache.clear();
    for (Shader* s : entries) {
        if (s->refs > 1) {  // a reference the game never gave back: leave the device shader alive
            o->by_handle.erase(s->handle);
            delete s;
            continue;
        }
        unref(o, s);
    }
}

void destroy(Object* o) {
    flush(o);
    for (auto& [handle, s] : o->by_handle) delete s;  // (nothing left normally)
    o->by_handle.clear();
    delete o;
}

// --- vtable entries ------------------------------------------------------------------------------
i32 JOAN_STDCALL query_interface(void* p, const u32* iid, void** out) {
    static const u32 kUnknown[4] = {0x00000000, 0x00000000, 0x000000c0, 0x46000000};
    static const u32 kINVLink[4] = {0x561e7d82, 0x4dddc854, 0x031ee6a5, 0x62d04740};
    if (out == nullptr) return core::kInvalidArg;
    if (iid && (std::memcmp(iid, kUnknown, 16) == 0 || std::memcmp(iid, kINVLink, 16) == 0)) {
        ++self_of(p)->refs;
        *out = p;
        return core::kOk;
    }
    return core::kNoInterface;
}

u32 JOAN_STDCALL add_ref(void* p) { return ++self_of(p)->refs; }

u32 JOAN_STDCALL release(void* p) {
    Object* const o = self_of(p);
    const u32 refs = --o->refs;
    if (refs == 0) destroy(o);
    return refs;
}

i32 JOAN_STDCALL add_fragments(void* p, const void* data, u32 bytes) {
    Object* const o = self_of(p);
    const i32 ours = o->linker.add_fragments(data, bytes);
    if (ours != core::kOk) log_line("nvlink: AddFragments (%u bytes) failed: not an .nvo file", static_cast<unsigned>(bytes));
    return ours;
}

i32 JOAN_STDCALL fragment_id(void* p, const char* name, u32* id) { return self_of(p)->linker.fragment_id(name, id); }
i32 JOAN_STDCALL constant_id(void* p, const char* name, u32* id) { return self_of(p)->linker.constant_id(name, id); }
i32 JOAN_STDCALL vertex_id(void* p, const char* name, u32* id) { return self_of(p)->linker.vertex_id(name, id); }

i32 JOAN_STDCALL reserve_constants(void* p, u32 first, u32 last) {
    Object* const o = self_of(p);
    return o->linker.reserve_constants(first, last);
}
i32 JOAN_STDCALL reserve_vertices(void* p, u32 first, u32 last) {
    Object* const o = self_of(p);
    return o->linker.reserve_vertices(first, last);
}
i32 JOAN_STDCALL clear_constants(void* p) {
    Object* const o = self_of(p);
    o->linker.clear_constant_reservation();
    return core::kOk;
}
i32 JOAN_STDCALL clear_vertices(void* p) {
    Object* const o = self_of(p);
    o->linker.clear_vertex_reservation();
    return core::kOk;
}

i32 JOAN_STDCALL method11(void* p, u32 /*a*/, u32 /*b*/, u32 /*c*/) {
    Object* const o = self_of(p);
    unused_called(o, 11);
    return core::kNotImpl;
}
i32 JOAN_STDCALL method14(void* p, u32 /*a*/) {
    Object* const o = self_of(p);
    unused_called(o, 14);
    return core::kNotImpl;
}
i32 JOAN_STDCALL link_stats(void* p, u32* stats) {
    Object* const o = self_of(p);
    unused_called(o, 15);
    if (stats == nullptr || stats[0] != 0x1c) return core::kInvalidArg;
    return core::kNotImpl;
}

void log_link_failure(Object* o, const u32* ids, const core::LinkResult& r) {
    static std::set<std::vector<u32>> logged;
    if (!logged.insert(read_list(ids, 0)).second) return;
    log_line("nvlink: link failed hr=0x%08x (%s):%s", static_cast<unsigned>(r.hr), r.error.c_str(),
             fragment_names(o->linker, ids).c_str());
}

/// [Fixes] Shaders: once per session, what the fixes did to the loaded fragments; once per list,
/// a corrected program that did not fit (the original one is used).
void log_fixes(Object* o, const u32* ids, const core::LinkResult& r) {
    if (!o->linker.fix_shaders()) return;
    static bool reported = false;
    if (!reported) {
        reported = true;
        log_line("nvlink: [Fixes] Shaders=1: corrected vertex shaders (NVLINK.md 13)");
        for (const std::string& line : o->linker.fix_report()) log_line("nvlink:   %s", line.c_str());
    }
    if (r.fix_fallback.empty()) return;
    static std::set<std::vector<u32>> logged;
    if (!logged.insert(read_list(ids, 0)).second) return;
    log_line("nvlink: [Fixes] Shaders: corrected shader does not link (%s) - original fragments used:%s", r.fix_fallback.c_str(),
             fragment_names(o->linker, ids).c_str());
}

/// Ours only: nvlinker 0x10005610.
i32 create_ours(Object* o, void* device, const u32* declaration, const u32* ids, u32* handle, u32 usage) {
    if (device == nullptr || ids == nullptr || handle == nullptr) return core::kInvalidArg;
    std::vector<u32> key = read_list(ids, 0);
    key.push_back(0);
    const std::vector<u32> decl = read_list(declaration, 0xffffffffu);
    key.insert(key.end(), decl.begin(), decl.end());
    key.push_back(0xffffffffu);
    key.push_back(o->linker.constant_reservation_first());
    key.push_back(o->linker.constant_reservation_last());
    key.push_back(o->linker.vertex_reservation_first());
    key.push_back(o->linker.vertex_reservation_last());
    if (const auto hit = o->cache.find(key); hit != o->cache.end()) {
        Shader* const s = hit->second;
        Shader* const old = o->last;
        o->last = nullptr;
        unref(o, old);
        s->refs += 2;  // last shader + caller
        o->last = s;
        *handle = s->handle;
        return core::kOk;
    }
    core::LinkResult r = o->linker.link(ids);
    log_fixes(o, ids, r);
    if (r.hr != core::kOk) {
        log_link_failure(o, ids, r);
        return r.hr;
    }
    u32 h = 0;
    const i32 hr = com_call(device, kCreateVertexShader, declaration, static_cast<const u32*>(r.tokens.data()), &h, usage);
    if (hr < 0) {
        log_line("nvlink: CreateVertexShader failed hr=0x%08x:%s", static_cast<unsigned>(hr), fragment_names(o->linker, ids).c_str());
        return hr;
    }
    com_call<u32>(device, kDevAddRef);
    Shader* const s = new Shader{std::move(key), std::move(r), device, h, 3};  // caller, cache, last shader
    Shader* const old = o->last;
    o->last = nullptr;
    unref(o, old);
    o->last = s;
    o->cache.emplace(s->key, s);
    o->by_handle[h] = s;
    *handle = h;
    return core::kOk;
}

i32 JOAN_STDCALL create_shader(void* p, void* device, const u32* declaration, const u32* ids, u32* handle, u32 usage, u32 flags) {
    Object* const o = self_of(p);
    c_create.hit();
    if (flags != 0) {
        static bool logged = false;
        if (!logged) log_line("nvlink: CreateShader flags=0x%x (ignored; the game passes 0)", static_cast<unsigned>(flags));
        logged = true;
    }
    return create_ours(o, device, declaration, ids, handle, usage);
}

i32 JOAN_STDCALL delete_shader(void* p, void* /*device*/, u32 handle) {
    Object* const o = self_of(p);
    const auto it = o->by_handle.find(handle);
    if (it == o->by_handle.end()) return core::kFail;
    unref(o, it->second);
    return core::kOk;
}

i32 JOAN_STDCALL constant_slot(void* p, u32 id, u32 instance, i32* reg) {
    Object* const o = self_of(p);
    c_slot.hit();
    if (reg == nullptr) return core::kInvalidArg;
    if (o->last == nullptr) return core::kFail;
    *reg = core::constant_slot(o->last->result, id, instance);
    return core::kOk;
}

i32 JOAN_STDCALL vertex_slot(void* p, u32 /*id*/, u32 /*instance*/, i32* reg) {
    Object* const o = self_of(p);
    unused_called(o, 17);
    if (reg == nullptr) return core::kInvalidArg;
    if (o->last == nullptr) return core::kFail;
    *reg = -1;  // the game's fragments have no v_ symbols
    return core::kOk;
}

i32 JOAN_STDCALL flush_entry(void* p) {
    Object* const o = self_of(p);
    unused_called(o, 18);
    flush(o);
    return core::kOk;
}

// 19: __thiscall scalar deleting destructor (this in ecx, flags on the stack, callee pops) -
// __fastcall has the same register / stack use with the unused edx argument (Windows x86 only: x64 and Linux have one convention,
// flags follow `this` directly).
#if defined(JOAN_X86_CONVENTIONS)
void* JOAN_FASTCALL deleting_destructor(void* p, void* /*edx*/, u32 flags) {
#else
void* deleting_destructor(void* p, u32 flags) {
#endif
    Object* const o = self_of(p);
    unused_called(o, 19);
    if (flags & 1) {
        destroy(o);
        return p;
    }
    return p;  // (not freeing without the flag; never called that way)
}

void** vtable() {
    static void* table[kVtableSize] = {
        reinterpret_cast<void*>(&query_interface),   reinterpret_cast<void*>(&add_ref),
        reinterpret_cast<void*>(&release),           reinterpret_cast<void*>(&add_fragments),
        reinterpret_cast<void*>(&fragment_id),       reinterpret_cast<void*>(&constant_id),
        reinterpret_cast<void*>(&vertex_id),         reinterpret_cast<void*>(&reserve_constants),
        reinterpret_cast<void*>(&reserve_vertices),  reinterpret_cast<void*>(&clear_constants),
        reinterpret_cast<void*>(&clear_vertices),    reinterpret_cast<void*>(&method11),
        reinterpret_cast<void*>(&create_shader),     reinterpret_cast<void*>(&delete_shader),
        reinterpret_cast<void*>(&method14),          reinterpret_cast<void*>(&link_stats),
        reinterpret_cast<void*>(&constant_slot),     reinterpret_cast<void*>(&vertex_slot),
        reinterpret_cast<void*>(&flush_entry),       reinterpret_cast<void*>(&deleting_destructor),
    };
    return table;
}

}  // namespace

bool enabled() {
    static const bool on = !module_disabled(kModule);  // on by default since stage S7d, like every module
    return on;
}

void* create_linker(std::uint32_t optimize) {
    c_linker.hit();
    static bool announced = false;
    if (!announced) {
        log_line("nvlink: %s: vertex shaders linked by our linker%s", kModule, " (nvlinker.dll not loaded)");
        announced = true;
    }
    return new Object(vtable(), optimize != 0);
}

}  // namespace joan::hook::nvlink
