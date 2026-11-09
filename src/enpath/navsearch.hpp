// SPDX-License-Identifier: GPL-3.0-or-later
// The route search of the new pathfinding (docs/PATHFINDING.md): any-angle search over the navigation mesh, which finds
// the shortest line through it instead of smoothing a chain of polygons. Independent of the game's objects: a NavMesh is plain data, immutable once built, and a search only writes to
// its own NavSearch, so any number of threads can search one mesh. A route depends on nothing but the mesh and the
// request: no clock, no thread timing, no hash order.
#pragma once

#include <cstdint>
#include <vector>

namespace joan::nav {

/// A point in the mesh's plane, world units.
struct Point {
    double x = 0.0;  ///< X, world units
    double y = 0.0;  ///< Y, world units
    friend bool operator==(const Point&, const Point&) = default;
};

/// One convex polygon of the mesh, counter-clockwise. Edge i runs from vertex i to vertex i + 1.
struct Poly {
    std::uint32_t first = 0;   ///< index of the first vertex in NavMesh::corners and of its edge in NavMesh::links
    std::uint32_t count = 0;   ///< number of vertices (and edges)
    std::int32_t weight = 0;   ///< terrain class: a route avoids a higher class before it considers length
    bool blocked = false;      ///< no route may enter this polygon
};

/// The navigation mesh of one block.
struct NavMesh {
    std::vector<Point> points;           ///< the shared vertices
    std::vector<std::uint32_t> corners;  ///< vertex index of each polygon corner
    std::vector<std::int32_t> links;     ///< polygon across each edge, -1 = wall
    std::vector<Poly> polys;             ///< the polygons

    /// The polygon containing `p`, -1 if none (the first in index order, blocked ones excluded).
    std::int32_t locate(Point p) const;
};

/// What a route is asked for.
struct Request {
    std::int32_t start_poly = -1;  ///< polygon the route starts in
    Point from;                    ///< start point
    Point to;                      ///< goal point
    double radius = 0.0;           ///< the unit's radius: portals narrower than 2 * radius are closed
    std::uint32_t budget = 0;      ///< search nodes to expand at most, 0 = no limit
};

/// How a search ended.
enum class Outcome {
    reached,    ///< the route ends at the goal
    partial,    ///< no route (or the budget ran out): the route ends at the closest polygon reached
    no_start,   ///< the start polygon does not exist or is blocked
};

/// A finished route.
struct Route {
    Outcome outcome = Outcome::no_start;  ///< how the search ended
    std::vector<Point> points;            ///< from the start to the end, inclusive
    std::uint32_t expanded = 0;           ///< search nodes expanded
};

/// One search's working memory. Reused between searches, one per thread; never shared.
class NavSearch {
   public:
    /// Searches `mesh` for `request`. The result depends only on the two arguments.
    Route find(const NavMesh& mesh, const Request& request);

   private:
    /// A cone of sight: everything seen from `root` through the interval [right, left] on an edge, entering `poly`.
    struct Node {
        Point root, left, right;
        double g = 0.0;                // length of the route up to the root
        std::int32_t poly = -1;        // polygon the interval leads into
        std::int32_t entry = -1;       // the edge of `poly` it enters through
        std::int32_t worst = 0;        // highest terrain class on the way
        std::int32_t parent = -1;      // index in nodes_
        bool left_corner = false;      // the interval's left end is a corner a route may turn at
        bool right_corner = false;     // the same for the right end
        bool final_node = false;       // ends at the goal
    };
    struct Open {
        std::int32_t worst;
        double f;
        std::uint64_t order;
        std::int32_t node;
    };
    static bool later(const Open& a, const Open& b);

    std::vector<Node> nodes_;
    std::vector<Open> heap_;
    std::vector<char> turnable_;
    std::uint64_t order_ = 0;
};

}  // namespace joan::nav
