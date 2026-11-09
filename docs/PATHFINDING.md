# Pathfinding

The replacement for the game's route search, on the `pathfinding` branch. Status: every route request of the game goes
through it, and units walk around other units in their way; the worker threads and the rest of the crowd behaviour are
still to come (see the plan below).

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

**Multithreading without changing results.** `Unit::move_to` (0x00505dd0) is synchronous and callers read the route
immediately, so that contract stays. A route is a pure function of the mesh, the blocked-polygon state and the request,
with no clock or thread timing in it. Workers can therefore compute routes ahead of time:

- every search owns its working memory (`NavSearch`) and the mesh is read-only, so searches run in parallel;
- when many units get one destination (an RTS order), workers compute their routes together;
- a request finds its finished result, waits for one in progress, or computes it itself. The answer is identical in all
  three cases; only the time differs.

Floating point is limited to `+ - * /` and `sqrt` on doubles, so results are the same on every platform.

**Other units.** A route knows only the level, so a unit standing on it is dealt with while walking
(`src/enpath/navsteer.{hpp,cpp}`, `src/game/unit_steer.{hpp,cpp}`). While following a route, a unit heads for a point beside
the nearest unit that it would otherwise walk into, instead of for its waypoint. Units going the same way are left alone so
that a column keeps flowing; units meeting head-on keep to their right. A side is kept while passing, and a side that runs
off the level is not taken.

A tunnel or gate may leave no way round at all. A unit pressed against a non-hostile unit for half a second therefore walks
through friendly units until it is clear of them (at least 1.2 s, then as long as it is still inside one). Hostile units
always collide. This replaces the original's pause for friendly units, and a unit passing through does not count as stuck.
Riders and siege engines do neither yet.

## Plan

1. Search core with tests (done).
2. Build a `NavMesh` from the game's `PathSpace`; keep the blocked flags current (done).
3. Replace `find_path`, keeping the `PathRoute` output the game follows and the partial-route behaviour (done). The old search
   (`find_path2` 0x0066ce30, `PathResult2::smooth_path` 0x0066ece0) is still compiled and is removed once this has been played.
4. Worker pool and shared results for group orders.
5. Crowds, for several dozen units moving together:
   - local avoidance while walking (done for units on foot);
   - units of one order spread over the width of a gap instead of all taking the same line;
   - detecting units that have stopped making progress, and re-routing or yielding, in place of the current stuck fixes.
6. Measure on real levels and tune; test on full missions, since mission scripts depend on units arriving.

Recordings made before this work do not replay with it; that is accepted.
