// SPDX-License-Identifier: GPL-3.0-or-later
// Co-op (docs/COOP.md): every game draws the view of every player, so that what drawing changes in the game (cells
// streamed in, camera motions advanced) is the same everywhere; each game shows only its own player's view. The views
// of the others are drawn into a scratch back buffer of the Direct3D 8 device (gl/device.cpp).
#pragma once

namespace joan::gld {

/// While on, the device's back buffer and its depth are scratch surfaces: what is drawn is not shown. Off: the real
/// back buffer again. Call between the views of a frame, outside a game render target.
void set_hidden_view(bool on);

}  // namespace joan::gld
