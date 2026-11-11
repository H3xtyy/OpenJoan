// SPDX-License-Identifier: GPL-3.0-or-later
// Co-op, a hero per player (docs/COOP.md, step 3): the game controls one "player" through its Player object (0x00a5b9e8)
// and the shared Input object (0x00a516b0). Every other player has an object of each kind of its own, swapped into the
// game's places while the game runs that player's input and tick, so that the game's own control code runs once per player.
#pragma once

namespace joan::coop {

/// CamCtl::camkeys (0x00562f40), after the host's Player::on_key (0x0042e1a0): the same for every other player.
/// `camera` is the game's camera (the CameraFollow the hook runs on); every player has a copy of it.
void other_players_input(void* camera);

/// Camera::update_5634e0, after the host's Player::tick (0x0042f700): the same for every other player.
/// `camera` as for other_players_input.
void other_players_tick(void* camera);

/// Game::reset_three (0x00421b60), after the host's Input::poll (0x00594f90): the same poll for every player on another
/// computer, on the items it sent.
void other_players_poll();

/// Game::run's end of a drawn frame, after the host's Input is told the frame is over (clear_deltas 0x004d8560 and the
/// key modifiers kept as the previous ones): the same for every player on another computer.
void other_players_clear_input();

/// Game::run's end of a drawn frame, after the host's mouse motion is kept as the previous one and zeroed: the same for
/// every player on another computer (else its last motion would act every frame).
void other_players_end_input();

/// True while the game reads another player's keyboard and mouse (Input::poll 0x00594f90 for that player): the poll then
/// leaves this computer's mouse cursor where it is.
bool polling_other_player();

/// What Player::select_hero (0x00432300) does to a hero it takes over: its order is kept for later and cleared, and the
/// orders of its group too (game/unknown_playerctl.cpp).
void take_orders_of(void* unit);

}  // namespace joan::coop
