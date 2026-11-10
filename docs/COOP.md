# Co-op

Plan for two players in one campaign, on the `coop` branch. Status: planning; nothing is built yet.

## What co-op means here

Each player controls a different hero of the party (Joan, Jean de Metz, La Hire, ...) at the same time, in the host's
campaign. The host's saves are the campaign's saves. Players join through Steam's relay, so nobody forwards ports.

## How the two games stay the same: lockstep on the replay stream

The game is deterministic: the replay system (`src/runtime/replay`) records every input the game takes from outside
(time, keyboard and mouse, file and window calls, sound) and a hash of the game state at every frame, and a replay that
feeds the same inputs back reproduces the hashes exactly. Co-op uses the same machinery live:

- the host plays normally while recording; the recording is sent to the guest as it is written instead of to a file;
- the guest runs the game from that stream, as a replay that keeps pace with the host;
- the guest's own keyboard and mouse go to the host, which feeds them to the second hero; they enter the stream there, so
  both games see them at the same frame;
- the frame hashes travel with the stream: a difference is a desync, found at the frame it happens.

The guest therefore sees the host's input with the network delay, and the second hero reacts to the guest with one round
trip of delay. That is acceptable for a game of this pace; it can be measured once it runs.

## Problems to solve, in order

1. **Everything the game depends on must be recorded.** Replacing SDL 3.4.16 by 3.4.18 in a game copy changed a replay
   (found 2026-11-09), so something the game reads is not in the recording yet. Two players will also often run
   different systems (Windows and Linux, other SDL versions). This is found and fixed first, with replays across those
   combinations as the test.
2. **Spectator.** The guest follows the host's stream live over the network and shows the host's view. This proves the
   transport and the determinism between two machines without touching game logic.
3. **A second human hero.** The game has one human-controlled unit (`g_player_unit_recno`, read in about 300 places).
   The host must run a second one from the guest's input: walking, fighting, items, skills, and the hero switching keys.
4. **A view per player.** The camera is part of the game state: the ground cells and textures around it are loaded, and
   the camera variables are in the hashed image. Each player's own camera needs the simulation separated from what is only
   drawn: both games load what both cameras need, and the drawing-only state stays out of the stream and the hash.
5. **Menus, pauses, dialogue, cutscenes, mission changes, saves and loads** for two players (who may pause; what a
   cutscene shows the guest; how a load reaches the guest, as a save sent over the stream).
6. **Recovering from a desync** by sending the host's save to the guest and starting the stream again from it.
7. **Steam relay transport.** See the licence question below. Until it is settled, a plain network transport is used for
   development and tests (two game copies on one PC).

## Licence question (open)

OpenJoan is GPL-3.0-or-later. Steam's relay is reached only through Valve's Steamworks SDK (`steam_api`), which is
proprietary and not GPL-compatible, so a GPL program may not be distributed linked to it without an additional
permission from the copyright holders (GPL-3.0 section 7). The SDK itself can never be in the repository. Valve's
open-source GameNetworkingSockets (BSD) does not include the relay. This is decided before step 7.

## Testing

Two game copies on one PC: the host replays a reference recording while streaming it, the guest follows over the
network, and the frame hashes must match to the end. Later steps add scripted input for the second hero, so that tests
do not need two people.
