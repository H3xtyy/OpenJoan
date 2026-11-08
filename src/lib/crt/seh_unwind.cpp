// Original library sources (re/attribution.csv original_file, under lib/): crt/crt0fp.c, crt/intel/exsup.asm,
//   crt/typinfo.cpp.
//
// Batch 117 / task 08: the last MSVC 6 library pieces that are still reachable once lib/crt/eh_support.cpp is installed
// (reachability: scratchpad w117_06/reach.md section 2 families A and F, re-run in w117_08). Read with
// lib/crt/eh_support.cpp (crt/eh: __ArrayUnwind, terminate) and notes CRT_SYSTEM.md (5.1 rules).
//
// Rules followed (NOTE113 / NOTE114 / NOTE115): exact transcription; other CRT functions and Win32 APIs are reached
// through their ORIGINAL addresses (whatever implementation is installed there runs); all state stays at its
// original address with the original layout; entry-patched (library range, no thunks).
//
// Port round P2: the VC6 SEH runtime of this file (exsup.asm / nlgsupp.asm: _global_unwind2 0x7b1c3c and its
// RtlUnwind target 0x7b1c54, __unwind_handler 0x7b1c5c, _local_unwind2 0x7b1c7e, _NLG_Notify 0x7b1d12) is deleted:
// its only caller was _except_handler3 (lib/crt/eh_support.cpp), which no frame uses any more (see there). Their code
// slots are traps; _NLG_Destination 0xa30930 (debugger hook data) has no reader and is no longer written.
//
// Modules:
//   "crt/eh"   the type_info destructors
//     0x007b70fd type_info::~type_info() (thiscall, ret 0): vfptr = 0x009e7054 (the original type_info vftable,
//                .rdata, kept), _lock(_TYPEINFO_LOCK 0x1b), free(_m_data +4) when not NULL (free 0x007b4e1d
//                directly, not operator delete), _unlock(0x1b); eax = _unlock's return (as the original).
//     0x007b7126 type_info::`scalar deleting destructor'(unsigned flags) (thiscall, ret 4): ~type_info (through
//                0x007b70fd), operator delete 0x007b1d2a(this) when flags bit 0 (byte test), eax = this.
//                Only reached through vftable slot 0 of 0x009e7054 (type_info objects of the game's ThrowInfo
//                data, game_b111_01); nothing deletes a type_info in this program.
//   "crt/fpinit" (family K, lib/crt/startup.cpp)
//     0x007c27c0 void __cdecl _fptrap(void): _amsg_exit(_RT_FLOAT 2) (R6002 "floating point not loaded"). The
//                initial value of the _cfltcvt_tab slots 0xa30fa8..0xa30fbc, overwritten by _cfltcvt_init
//                (lib/crt/startup.cpp) before any reader; replaced so the table's initial pointers name our code.
//
// Already replaced elsewhere (task 07, lib/crt/eh_support.cpp): __ArrayUnwind, terminate, the FDIV helpers,
// ctrlevent_capture, the _wenviron path, wctomb / mbtowc.
//
// DirectInput data formats (second part of the task): the arrays at 0x007a78d0 / 0x007a88d0 / 0x007a8950 (.text)
// and their DIDATAFORMATs 0x009e65b4 / 0x009e67bc / 0x009e69c4 are copied in ilib/dinput_formats.hpp (inline
// constexpr, no link dependency); game_b108_01.cpp and game_b102_09.cpp pass those copies to SetDataFormat.
//
// Tests: scratchpad w117_08/h (32-bit Linux, joan.exe mapped at its address): the type_info destructors and _fptrap
// with logged _lock / free / _unlock / delete / _amsg_exit, against the originals. The DirectInput copies are
// compared byte for byte with joan.exe (pointer fields mapped).

#include "../../callconv.hpp"
#include <cstdint>

#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"

namespace {
using u32 = std::uint32_t;

using joan::call::cdecl_call;
// MSVC type_info: vftable, _m_data (the undecorated name cache), then the decorated name.
struct TypeInfoData {
    const void* vftable;  // 0x0
    void* m_data;         // 0x4
    char d_name[1];       // 0x8
};

// ---- other families, called at their original addresses
constexpr fn::Code kLock = fn::crt_lock;            // _lock (crt/mt)
constexpr fn::Code kUnlock = fn::crt_unlock;          // _unlock (crt/mt)
constexpr fn::Code kFree = fn::free;            // free (crt/heap)
constexpr fn::Code kOperatorDelete = fn::op_delete;  // operator delete (crt/heap)
constexpr fn::Code kAmsgExit = fn::crt_amsg_exit;        // _amsg_exit (crt/exit)
// ---- this file's functions (called through the original addresses)
constexpr fn::Code kTypeInfoDtor = fn::crt_type_info_dtor;
// ---- state
constexpr img::Addr kTypeInfoVftable = d::kTypeInfoVftable;  // const type_info::`vftable' (.rdata)
constexpr int kTypeInfoLock = 0x1b;           // _TYPEINFO_LOCK
constexpr int kRtFloat = 2;                   // _RT_FLOAT

// =====================================================================================================
// crt/eh: type_info destructors (thiscall)
// =====================================================================================================

struct TypeInfo {
    // 0x007b70fd type_info::~type_info(). ret 0, eax = _unlock's.
    u32 JOAN_THISCALL destroy() {
        reinterpret_cast<TypeInfoData*>(this)->vftable = img::obj(kTypeInfoVftable);
        cdecl_call(kLock, kTypeInfoLock);
        void* const name = reinterpret_cast<TypeInfoData*>(this)->m_data;  // undecorated name cache
        if (name != nullptr) cdecl_call(kFree, name);
        return cdecl_call<u32>(kUnlock, kTypeInfoLock);
    }
    // 0x007b7126 type_info::`scalar deleting destructor'(flags). ret 4.
    void* JOAN_THISCALL scalar_delete(u32 flags) {
        reinterpret_cast<u32(JOAN_THISCALL*)(void*)>(fn::code_value(kTypeInfoDtor))(this);
        if ((flags & 1) != 0) cdecl_call(kOperatorDelete, static_cast<void*>(this));
        return this;
    }
};

// =====================================================================================================
// crt/fpinit
// =====================================================================================================

// 0x007c27c0 _fptrap()
void JOAN_CDECL fptrap() { cdecl_call(kAmsgExit, kRtFloat); }

#define EH(name, addr, fn) JOAN_REPLACE("crt/eh", name, addr, fn)
EH("crt::type_info::~type_info", 0x007b70fd, &TypeInfo::destroy);
EH("crt::type_info::scalar_deleting_destructor", 0x007b7126, &TypeInfo::scalar_delete);
#undef EH
JOAN_REPLACE("crt/fpinit", "crt::_fptrap", 0x007c27c0, &fptrap);

}  // namespace
