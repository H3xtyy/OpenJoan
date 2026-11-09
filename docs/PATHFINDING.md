# Pathfinding

The replacement for the game's route search, on the `pathfinding` branch. Status: every route request of the game goes
through it, and units walk around other units in their way; the rest of the crowd behaviour is still to come (see the plan
below).

## What was wrong

The original search (`PathSpace::find_path` 0x0066cb40, `docs/re/functions/pathfind.md`) runs A* over polygon centres,
with a bonus for large polygons, usually a 100-node budget, and closed polygons that are never improved. Its smoothing
looks only 8 portals ahead. Narrow gaps are decided per polygon by a rough length test. Dynamic obstacles and other
units play no part in it, so groups push into each other at chokepoints.

## Design

**Exact routes.** `src/enpath/navsearch.{hpp,cpp}` is an any-angle search over the same navigation mesh (`.pfd`): each
search node is a cone of sight from a root point through part of a polygon edge. It finds the shortest line through the
mesh, and bends only at the corners of walls, blocked polygons and terrain class changes. Polygon weights stay a priority:
a route avoids a higher class before it considers length. Where an edge ends at an obstacle, that end is moved inwards by
the unit's radius; a gap narrower than the unit is still crossed, at its middle, because missions depend on units getting
through. Units of at least 200 world units keep out of polygons the loader marked narrow, as in the original.

**The route the game receives.** `src/enpath/navgame.cpp` builds a `NavMesh` from a `PathSpace` on the first request and
reads the current blocked flags on every request. The route it writes has one point per polygon edge crossed, so each leg
stays inside one convex polygon and the height of every point comes from the edge it lies on. The unit's place in its
formation moves each point along its edge, within the usable part: this is what spreads the units of one order across a
gap. When the goal cannot be reached, the route ends at the point of the visited polygons closest to it.

The polygons of the shipped levels are small enough for this to be cheap. Over 45,600 random requests on all 38 meshes (up
to 3,875 polygons), a request takes 0.1 ms at the median, 4.4 ms at the 99th percentile and 58 ms at worst, the worst being
a goal that cannot be reached, which visits a whole level. A goal inside one of the handful of non-convex polygons is
treated as unreachable, and the route stops beside it.

**One thread, and no repeated work.** `Unit::move_to` (0x00505dd0) is synchronous and callers read the route immediately.
A unit walking along asks for its route again every few frames: over 1,000 requests a second were counted in play, against a
few dozen orders in a whole session. A search therefore starts from the middle of a 16-unit square around the unit, which
makes those requests the same search, and a finished route is remembered until a polygon opens or closes. The piece of walk
from the unit to its first edge stays inside the unit's polygon. Over 90% of requests were answered from memory, leaving a
few dozen small searches a second. The debug build's log counts both: `Nav::route_repeated` and `Nav::route_searched`.

Worker threads were tried and removed. Callers need the route at once, so a thread can only help with a route known in
advance, and almost none are: in play only 27 of 2,787 new searches used a route prepared ahead of time.

**Formations.** A follower close to its leader copies the leader's route, shifted by the distance between them
(`route_around` 0x006ac050). It joins that route on the stretch it stands beside. The new routes are often straight, so this
test looks at the stretch itself and not at the infinite line through it, which would match the first stretch every time.

**Other units.** A route knows only the level, so a unit standing on it is dealt with while walking
(`src/enpath/navsteer.{hpp,cpp}`, `src/game/unit_steer.{hpp,cpp}`). While following a route, a unit heads for a point beside
the nearest unit that it would otherwise walk into, instead of for its waypoint. Units going the same way are left alone so
that a column keeps flowing; units meeting head-on keep to their right. A side is kept while passing, and a side that runs
off the level is not taken.

A tunnel or gate may leave no way round at all. A unit pressed against a non-hostile unit for half a second therefore walks
through friendly units until it is clear of them (at least 1.2 s, then as long as it is still inside one). Hostile units
always collide, and nobody walks through the player. Two friendly bodies already inside each other do not collide, so
that neither can be trapped when one stops inside the other. This replaces the original's pause for friendly units, and a unit passing through does not count as stuck.
Riders and siege engines do neither yet.

A unit in no formation gets a line of its own from its id, used only at corners and in gaps narrower than 400 units, so that
a crowd does not funnel to one point; on open ground it keeps the straight line.

The mesh does not know everything: a model's wall may stand on ground the mesh calls free. A unit that has come no closer
to its waypoint for 0.45 s steps aside for 0.35 s, one way and then the other, up to four times. Only then does the older
check (`game/unit_stuck.hpp`, a unit with no progress for a second ends its route) apply. That check stays, because it deals
with exactly this case when stepping aside does not help.

## Plan

1. Search core with tests (done).
2. Build a `NavMesh` from the game's `PathSpace`; keep the blocked flags current (done).
3. Replace `find_path`, keeping the `PathRoute` output the game follows and the partial-route behaviour (done).
4. Remembering routes so that repeated requests cost nothing (done).
5. Crowds, for several dozen units moving together:
   - local avoidance while walking (done for units on foot);
   - units spread over the width of a gap instead of all taking the same line (done);
   - stepping aside from what the mesh does not know, before the existing stuck check applies (done).
6. Measure on real levels and tune; test on full missions, since mission scripts depend on units arriving.

Recordings made before this work do not replay with it; that is accepted.
