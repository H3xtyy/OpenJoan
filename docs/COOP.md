# Co-op

Plan for several players in one campaign, on the `coop` branch. Status: step 2 (spectators) is being built and not yet
proven to work: a debug build takes `--stream PORT` (host: waits for one player, then sends what it records, or what it
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

## Licence question (open)

OpenJoan is GPL-3.0-or-later. Steam's relay is reached only through Valve's Steamworks SDK (`steam_api`), which is
proprietary and not GPL-compatible, so a GPL program may not be distributed linked to it without an additional
permission from the copyright holders (GPL-3.0 section 7). The SDK itself can never be in the repository. Valve's
open-source GameNetworkingSockets (BSD) does not include the relay. This is decided before step 7.

## Testing

Several game copies on one PC: the host replays a reference recording while streaming it, the guests follow over the
network, and the frame hashes must match to the end. Later steps add scripted input for the guests' heroes, so that tests
do not need several people.
