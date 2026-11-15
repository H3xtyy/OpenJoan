// SPDX-License-Identifier: GPL-3.0-or-later
// Co-op (docs/COOP.md): a player's game hears from its own player's camera. The game keeps one listener (its position
// is read by the AI, so it stays the host's in every game); in a player's game only what reaches OpenAL is moved.
#pragma once

namespace joan::openal {

/// While on, OpenAL's listener is `position` with the orientation `at` / `up` (the game's coordinates, as
/// Listener::set_orientation 0x005a0050 and set_pos 0x0059ff70 pass them) whatever the game sets, and a source the
/// game places on its own listener (the sounds it plays "at the player") is placed on this one. Off: the game's again.
void set_own_listener(bool on, const float position[3], const float at[3], const float up[3]);

}  // namespace joan::openal
