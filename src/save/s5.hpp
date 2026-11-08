// Batch S5 of re/a6/SAVES_PLAN.md: field-wise chunks shared by several game files (gamefile.cpp / gamefil2.cpp
// markers, the Profile head of the .SAV step 14 and of SAVE\<name>.dat in gfile3.cpp and profile.cpp, the
// ProfileUnit name key). Each helper writes / reads exactly the chunks of the raw calls it replaces (same count,
// sizes and order, so the File framing is unchanged) and returns what the last of them returned (0 = failed; a
// failing chunk ends the sequence, as the callers' early returns did). [Debug] SaveFieldwise=0 keeps the raw calls.
#pragma once

#include <cstdint>

#include "../original/direct.hpp"
#include "gen/records_s5.hpp"
#include "io.hpp"

namespace joan::save::s5 {

/// The .SAV section marker (GameFile::write_body 0x474ec0 / the reader 0x4735c0) and the "RSav" magic.
inline constexpr std::uint32_t kMarker = 0x2facebee;
inline constexpr std::uint32_t kMagic = 0x76615352;  // "RSav"

/// One 4-byte chunk (File::file_write(&v, 4)).
inline std::int32_t write_u32(orig::File& f, std::uint32_t v, const char* name) {
    if (!fieldwise()) return f.file_write(&v, 4);
    Out o(port(f), 4, name, &v);
    o.u32(v);
    return o.end();
}
/// One 4-byte chunk (File::file_read(&v, 4)): v keeps its bytes where the read leaves them, as the raw read.
inline std::int32_t read_u32(orig::File& f, std::uint32_t& v, const char* name) {
    if (!fieldwise()) return f.file_read(&v, 4);
    In in(port(f), 4, name, &v);
    v = in.u32();
    return in.end();
}

/// Profile + its name again (file_write(p, 0x48); file_write(p, 0x20): Obj::save_483250, Profile::save_4c8810,
/// Profile::save 0x4c8220).
inline std::int32_t write_profile_head(orig::File& f, const orig::Profile& p) {
    if (!fieldwise()) {
        if (f.file_write(&p, 0x48) == 0) return 0;
        return f.file_write(&p, 0x20);
    }
    {
        Out o(f, kProfile, &p);
        put_Profile(o, p);
        if (o.end() == 0) return 0;
    }
    Out o(f, kProfileName, &p);
    put_ProfileName(o, p);
    return o.end();
}
/// file_read(p, 0x48); file_read(p, 0x20) (Obj::load_483430, Profile::load 0x4c8bb0 / 0x4c8490).
inline std::int32_t read_profile_head(orig::File& f, orig::Profile& p) {
    if (!fieldwise()) {
        if (f.file_read(&p, 0x48) == 0) return 0;
        return f.file_read(&p, 0x20);
    }
    {
        In in(f, kProfile, &p);
        get_Profile(in, p);
        if (in.end() == 0) return 0;
    }
    In in(f, kProfileName, &p);
    get_ProfileName(in, p);
    return in.end();
}

/// The first 0x20 bytes of ProfileUnit.name[33] (Key::read 0x4c93e0 and Record::write 0x4c9460 write them, Key::write
/// 0x4c9420 and Obj::load_4c9530 read them; a record cannot cut name[33], so this is an ad-hoc chunk).
inline std::int32_t write_unit_key(orig::File& f, const orig::ProfileUnit& u) {
    if (!fieldwise()) return f.file_write(&u, 0x20);
    Out o(port(f), 0x20, "ProfileUnit.name", &u);
    o.bytes(u.name, 0x20);
    return o.end();
}
inline std::int32_t read_unit_key(orig::File& f, orig::ProfileUnit& u) {
    if (!fieldwise()) return f.file_read(&u, 0x20);
    In in(port(f), 0x20, "ProfileUnit.name", &u);
    in.bytes(u.name, 0x20);
    return in.end();
}

}  // namespace joan::save::s5
