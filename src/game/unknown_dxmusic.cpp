// game/unknown_dxmusic.cpp: functions whose original source file is unknown (formerly game/_unplaced/dxmusic.cpp;
// re/attribution.csv: between ww/ilib/db.cpp and ww/ilib/dynarr.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.
//
// The game's DirectMusic wrapper (Music at 0x00a3ce50, segments, scripts). Its only initialisation, Dx::create
// 0x00582d20, has no caller, so no DirectMusic object is ever created: `loader`, `performance` stay null and
// `initialised` 0. Cleanup C1 deleted everything only reachable through such an object (Dx::create, the loaders,
// play / stop / download, the COM wrappers 0x00584310 / 0x00584370); what stays is what live code still calls, reduced
// to what it does with no DirectMusic object:
//   0x00582c20 ComInit::ctor (static initialiser), 0x00582c60 DSound::shutdown (atexit), 0x00583670 set_volume
//   (sndplay.cpp: E_UNEXPECTED, never initialised), 0x00583aa0 / 0x00583c40 the global segment's constructor /
//   destructor (globals.cpp), 0x00583b10 / 0x00584170 the scalar destructors (stored in the vtables 0x009d88a4 /
//   0x009d88a8) with 0x00584210.
// Cleanup C2: the COM apartment calls went too - ComInit::ctor's CoInitialize and DSound::shutdown's CoUninitialize
// only opened / closed COM for DirectMusic (answered by platform/system.cpp's "no COM" stand-ins, whose per-thread
// count nothing reads: the other callers, uti3d/video.cpp and videocap.cpp, discard the result) - and the
// MusicScript destructor's loader / performance releases (no MusicScript is ever made: vtable 0x009d88a8 is only
// stored by that destructor itself, and its only instance pointer 0x00a5d878 is never set) - and 0x005842b0
// MusicScript::call_routine with its ANSI -> wide helpers 0x00582ee0 / 0x00582f20: a COM call on that never-made
// object, whose only caller was the dead Music::play_event 0x004a9d40 (misc_4a9d40.cpp, deleted).

#include <cstdint>
#include "../original/helpers.hpp"

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
namespace {
namespace game_b95_05 {

struct H {
    // 0x00582c20 (the original also called CoInitialize(NULL) for DirectMusic; see the top of the file)
    void* com_ctor() {
        jo::self<jo::Music>(this)->loader = nullptr;
        jo::self<jo::Music>(this)->performance = nullptr;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("ComInit::ctor", 0x00582c20, &H::com_ctor);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
namespace {
namespace game_b97_05 {

JOAN_COUNTER(c_ds_shutdown, "DSound::shutdown");

struct H {
    // 0x00582c60: the original released `loader` / `performance` - always null - and called CoUninitialize (see the
    // top of the file).
    void ds_shutdown() { c_ds_shutdown.hit(); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("DSound::shutdown", 0x00582c60, &H::ds_shutdown);
#undef B

}  // namespace game_b97_05
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
namespace {
namespace game_b100_10 {

JOAN_COUNTER(c_call14, "Stream::call14");

struct H {
    void* ctor_583aa0() {  // 0x00583aa0
        jo::self<jo::MediaSample>(this)->performance = nullptr;
        jo::self<jo::MediaSample>(this)->loader = nullptr;
        jo::self<jo::MediaSample>(this)->segment = nullptr;
        jo::self<jo::MediaSample>(this)->segment_state = nullptr;
        jo::self<jo::MediaSample>(this)->downloaded = 0;
        jo::self<jo::MediaSample>(this)->vtable = img::obj(d::vt_Obj_9d88a4);
        return this;
    }

    // ------------------------------------------------------------------------ 0x00583670
    // The original asked the default audio path (0x00582fa0) of an initialised DirectMusic object for its volume;
    // never initialised: E_UNEXPECTED.
    u32 call14(u32, u32) {
        c_call14.hit();
        return 0x8000ffffu;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("Stream::call14", 0x00583670, &H::call14);
B("Obj::ctor_583aa0", 0x00583aa0, &H::ctor_583aa0);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b103_08.cpp (game/b103_08)
namespace {
namespace game_b103_08 {

using joan::orig::helpers::op_delete;

struct H {
    H* scalar_dtor_583b10(u32 flags) {
        joan::orig::as_music_segment(this)->destruct();  // 0x00583c40
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
    H* scalar_dtor_584170(u32 flags) {
        joan::orig::as_music_script(this)->destruct();  // 0x00584210
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("Obj::scalar_dtor_583b10", 0x00583b10, &H::scalar_dtor_583b10);
B("DP::scalar_dtor_584170", 0x00584170, &H::scalar_dtor_584170);
#undef B

}  // namespace game_b103_08
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
namespace {
namespace game_b92_04 {

JOAN_COUNTER(c_media_dtor, "MediaSample::~MediaSample");

struct H {
    // ------------------------------------------------------------------------ 0x00583c40
    // The original first unloaded / stopped / released the segment when `performance` was set - only the dead
    // MediaSample::MediaSample 0x00583b60 set it.
    void media_dtor() {
        c_media_dtor.hit();
        jo::self<jo::MediaSample>(this)->vtable = img::obj(d::vt_Obj_9d88a4);
        jo::self<jo::MediaSample>(this)->segment = nullptr;
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("MediaSample::~MediaSample", 0x00583c40, &H::media_dtor);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
namespace {
namespace game_b96_05 {

JOAN_COUNTER(c_dtor_584210, "Obj::dtor_584210");

struct H {
    // 0x00584210: the original first released `loader` (IDirectMusicLoader::ReleaseObject(performance)) and
    // `performance` (Release) when set - never: no MusicScript is ever made (see the top of the file).
    void dtor_584210() {
        c_dtor_584210.hit();
        jo::self<jo::DP>(this)->vtable = img::obj(d::vt_DP);
        jo::self<jo::DP>(this)->loader = nullptr;
        jo::self<jo::DP>(this)->performance = nullptr;
        jo::self<jo::DP>(this)->script = nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("Obj::dtor_584210", 0x00584210, &H::dtor_584210);
#undef B

}  // namespace game_b96_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::DirectMusic::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_05::H::com_ctor>(this)); }  // 0x582c20
auto joan::orig::DirectMusic::set_volume(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_10::H::call14>(this, a0, a1)); }  // 0x583670
auto joan::orig::MusicSegment::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_10::H::ctor_583aa0>(this)); }  // 0x583aa0
auto joan::orig::MusicSegment::destruct() -> void { joan::bind::member<&::game_b92_04::H::media_dtor>(this); }  // 0x583c40
auto joan::orig::MusicScript::destruct() -> void { joan::bind::member<&::game_b96_05::H::dtor_584210>(this); }  // 0x584210
