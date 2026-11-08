// The sectors' actor lists (SceneS nodes_tail / nodes_head, linked through Actor scene_prev / scene_next).
#pragma once

#include <cstdint>
#include <vector>

namespace joan::en3d {

/// Changes whenever any sector's actor list is edited (Scene::attach / detach, SceneGroup::release_ce, the world
/// sectors' attach, the save loader). SceneS::draw_lists keeps a sector's layer 2 / 3 actors from its layer 1 walk
/// only while this stays the same.
inline std::uint32_t g_scene_list_edits = 0;

// --- [Debug] UnitIndex: the unit actors (kind 0xb) of a sector, in the order the game's queries walk its list ---------
// The queries "which units are near" (Unit::recruit_nearby, Unit::find_target) walk every actor of the sectors in a
// box - scenery and thousands of grass patches too - to look at the units among them. The index keeps each sector's
// units in that walk order (nodes_tail, then scene_prev) and is rebuilt after anything that can change it: every edit
// of the list (Scene::attach / detach, SceneGroup::release_ce, the save loader's sector reset) and every write of an
// actor's kind - all in this code (no original function body runs). A query that edits a list while it goes through
// the units goes on with the walk itself from that actor, as the original.
// [Debug] UnitIndexCheck=N compares every Nth lookup with the walk: on a difference openjoan.log says so, the walk's
// units are used and the index is switched off for the session.

/// Bumped by unit_index_touch: a query compares it before / after the work it does for a unit.
inline std::uint32_t g_unit_list_edits = 0;
/// `sector`'s actor list changed, or the kind of one of its actors (nullptr: nothing).
void unit_index_touch(const void* sector);
/// The kind of `actor` (an Actor) is about to change / changed: its sector's units are rebuilt.
void unit_index_actor_kind(const void* actor);
/// Every sector's units are rebuilt (sectors reset where the sector is not known).
void unit_index_touch_all();
/// The unit actors of `sector` (a SceneS) in walk order, or nullptr: walk the list (index off).
const std::vector<std::uint8_t*>* sector_units(void* sector);

}  // namespace joan::en3d
