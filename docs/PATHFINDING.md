# Pathfinding

The replacement for the game's route search, on the `pathfinding` branch. Status: the search core exists and is tested on
its own; it is not connected to the game yet.

## What was wrong

The original search (`PathSpace::find_path` 0x0066cb40, `docs/re/functions/pathfind.md`) runs A* over polygon centres,
with a bonus for large polygons, usually a 100-node budget, and closed polygons that are never improved. Its smoothing
looks only 8 portals ahead. Narrow gaps are decided per polygon by a rough length test. Dynamic obstacles and other
units play no part in it, so groups push into each other at chokepoints.

## Design

**Exact routes.** `src/enpath/navsearch.{hpp,cpp}` is an any-angle search over the same navigation mesh (`.pfd`): each
search node is a cone of sight from a root point through part of a polygon edge. It finds the shortest line through the
mesh, and bends only at the corners of walls, blocked polygons, gaps too narrow for the unit, and terrain class changes.
Polygon weights stay a priority: a route avoids a higher class before it considers length. A portal narrower than the
unit is closed, and portal ends are moved inwards by the unit's radius.

**Multithreading without changing results.** `Unit::move_to` (0x00505dd0) is synchronous and callers read the route
immediately, so that contract stays. A route is a pure function of the mesh, the blocked-polygon state and the request,
with no clock or thread timing in it. Workers can therefore compute routes ahead of time:

- every search owns its working memory (`NavSearch`) and the mesh is read-only, so searches run in parallel;
- when many units get one destination (an RTS order), workers compute their routes together;
- a request finds its finished result, waits for one in progress, or computes it itself. The answer is identical in all
  three cases; only the time differs.

Floating point is limited to `+ - * /` and `sqrt` on doubles, so results are the same on every platform.

## Plan

1. Search core with tests (done).
2. Build a `NavMesh` from the game's `PathSpace` when a level loads; keep the blocked flags current.
3. Replace `find_path`, keeping the `PathRoute` output the game follows and the partial-route behaviour.
4. Worker pool and shared results for group orders.
5. Crowds, for several dozen units moving together:
   - local avoidance while walking (units steer around each other instead of pushing), on top of the route;
   - units of one order spread over the width of a gap instead of all taking the same line;
   - detecting units that have stopped making progress, and re-routing or yielding, in place of the current stuck fixes.
6. Measure on real levels and tune; test on full missions, since mission scripts depend on units arriving.

Recordings made before this work do not replay with it; that is accepted.
