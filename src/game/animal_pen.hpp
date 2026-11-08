// SPDX-License-Identifier: GPL-3.0-or-later
// [Fixes] AnimalPen: animals that stand outside the navigation mesh graze instead of walking on the spot.
//
// A body moves only while it stands on a polygon of the scene's navigation mesh (`.pfd`): FormMove::step_6a45a0
// (0x006a45a0) undoes every step that ends on no polygon (BodyState::zone 0). The level designers left the fenced pens
// out of the mesh, so the pigs placed in them (mission 5, map\scenario5\S5L1_7.uni: 12 pigs, all outside the mesh of
// S5L1_7.pfd) never move, while Unit::wander (0x004f7bf0) keeps giving them routes and the walk animation (recording
// ref-x86-2026-11-5n-pigs: units 1342..1353 at the same position for all 8,779 frames). Letting them step is no
// option: off the mesh nothing collides, the fences included (owner test of such a version, 2026-11-05: pigs walked
// through the fence).
//
// With the fix, the step that the physics undoes for having no ground marks the body (mark_off_mesh, called by
// FormMove::step_6a45a0). Unit::move_update (0x0052daa0) then ends a marked animal's route and gives it no new wander
// route, so it keeps its idle animations. BodyState::zone 0 alone does not tell: an animal on the mesh also reads 0 for
// a while when its cell is not being simulated. A body seen on the mesh again loses the mark.
//
// The marks are a table of ours keyed by the body record (not saved: after a load an animal takes one refused step
// before it settles); every value comes from game state, so a replay sees the same decisions.
#pragma once

namespace joan::animal_pen {

/// FormMove::step_6a45a0 (0x006a45a0), [Fixes] AnimalPen: the step of body record `body`, owned by `owner` (the actor's
/// owner), was undone because it ends on no polygon of the navigation mesh.
void mark_off_mesh(const void* body, const void* owner);

/// Unit::move_update (0x0052daa0), [Fixes] AnimalPen: whether `body`, the body of unit `owner`, was marked by
/// mark_off_mesh and has stood on no polygon since. `on_mesh` is whether it stands on one now (it clears the mark).
bool off_mesh(const void* body, const void* owner, bool on_mesh);

}  // namespace joan::animal_pen
