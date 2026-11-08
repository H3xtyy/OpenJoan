// Vertex shader linker log ([Graphics] ShaderLog=1) - see nvlink_log.hpp and notes NVLINK.md.
//
// nvlinker.dll's INVLink object (CreateLinker(100, 1), one per VsCache entry) is used through
// these vtable entries (stdcall, `this` first):
//    4 GetFragmentID(name, &id)          5 GetConstantID(name, &id)
//    7 ReserveConstantRange(first, last) 9 / 10 clear the constant / vertex reservation
//   12 CreateShader(device, declaration, fragment ids (0-terminated), &handle, usage, flags) - links
//      and calls device->CreateVertexShader itself, so the function tokens never reach the game;
//      they are read back with IDirect3DDevice8::GetVertexShaderFunction.
//   16 GetConstantSlot(id, instance, &register)  (register -1 = not in this shader)
// The wrappers log and forward; each distinct shader and each (shader, constant, instance)
// answer is written once.
#include "../callconv.hpp"
#include "nvlink_log.hpp"

#include "../compiler.hpp"

#include <cstdint>
#include <cstdio>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "../registry.hpp"

namespace joan::hook::nvlink_log {
namespace {

using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr int kVtableSize = 20;  // INVLink vtable at nvlinker.dll 0x1003d3a0
constexpr int kFragmentId = 4, kConstantId = 5, kReserveConstants = 7, kCreateShader = 12, kConstantSlot = 16;
constexpr int kGetVertexShaderFunction = 82;  // IDirect3DDevice8

using FragmentIdFn = i32(JOAN_STDCALL*)(void*, const char*, u32*);
using ReserveFn = i32(JOAN_STDCALL*)(void*, u32, u32);
using CreateShaderFn = i32(JOAN_STDCALL*)(void*, void*, const u32*, const u32*, u32*, u32, u32);
using ConstantSlotFn = i32(JOAN_STDCALL*)(void*, u32, u32, i32*);
using FunctionFn = i32(JOAN_STDCALL*)(void*, u32, void*, u32*);

struct LinkerState {
    u32 reserve_first = 0, reserve_last = 0;
    bool reserved = false;
    int shader = -1;  // index of the logged shader this linker produced
};

struct State {
    std::mutex mutex;
    void* const* original = nullptr;  // the DLL's vtable
    void* table[kVtableSize] = {};
    std::map<u32, std::string> fragment_names, constant_names;
    std::map<void*, LinkerState> linkers;
    std::map<std::vector<u32>, int> shaders;  // fragments | declaration | function -> index
    std::set<std::tuple<int, u32, u32>> slots_logged;
};
State& state() {
    static State s;
    return s;
}

template <class Fn>
Fn original(int index) {
    return reinterpret_cast<Fn>(state().original[index]);
}

void log_tokens(const char* what, const u32* tokens, std::size_t count) {
    log_line("nvlink:   %s (%u tokens):", what, static_cast<unsigned>(count));
    for (std::size_t i = 0; i < count; i += 16) {
        char line[16 * 9 + 1];
        int n = 0;
        for (std::size_t j = i; j < count && j < i + 16; ++j)
            n += std::snprintf(line + n, sizeof(line) - static_cast<std::size_t>(n), " %08x", static_cast<unsigned>(tokens[j]));
        log_line("nvlink:    %s", line);
    }
}

i32 JOAN_STDCALL fragment_id(void* self, const char* name, u32* id) {
    const i32 hr = original<FragmentIdFn>(kFragmentId)(self, name, id);
    if (hr == 0 && name && id) {
        std::lock_guard lock(state().mutex);
        state().fragment_names.emplace(*id, name);
    }
    return hr;
}

i32 JOAN_STDCALL constant_id(void* self, const char* name, u32* id) {
    const i32 hr = original<FragmentIdFn>(kConstantId)(self, name, id);
    if (hr == 0 && name && id) {
        std::lock_guard lock(state().mutex);
        state().constant_names.emplace(*id, name);
    }
    return hr;
}

i32 JOAN_STDCALL reserve_constants(void* self, u32 first, u32 last) {
    const i32 hr = original<ReserveFn>(kReserveConstants)(self, first, last);
    std::lock_guard lock(state().mutex);
    auto& l = state().linkers[self];
    l.reserve_first = first;
    l.reserve_last = last;
    l.reserved = true;
    return hr;
}

i32 JOAN_STDCALL create_shader(void* self, void* device, const u32* declaration, const u32* fragments, u32* handle, u32 usage,
                            u32 flags) {
    const i32 hr = original<CreateShaderFn>(kCreateShader)(self, device, declaration, fragments, handle, usage, flags);
    std::vector<u32> decl, function;
    if (declaration)
        for (const u32* t = declaration;; ++t) {
            decl.push_back(*t);
            if (*t == 0xffffffffu || decl.size() > 256) break;
        }
    if (hr == 0 && device && handle) {
        u32 size = 0;
        const auto get = reinterpret_cast<FunctionFn>(joan::call::vtable_of(device)[kGetVertexShaderFunction]);
        if (get(device, *handle, nullptr, &size) == 0 && size >= 4 && size < 0x10000) {
            function.resize(size / 4);
            if (get(device, *handle, function.data(), &size) != 0) function.clear();
        }
    }
    std::lock_guard lock(state().mutex);
    State& s = state();
    std::vector<u32> key;
    std::string names;
    std::size_t count = 0;
    for (const u32* f = fragments; f && *f != 0; ++f, ++count) {
        key.push_back(*f);
        const auto it = s.fragment_names.find(*f);
        char id[16];
        std::snprintf(id, sizeof(id), "#%u", static_cast<unsigned>(*f));
        names += ' ';
        names += it != s.fragment_names.end() ? it->second : std::string(id);
    }
    LinkerState& l = s.linkers[self];
    key.push_back(0);
    key.push_back(l.reserve_last);
    key.push_back(static_cast<u32>(hr));
    key.insert(key.end(), decl.begin(), decl.end());
    key.insert(key.end(), function.begin(), function.end());
    const auto [it, added] = s.shaders.emplace(std::move(key), static_cast<int>(s.shaders.size()) + 1);
    l.shader = it->second;
    if (!added) return hr;
    log_line("nvlink: shader %d: CreateShader hr=0x%08x handle=0x%x usage=0x%x flags=0x%x reserved c%u..c%u%s", it->second,
             static_cast<unsigned>(hr), handle ? static_cast<unsigned>(*handle) : 0u, static_cast<unsigned>(usage),
             static_cast<unsigned>(flags), static_cast<unsigned>(l.reserve_first), static_cast<unsigned>(l.reserve_last),
             l.reserved ? "" : " (no reservation seen)");
    log_line("nvlink:   fragments (%u):%s", static_cast<unsigned>(count), names.c_str());
    log_tokens("declaration", decl.data(), decl.size());
    if (!function.empty())
        log_tokens("function", function.data(), function.size());
    else if (hr == 0)
        log_line("nvlink:   function: not available from the device");
    return hr;
}

i32 JOAN_STDCALL constant_slot(void* self, u32 id, u32 instance, i32* slot) {
    const i32 hr = original<ConstantSlotFn>(kConstantSlot)(self, id, instance, slot);
    void* const caller = JOAN_RETURN_ADDRESS();
    std::lock_guard lock(state().mutex);
    State& s = state();
    const auto l = s.linkers.find(self);
    const int shader = l == s.linkers.end() ? -1 : l->second.shader;
    const auto key = std::make_tuple(shader, id, instance);
    if (s.slots_logged.count(key) != 0) return hr;  // VsConstants::set runs every frame: no allocation here
    s.slots_logged.insert(key);
    const auto name = s.constant_names.find(id);
    char reg[16];
    if (slot && *slot >= 0)
        std::snprintf(reg, sizeof(reg), "c%d", static_cast<int>(*slot));
    else
        std::snprintf(reg, sizeof(reg), "%d", slot ? static_cast<int>(*slot) : -2);
    log_line("nvlink: shader %d: constant %s[%u] -> %s (hr=0x%08x, caller 0x%08llx)", shader,
             name != s.constant_names.end() ? name->second.c_str() : "?", static_cast<unsigned>(instance), reg,
             static_cast<unsigned>(hr), static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(caller)));
    return hr;
}

}  // namespace

void wrap_linker(void* linker) {
    if (!settings().shader_log || linker == nullptr) return;
    State& s = state();
    std::lock_guard lock(s.mutex);
    void* const* const vtable = joan::call::vtable_of(linker);
    if (s.original == nullptr) {
        s.original = vtable;
        for (int i = 0; i < kVtableSize; ++i) s.table[i] = vtable[i];
        s.table[kFragmentId] = reinterpret_cast<void*>(&fragment_id);
        s.table[kConstantId] = reinterpret_cast<void*>(&constant_id);
        s.table[kReserveConstants] = reinterpret_cast<void*>(&reserve_constants);
        s.table[kCreateShader] = reinterpret_cast<void*>(&create_shader);
        s.table[kConstantSlot] = reinterpret_cast<void*>(&constant_slot);
        log_line("nvlink: ShaderLog on (INVLink vtable at 0x%08llx)", static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(vtable)));
    } else if (vtable != s.original) {
        log_line("nvlink: linker 0x%08llx has an unexpected vtable 0x%08llx - not logged",
                 static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(linker)),
                 static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(vtable)));
        return;
    }
    s.linkers[linker] = LinkerState{};  // a freed linker's address can come back
    joan::call::set_vtable(linker, s.table);
}

}  // namespace joan::hook::nvlink_log
