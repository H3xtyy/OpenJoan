# Co-op

Plan for several players in one campaign, on the `coop` branch. Status: step 2 (spectators) works: a player followed the
host's replay of the mission 1 reference over the network to its end, with the same calls and the same state as a replay of
the file, checked word by word on every frame. A debug build takes `--stream PORT` (host: waits for one player, then sends what it records, or what it
replays, as it goes) and `--follow HOST:PORT` (player: replays the host's stream as it arrives, checking every frame hash).
The connection is plain TCP (`src/platform/net.hpp`) until the Steam transport.

## What co-op means here

Each player controls a different hero of the party (Joan, Jean de Metz, La Hire, Alencon, ...) at the same time, in the
host's campaign, so there are as many players as heroes in the party; the party changes from mission to mission, so a
player without a hero in the current mission watches or commands the troops until one is free. The host's saves are the
campaign's saves. Players join through Steam's relay, so nobody forwards ports.

The story is one for everybody: there is a single campaign, so its scripts, objectives, dialogue and choices are the same
for all players, and a cutscene plays on every player's screen at the same moment, with the camera the script gives it.

## How the games stay the same: lockstep on the replay stream

The game is deterministic: the replay system (`src/runtime/replay`) records every input the game takes from outside
(time, keyboard and mouse, file and window calls, sound) and a hash of the game state at every frame, and a replay that
feeds the same inputs back reproduces the hashes exactly. Co-op uses the same machinery live:

- the host plays normally while recording; the recording is sent to every guest as it is written instead of to a file;
- each guest runs the game from that stream, as a replay that keeps pace with the host;
- each guest's own keyboard and mouse go to the host, which feeds them to that guest's hero; they enter the stream there,
  so every game sees them at the same frame;
- the frame hashes travel with the stream: a difference is a desync, found at the frame it happens.

A guest therefore sees the host's input with the network delay, and its hero reacts with one round trip of delay. That is
acceptable for a game of this pace; it can be measured once it runs. The host sends one stream to every guest and takes
one input stream from each, so the number of players changes the host's bandwidth, not the design.

## Problems to solve, in order

1. **Everything the game depends on must be recorded.** Replacing SDL 3.4.16 by 3.4.18 in a game copy changed a replay
   (found 2026-11-09), so something the game reads is not in the recording yet. Players will also often run
   different systems (Windows and Linux, other SDL versions). This is found and fixed first, with replays across those
   combinations as the test.
2. **Spectators.** Guests follow the host's stream live over the network and show the host's view. This proves the
   transport and the determinism between machines without touching game logic.
3. **A human hero per player.** The game has one human-controlled unit (`g_player_unit_recno`, read in about 300 places).
   The host must run one per player from that player's input: walking, fighting, items, skills, and who may switch to which
   hero.
4. **A view per player.** The camera is part of the game state: the ground cells and textures around it are loaded, and
   the camera variables are in the hashed image. Each player's own camera needs the simulation separated from what is only
   drawn: every game loads what all the cameras need, and the drawing-only state stays out of the stream and the hash.
5. **Menus, pauses, dialogue, cutscenes, mission changes, saves and loads** for several players (who may pause; a
   cutscene shows every player the script's camera, and only the host may skip it or a line of dialogue; how a load reaches them, as a save sent over the stream; joining a running game the same way).
6. **Recovering from a desync** by sending the host's save to that guest and starting its stream again from it.
7. **Steam relay transport.** See the licence question below. Until it is settled, a plain network transport is used for
   development and tests (several game copies on one PC).

The release build has no record and replay (`build_flavor.hpp`): co-op needs that machinery in the builds players run,
which is decided when the spectator works.

## A hero per player (step 3): design

What the game has (read from the code, 2026-11-11):

- `d::g_player_unit_recno` (0x00a5b9e8) is not only a unit number: it is the address of the one `jo::Player` object
  (0x126e bytes, `src/original/gen/player.hpp`), whose first field is the controlled unit. All of the control state is in
  it: the input mode (`mode` = `d::g_control_mode`, keyboard / pad / keyboard and mouse / RTS), riding and mounting, combo
  timing (`press_time`, `order_state`), the attack target, the RTS selection and groups, the camera's follow mode.
- Each frame, `Game::process_input` (0x0052f930) passes the input to `Player::on_key` (0x0042e1a0), which dispatches on the
  mode (`PlayerCtl::update_430480` for keyboard and mouse, `Player::handle_input` 0x00440b30 for RTS), and the camera
  update runs `Player::tick` (0x0042f700). The handlers find the hero through the object, not through the global, and read
  the shared input globals (`g_mouse_delta_*`, `g_key`, ...) filled by `Input::poll` (0x00594f90).
- A hero is driven by a human when `Unit::layer` is 0, by its AI when it is 1; hero switching (`Player::select_hero`
  0x00432300) moves the object to another unit and flips the layers.
- About 290 places read the global: some 35 control the hero, about 105 draw the camera, HUD and menus for it, about 125
  are game rules that treat "the player's unit" specially (hostility, damage credit, AI target choice, script functions,
  triggers), the rest debug and cheats.

The plan: one `Player` per human. The host keeps the game's own object for player 1 and one copy per other player, each on
its own hero. Each frame, for every other player, the host swaps that player's object and that player's input into the
game's places, runs `on_key` and `tick` for it, and swaps them back. The game code stays as it is: it controls "the
player" as before, once per player. Every player's hero has layer 0, so the rules that check the layer already treat all
of them as human. The rules that compare a unit with the global (the 125) are gone through one by one: for most, "a unit
of any player" is the right reading. A player's input reaches the host as an event of the stream, so every game applies it
at the same frame.

Tested on one PC (2026-11-11, `[Debug] CoopTestPlayer=1`, `src/game/coop_players.cpp`): a scripted second player took over
a second hero of the party in a mission and walked, turned and attacked while the host played Joan normally; the host's
camera stayed with Joan. Two things were needed: the flags the mission's scripts set on "the player" (blocked during
cutscenes and dialogue, and its two neighbours) are copied from the host's object to every other player each frame, and a
hero taken over has its order cleared as `Player::select_hero` does.

A player on another computer (written 2026-11-11, `src/game/coop_players.cpp`; not yet tested with a person at each
window):

- The player's game follows the host's stream as a spectator does, and sends every key, mouse movement, wheel step and
  button of its computer to the host while its window has the focus (`platform::input::forward`, then
  `replay::coop_send`). The replayed game itself never reads the devices.
- The host keeps those items per player. In `CamCtl::camkeys`, right before running that player's `Player::on_key`, it
  runs the game's own `Input::poll` (0x00594f90) with the player's `Input` object in place and the received items as its
  devices (`platform::input::read_from`). Key repeats, double taps and button timing then work as for the host.
- The poll's device reads are recorded like any other, so the player's game, and any replay of the file, runs the same
  poll with the same items in the same frame. The other player's mouse does not move the host's cursor.
- The recording's header holds the number of players on other computers (flags bits 20..23). Every game that replays
  it starts the same remote player on the same hero in the first frame of a mission.

For now the player sees the host's view (step 4 gives each player its own).

## Licence question (open)

OpenJoan is GPL-3.0-or-later. Steam's relay is reached only through Valve's Steamworks SDK (`steam_api`), which is
proprietary and not GPL-compatible, so a GPL program may not be distributed linked to it without an additional
permission from the copyright holders (GPL-3.0 section 7). The SDK itself can never be in the repository. Valve's
open-source GameNetworkingSockets (BSD) does not include the relay. This is decided before step 7.

## Testing

A player on another computer: the host records with `--stream PORT`, the player starts with `--follow HOST:PORT` (both
debug builds with `[Debug]` record / replay available). In a mission the player's keyboard and mouse drive the second
hero of the party while the window of the player's game has the focus.

Several game copies on one PC: the host replays a reference recording while streaming it, the guests follow over the
network, and the frame hashes must match to the end. Later steps add scripted input for the guests' heroes, so that tests
do not need several people.
