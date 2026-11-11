// SPDX-License-Identifier: GPL-3.0-or-later
// Co-op, a hero per player (docs/COOP.md, step 3): the game controls one "player" through its Player object (0x00a5b9e8)
// and the shared Input object (0x00a516b0). Every other player has an object of each kind of its own, swapped into the
// game's places while the game runs that player's input and tick, so that the game's own control code runs once per player.
#pragma once

namespace joan::coop {

/// CamCtl::camkeys (0x00562f40), after the host's Player::on_key (0x0042e1a0): the same for every other player.
void other_players_input();

/// Camera::update_5634e0, after the host's Player::tick (0x0042f700): the same for every other player.
void other_players_tick();

}  // namespace joan::coop
