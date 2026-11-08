// [Audio] SoundsInMemory: the game's sound archives kept in memory, so a sound starting during play is not read
// from disk on the game thread (Sound3d::load, ilib/sound3d.cpp).
//   - RESOURCE\WAVE.RES (every sound effect, 80 MB): read at launch;
//   - the mission's voice archives (RESOURCE\S*L1_tactic.RES ...): read when the mission's sound table loads them
//     (ResTable::load_639560, game/unknown_dlgtable.cpp), dropped when the next mission's table starts.
//   - the music and ambience tracks, MUSIC\*.wma (40 MB): read at launch; FFmpeg reads a track that starts from
//     memory instead of opening the file (media/wm_reader.cpp reader_open).
// Each archive (and the music folder) is read whole on a thread of its own; until it is in, its sounds load from
// disk as in the original. Game thread only (the reading threads touch nothing but their own data).
#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace joan::sound_archives {

/// Starts reading RESOURCE\WAVE.RES and the MUSIC folder (host start, after the settings).
void start();

/// The file `path` (the game's relative name, "MUSIC\name.wma", any case) when the music folder is in memory;
/// nullptr otherwise (open it from disk).
std::shared_ptr<const std::vector<std::uint8_t>> music_file(const char* path);

/// A mission's sound table loads the archive `path` (the game's relative path): starts reading it. `fresh` (the
/// table was empty: a new mission) drops the voice archives read for the mission before.
void mission_archive(const char* path, bool fresh);

/// The record `name` of the archive `path` (the game's path and name, as Sound3d::load gets them) when that archive
/// is in memory and holds exactly one record of that name; nullptr otherwise (load it from disk).
const char* find(const char* path, const char* name, std::uint32_t& size);

}  // namespace joan::sound_archives
