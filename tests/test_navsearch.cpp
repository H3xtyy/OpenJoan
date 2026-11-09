// SPDX-License-Identifier: GPL-3.0-or-later
// The route search of src/enpath/navsearch.hpp on small meshes built by hand.
#include <cmath>
#include <cstdio>
#include <thread>
#include <vector>

#include "../src/enpath/navsearch.hpp"

using namespace joan::nav;

namespace {

int g_failures = 0;
void check(bool ok, const char* what) {
    if (!ok) {
        std::printf("FAILED: %s\n", what);
        ++g_failures;
    }
}

// A grid of unit-sized squares of `size`; `open(x, y)` says which cells exist.
template <class Open>
NavMesh grid(int width, int height, double size, Open open, double door = 0.0) {
    NavMesh m;
    const auto point = [&](int x, int y) { return static_cast<std::uint32_t>(y * (width + 1) + x); };
    for (int y = 0; y <= height; ++y)
        for (int x = 0; x <= width; ++x) m.points.push_back({x * size, y * size});
    std::vector<std::int32_t> index(static_cast<std::size_t>(width * height), -1);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            if (open(x, y)) {
                index[static_cast<std::size_t>(y * width + x)] = static_cast<std::int32_t>(m.polys.size());
                m.polys.push_back({static_cast<std::uint32_t>(m.corners.size()), 4, 0, false});
                for (const std::uint32_t c : {point(x, y), point(x + 1, y), point(x + 1, y + 1), point(x, y + 1)}) m.corners.push_back(c);
            }
    (void)door;
    const auto at = [&](int x, int y) { return x < 0 || y < 0 || x >= width || y >= height ? -1 : index[static_cast<std::size_t>(y * width + x)]; };
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            if (at(x, y) >= 0)
                for (const std::int32_t n : {at(x, y - 1), at(x + 1, y), at(x, y + 1), at(x - 1, y)}) m.links.push_back(n);
    return m;
}

double length(const Route& r) {
    double total = 0.0;
    for (std::size_t i = 1; i < r.points.size(); ++i) total += std::hypot(r.points[i].x - r.points[i - 1].x, r.points[i].y - r.points[i - 1].y);
    return total;
}

Route find(const NavMesh& m, Point a, Point b, double radius = 0.0, std::uint32_t budget = 0) {
    NavSearch search;
    return search.find(m, {m.locate(a), a, b, radius, budget});
}

}  // namespace

int main() {
    {  // an open room: one straight line
        const NavMesh m = grid(6, 6, 10.0, [](int, int) { return true; });
        const Route r = find(m, {5, 5}, {55, 45});
        check(r.outcome == Outcome::reached, "open room reaches the goal");
        check(r.points.size() == 2, "open room is a straight line");
    }
    {  // a wall with a gap at the top: the route turns only at the gap's corner
        const auto open = [](int x, int y) { return !(x == 3 && y < 5); };
        const NavMesh m = grid(7, 6, 10.0, open);
        const Route r = find(m, {5, 5}, {65, 5});
        check(r.outcome == Outcome::reached, "wall reaches the goal");
        check(r.points.size() == 4, "wall route turns at the two corners");
        const double best = 2.0 * std::hypot(25.0, 45.0) + 10.0;
        check(std::fabs(length(r) - best) < 1e-9, "wall route is the shortest");
    }
    {  // a one-cell corridor: a gap too narrow for the radius is crossed at its middle; a polygon marked narrow can be avoided
        const auto open = [](int x, int y) { return y == 1 || x == 2; };
        NavMesh m = grid(5, 3, 10.0, open);
        check(find(m, {5, 15}, {45, 15}, 4.0).outcome == Outcome::reached, "a small unit fits the corridor");
        const Route tight = find(m, {5, 15}, {45, 15}, 6.0);
        check(tight.outcome == Outcome::reached, "a gap narrower than the unit stays open");
        check(!tight.crossings.empty(), "the route lists its crossings");
        bool centred = true;
        for (const Crossing& c : tight.crossings) centred = centred && std::fabs(c.at.y - 15.0) < 0.5;
        check(centred, "a narrow gap is crossed at its middle");
        for (Poly& p : m.polys) p.narrow = true;
        NavSearch search;
        Request request{m.locate({5, 15}), {5, 15}, {45, 15}, 6.0, 0};
        request.avoid_narrow = true;
        check(search.find(m, request).outcome == Outcome::partial, "a large unit avoids narrow polygons");
    }
    {  // clearance is kept from walls, not from the open vertices in the middle of a room
        const auto open = [](int x, int y) { return !(x == 3 && y < 5); };
        const NavMesh m = grid(7, 6, 10.0, open);
        const Route r = find(m, {5, 5}, {65, 5}, 2.0);
        check(r.outcome == Outcome::reached, "with a radius the wall route is found");
        for (const Point& p : r.points) check(p.y <= 52.0 + 1e-9, "the route stays below the top wall");
        bool away = false;
        for (const Point& p : r.points) away = away || std::fabs(p.y - 52.0) < 1e-9;
        check(away, "the route passes the wall end at the radius");
        const NavMesh room = grid(6, 6, 10.0, [](int, int) { return true; });
        check(find(room, {5, 5}, {55, 45}, 3.0).points.size() == 2, "an open room stays one straight line with a radius");
    }
    {  // every crossing lies on its edge, in walking order, and the legs between them stay inside the mesh
        const auto open = [](int x, int y) { return !(x == 2 && y < 5) && !(x == 5 && y > 0); };
        const NavMesh m = grid(8, 6, 10.0, open);
        const Route r = find(m, {5, 5}, {75, 55}, 1.0);
        check(r.crossings.size() >= 7, "a long route has its crossings");
        Point previous{5, 5};
        double walked = 0.0;
        for (const Crossing& c : r.crossings) {
            check(std::fabs(std::hypot(c.at.x - c.left.x, c.at.y - c.left.y) + std::hypot(c.right.x - c.at.x, c.right.y - c.at.y) - std::hypot(c.right.x - c.left.x, c.right.y - c.left.y)) < 1e-6, "a crossing lies on its edge");
            walked += std::hypot(c.at.x - previous.x, c.at.y - previous.y);
            previous = c.at;
        }
        walked += std::hypot(75.0 - previous.x, 55.0 - previous.y);
        check(walked < length(r) + 1e-6, "the crossings follow the route");
    }
    {  // a goal in a closed part: the route ends at the closest point of the part it can reach
        NavMesh m = grid(3, 1, 100.0, [](int, int) { return true; });
        m.polys[2].blocked = true;
        const Route r = find(m, {5, 50}, {250, 50});
        check(r.outcome == Outcome::partial && std::fabs(r.points.back().x - 200.0) < 1e-9, "a partial route ends at the far edge");
    }
    {  // a start a little outside its polygon
        const NavMesh m = grid(3, 1, 10.0, [](int, int) { return true; });
        NavSearch search;
        const Route r = search.find(m, {0, {-2, 5}, {25, 5}, 0.0, 0});
        check(r.outcome == Outcome::reached && r.points.front() == Point{-2, 5}, "a route starts where the unit is");
    }
    {  // no connection: the route ends as close as the search got
        const auto open = [](int x, int) { return x != 3; };
        const NavMesh m = grid(7, 3, 10.0, open);
        const Route r = find(m, {5, 5}, {65, 5});
        check(r.outcome == Outcome::partial, "unreachable goal is partial");
        check(r.points.back().x <= 30.0 && r.points.back().x > 25.0, "partial route ends at the wall");
    }
    {  // higher terrain classes are avoided first, even when longer
        NavMesh m = grid(5, 3, 10.0, [](int, int) { return true; });
        for (const int y : {0, 1}) m.polys[static_cast<std::size_t>(y * 5 + 2)].weight = 5;
        const Route r = find(m, {5, 5}, {45, 5});
        check(r.outcome == Outcome::reached, "weighted room reaches the goal");
        bool high = false;
        for (const Point& p : r.points) high = high || (p.y >= 20.0);
        check(high, "the route goes around the higher class");
    }
    {  // blocked polygons, and the budget
        NavMesh m = grid(5, 1, 10.0, [](int, int) { return true; });
        m.polys[2].blocked = true;
        check(find(m, {5, 5}, {45, 5}).outcome == Outcome::partial, "a blocked polygon closes the way");
        m.polys[2].blocked = false;
        check(find(m, {5, 5}, {45, 5}, 0.0, 2).outcome == Outcome::partial, "the budget stops the search");
        check(find(m, {5, 5}, {45, 5}, 0.0, 2).expanded == 2, "the budget is exact");
    }
    {  // a serpentine of walls: the exact length, turning only at the wall ends
        const auto open = [](int x, int y) { return !(x == 2 && y < 5) && !(x == 5 && y > 0); };
        const NavMesh m = grid(8, 6, 10.0, open);
        const Route r = find(m, {5, 5}, {75, 55});
        check(r.outcome == Outcome::reached, "serpentine reaches the goal");
        const double best = std::hypot(15.0, 45.0) + 10.0 + std::hypot(20.0, 40.0) + 10.0 + std::hypot(15.0, 45.0);
        check(std::fabs(length(r) - best) < 1e-9, "serpentine route is the exact shortest");
        check(r.points.size() == 6, "serpentine turns only at the four wall corners");
    }
    {  // the same answer from any number of threads
        const NavMesh m = grid(40, 40, 10.0, [](int x, int y) { return (x + y * 7) % 11 != 0; });
        const Request req{m.locate({5, 15}), {5, 15}, {395, 385}, 2.0, 0};
        const Route reference = NavSearch{}.find(m, req);
        std::vector<int> same(8, 0);
        std::vector<std::thread> threads;
        for (std::size_t t = 0; t < same.size(); ++t)
            threads.emplace_back([&, t] {
                NavSearch search;
                int ok = 1;
                for (int i = 0; i < 200; ++i) {
                    const Route r = search.find(m, req);
                    ok = ok && r.points == reference.points && r.expanded == reference.expanded;
                }
                same[t] = ok;
            });
        for (std::thread& t : threads) t.join();
        for (const int ok : same) check(ok != 0, "every thread finds the same route");
    }
    if (g_failures == 0) std::printf("navsearch: all checks passed\n");
    return g_failures == 0 ? 0 : 1;
}
