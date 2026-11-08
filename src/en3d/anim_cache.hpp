// [Game] AnimCache: the converted bone keys of the game's animations kept in ANIM_TMP\ (game folder).
//
// Loading an animation with a bone tree (.AM2 + .AM3) makes every bone's keys relative to its parent's
// (AnimDef::resolve_bones / make_relative: a matrix inverse and product per key in the exact x87 emulation) - most of
// the time a new unit type takes to appear, a long hitch when a cutscene's camera reaches an army. The original had a
// cache for this (.AMT, AnimDef::load) that the shipped game leaves switched off; this one stores the converted keys
// under a hash of the keys as read (and of the x87 precision in force), so a cached result is only ever used for
// exactly the input it was computed from - the same bits as converting again. Every animation not cached yet is
// converted once while the game starts (build_all); after that no animation is converted during play.
#pragma once

#include <cstdint>

namespace joan::anim_cache {

/// AnimDef::load, after the keys and the bone tree are read: when the cache holds this animation's converted keys,
/// copies them in, marks the bones resolved (all resolve_bones does) and returns true. Otherwise false, with `hash`
/// for store().
bool restore(void* anim_def, const char* directory, const char* name, std::uint64_t& hash);

/// AnimDef::load, after resolve_bones: saves the converted keys (when the cache is on and restore gave a hash).
void store(void* anim_def, const char* directory, const char* name, std::uint64_t hash);

/// The end of Game::load_resources: converts and stores every animation of ANIM, ITEM and UNIT not cached yet.
void build_all();

}  // namespace joan::anim_cache
