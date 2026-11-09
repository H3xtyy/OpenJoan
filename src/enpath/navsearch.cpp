// SPDX-License-Identifier: GPL-3.0-or-later
// docs/PATHFINDING.md. Floating point here is + - * / and sqrt on doubles only: exact IEEE results on every target.
#include "navsearch.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <utility>

namespace joan::nav {

namespace {

Point operator-(Point a, Point b) { return {a.x - b.x, a.y - b.y}; }
Point operator+(Point a, Point b) { return {a.x + b.x, a.y + b.y}; }
Point operator*(Point a, double s) { return {a.x * s, a.y * s}; }
double cross(Point a, Point b) { return a.x * b.y - a.y * b.x; }
double dot(Point a, Point b) { return a.x * b.x + a.y * b.y; }
double length(Point a) { return std::sqrt(dot(a, a)); }
double distance(Point a, Point b) { return length(a - b); }

bool contains(const NavMesh& mesh, const Poly& poly, Point p) {
    for (std::uint32_t i = 0; i < poly.count; ++i) {
        const Point a = mesh.points[mesh.corners[poly.first + i]];
        const Point b = mesh.points[mesh.corners[poly.first + (i + 1) % poly.count]];
        if (cross(b - a, p - a) < 0.0) return false;
    }
    return true;
}

struct Portal {
    Point left, right;
};

constexpr std::uint8_t kCorner = 1;    // a route may bend here: the terrain class changes
constexpr std::uint8_t kObstacle = 2;  // a route may bend here and keeps its distance: a wall or a closed polygon touches it
constexpr double kMostPartialInset = 0.9;  // of the way to a polygon's centre: how far a partial route's end may move from the edge
constexpr double kMostInset = 0.49;    // of a gap's width: a gap too narrow for the radius is crossed at its middle

// The usable part of the edge from `right_vertex` to `left_vertex`: the ends touching an obstacle moved `radius` inwards.
// False only for an edge of no length.
bool portal_of(const NavMesh& mesh, const std::vector<std::uint8_t>& vertex, std::uint32_t right_vertex, std::uint32_t left_vertex, double radius, Portal& out) {
    const Point right = mesh.points[right_vertex], left = mesh.points[left_vertex];
    const double width = distance(left, right);
    if (width == 0.0) return false;
    const Point unit = (left - right) * (1.0 / width);
    const double inset = std::min(radius, width * kMostInset);
    out.left = (vertex[left_vertex] & kObstacle) != 0 ? left - unit * inset : left;
    out.right = (vertex[right_vertex] & kObstacle) != 0 ? right + unit * inset : right;
    return true;
}

constexpr double kEpsilon = 1e-9;
constexpr std::size_t kMaxNodes = 1u << 20;  // a search never holds more nodes than this, whatever the mesh

// Where the line from `root` through `through` meets the edge a -> b, as a fraction of the edge; `parallel` if it does not.
double hit_fraction(Point root, Point through, Point a, Point b, double parallel) {
    const Point direction = through - root;
    const double denominator = cross(b - a, direction);
    if (std::fabs(denominator) < kEpsilon) return parallel;
    return cross(root - a, direction) / denominator;
}

// The length of the shortest way from `root` to `target` through the interval [right, left].
double through_interval(Point root, Point left, Point right, Point target) {
    // a target on the root's side of the interval's line is mirrored across it: the route may come back
    const Point edge = left - right;
    const double root_side = cross(edge, root - right), target_side = cross(edge, target - right);
    Point goal = target;
    if ((root_side > 0.0) == (target_side > 0.0) && dot(edge, edge) > 0.0) {
        const Point foot = right + edge * (dot(target - right, edge) / dot(edge, edge));
        goal = foot + (foot - target);
    }
    const double s = hit_fraction(root, goal, right, left, -1.0);
    if (s >= 0.0 && s <= 1.0) return distance(root, goal);
    return std::min(distance(root, left) + distance(left, goal), distance(root, right) + distance(right, goal));
}

Point closest_on_segment(Point a, Point b, Point p) {
    const Point edge = b - a;
    const double squared = dot(edge, edge);
    if (squared == 0.0) return a;
    return a + edge * std::clamp(dot(p - a, edge) / squared, 0.0, 1.0);
}

// The point of a convex polygon's boundary closest to `p`.
Point closest_on_boundary(const NavMesh& mesh, const Poly& poly, Point p) {
    Point best = mesh.points[mesh.corners[poly.first]];
    double best_distance = distance(best, p);
    for (std::uint32_t i = 0; i < poly.count; ++i) {
        const Point on = closest_on_segment(mesh.points[mesh.corners[poly.first + i]], mesh.points[mesh.corners[poly.first + (i + 1) % poly.count]], p);
        const double d = distance(on, p);
        if (d < best_distance) {
            best_distance = d;
            best = on;
        }
    }
    return best;
}

}  // namespace

std::int32_t NavMesh::locate(Point p) const {
    for (std::size_t i = 0; i < polys.size(); ++i)
        if (!polys[i].blocked && joan::nav::contains(*this, polys[i], p)) return static_cast<std::int32_t>(i);
    return -1;
}

// Worst terrain first, then length; on a tie the earlier push, so the order never depends on the heap's layout.
bool NavSearch::later(const Open& a, const Open& b) {
    if (a.worst != b.worst) return a.worst > b.worst;
    if (a.f != b.f) return a.f > b.f;
    return a.order > b.order;
}

Route NavSearch::find(const NavMesh& mesh, const Request& request) {
    Route route;
    const auto count = static_cast<std::int32_t>(mesh.polys.size());
    if (request.start_poly < 0 || request.start_poly >= count) return route;
    const Point target = request.to;
    const Poly& start = mesh.polys[static_cast<std::size_t>(request.start_poly)];
    if (contains(mesh, start, target)) {
        route.outcome = Outcome::reached;
        route.points = {request.from, target};
        return route;
    }
    // a unit a little outside its polygon: the search starts from the polygon's edge, the route from the unit
    const Point origin = contains(mesh, start, request.from) ? request.from : closest_on_boundary(mesh, start, request.from);
    nodes_.clear();
    heap_.clear();
    order_ = 0;
    std::map<std::pair<double, double>, double> root_length;  // looked up only, never walked: no effect on the order

    const auto closed = [&](std::int32_t poly) {
        const Poly& p = mesh.polys[static_cast<std::size_t>(poly)];
        return p.blocked || (request.avoid_narrow && p.narrow);
    };
    const auto corner = [&](const Poly& poly, std::uint32_t i) { return mesh.corners[poly.first + i % poly.count]; };
    const auto push = [&](const Node& node, double f) {
        nodes_.push_back(node);
        heap_.push_back({node.worst, f, order_++, static_cast<std::int32_t>(nodes_.size() - 1)});
        std::push_heap(heap_.begin(), heap_.end(), later);
    };
    const auto push_interval = [&](Node node) {
        if (distance(node.left, node.right) == 0.0 && node.root == node.left) return;
        const auto [it, added] = root_length.try_emplace({node.root.x, node.root.y}, node.g);
        if (!added) {
            if (node.g > it->second + kEpsilon) return;  // this corner was reached by a shorter way
            it->second = std::min(it->second, node.g);
        }
        push(node, node.g + through_interval(node.root, node.left, node.right, target));
    };
    const auto entry_of = [&](std::int32_t from, std::int32_t next) {
        const Poly& p = mesh.polys[static_cast<std::size_t>(next)];
        for (std::uint32_t e = 0; e < p.count; ++e)
            if (mesh.links[p.first + e] == from) return static_cast<std::int32_t>(e);
        return -1;
    };

    // A route can only bend around something: the vertices touching a wall, a closed polygon, or another terrain class.
    vertex_.assign(mesh.points.size(), 0);
    for (std::size_t i = 0; i < mesh.polys.size(); ++i) {
        const Poly& poly = mesh.polys[i];
        if (poly.blocked) continue;
        for (std::uint32_t edge = 0; edge < poly.count; ++edge) {
            const std::int32_t next = mesh.links[poly.first + edge];
            std::uint8_t kind = 0;
            if (next < 0 || closed(next)) {
                kind = kObstacle;
            } else if (mesh.polys[static_cast<std::size_t>(next)].weight != poly.weight) {
                kind = kCorner;
            }
            vertex_[corner(poly, edge)] |= kind;
            vertex_[corner(poly, edge + 1)] |= kind;
        }
    }
    const auto left_turnable = [&](const Poly& poly, std::uint32_t edge) { return vertex_[corner(poly, edge + 1)] != 0; };
    const auto right_turnable = [&](const Poly& poly, std::uint32_t edge) { return vertex_[corner(poly, edge)] != 0; };

    // The way out of the search when the goal cannot be reached: the point of the visited polygons closest to it.
    Point nearest = closest_on_boundary(mesh, start, target);
    std::int32_t nearest_node = -1;
    double nearest_score = distance(nearest, target) + distance(origin, nearest) / 16.0;

    const std::int32_t start_worst = std::max(0, start.weight);
    for (std::uint32_t edge = 0; edge < start.count; ++edge) {
        const std::int32_t next = mesh.links[start.first + edge];
        if (next < 0 || closed(next)) continue;
        Portal portal;
        if (!portal_of(mesh, vertex_, corner(start, edge), corner(start, edge + 1), request.radius, portal)) continue;
        Node node;
        node.root = origin;
        node.left = portal.left;
        node.right = portal.right;
        node.poly = next;
        node.entry = entry_of(request.start_poly, next);
        node.via_poly = request.start_poly;
        node.via_edge = static_cast<std::int32_t>(edge);
        node.worst = std::max(start_worst, mesh.polys[static_cast<std::size_t>(next)].weight);
        node.left_corner = left_turnable(start, edge);
        node.right_corner = right_turnable(start, edge);
        push_interval(node);
    }

    std::int32_t found = -1;
    while (!heap_.empty() && nodes_.size() < kMaxNodes && (request.budget == 0 || route.expanded < request.budget)) {
        std::pop_heap(heap_.begin(), heap_.end(), later);
        const std::int32_t index = heap_.back().node;
        heap_.pop_back();
        const Node current = nodes_[static_cast<std::size_t>(index)];  // a copy: pushes below may move the storage
        if (current.final_node) {
            found = index;
            break;
        }
        ++route.expanded;

        const Poly& poly = mesh.polys[static_cast<std::size_t>(current.poly)];
        const Point closest = closest_on_boundary(mesh, poly, target);
        const double score = distance(closest, target) + (current.g + distance(current.root, closest)) / 16.0;
        if (score < nearest_score) {
            nearest_score = score;
            nearest = closest;
            nearest_node = index;
        }

        if (contains(mesh, poly, target)) {  // straight to the goal, or around one end of the interval
            Node last = current;
            last.final_node = true;
            last.parent = index;
            const double side_left = cross(current.left - current.root, target - current.root);
            const double side_right = cross(current.right - current.root, target - current.root);
            if (side_left <= kEpsilon && side_right >= -kEpsilon) {
                push(last, current.g + distance(current.root, target));
            } else if (side_left > 0.0 && current.left_corner) {
                last.g = current.g + distance(current.root, current.left);
                last.root = current.left;
                push(last, last.g + distance(last.root, target));
            } else if (side_right < 0.0 && current.right_corner) {
                last.g = current.g + distance(current.root, current.right);
                last.root = current.right;
                push(last, last.g + distance(last.root, target));
            }
            continue;
        }

        for (std::uint32_t edge = 0; edge < poly.count; ++edge) {
            if (static_cast<std::int32_t>(edge) == current.entry) continue;
            const std::int32_t next = mesh.links[poly.first + edge];
            if (next < 0 || closed(next)) continue;
            Portal portal;
            if (!portal_of(mesh, vertex_, corner(poly, edge), corner(poly, edge + 1), request.radius, portal)) continue;
            // Clip the edge (right end to left end) against the cone: what lies inside is seen from the root, what lies
            // beyond an end of the interval needs a turn at that end.
            const Point right_end = portal.right, left_end = portal.left, span = left_end - right_end;
            const double left_a = -cross(current.left - current.root, right_end - current.root);
            const double left_b = -cross(current.left - current.root, left_end - current.root);
            const double right_a = cross(current.right - current.root, right_end - current.root);
            const double right_b = cross(current.right - current.root, left_end - current.root);
            double seen_from = 0.0, seen_to = 1.0;
            const auto clip = [&](double a, double b) {
                if (a < -kEpsilon && b < -kEpsilon) {
                    seen_from = 2.0;  // nothing of the edge is inside
                } else if (a < -kEpsilon) {
                    seen_from = std::max(seen_from, a / (a - b));
                } else if (b < -kEpsilon) {
                    seen_to = std::min(seen_to, a / (a - b));
                }
            };
            clip(left_a, left_b);
            clip(right_a, right_b);

            Node child;
            child.poly = next;
            child.entry = entry_of(current.poly, next);
            child.via_poly = current.poly;
            child.via_edge = static_cast<std::int32_t>(edge);
            child.worst = std::max(current.worst, mesh.polys[static_cast<std::size_t>(next)].weight);
            child.parent = index;
            if (seen_from + kEpsilon < seen_to) {
                child.root = current.root;
                child.g = current.g;
                child.right = right_end + span * seen_from;
                child.left = right_end + span * seen_to;
                child.right_corner = seen_from == 0.0 && right_turnable(poly, edge);
                child.left_corner = seen_to == 1.0 && left_turnable(poly, edge);
                push_interval(child);
            }
            if (current.left_corner && (left_a < -kEpsilon || left_b < -kEpsilon)) {  // beyond the left end: turn there
                const double from = left_a < -kEpsilon && left_b < -kEpsilon ? 0.0 : left_a / (left_a - left_b);
                const double to = left_b < -kEpsilon ? 1.0 : from;
                const double low = left_a < -kEpsilon && !(left_b < -kEpsilon) ? 0.0 : from;
                child.root = current.left;
                child.g = current.g + distance(current.root, current.left);
                child.right = right_end + span * low;
                child.left = right_end + span * (left_b < -kEpsilon ? 1.0 : to);
                child.right_corner = low == 0.0 && right_turnable(poly, edge);
                child.left_corner = left_b < -kEpsilon && left_turnable(poly, edge);
                push_interval(child);
            }
            if (current.right_corner && (right_a < -kEpsilon || right_b < -kEpsilon)) {  // beyond the right end
                const double from = right_a < -kEpsilon && right_b < -kEpsilon ? 1.0 : right_a / (right_a - right_b);
                const double high = right_b < -kEpsilon && !(right_a < -kEpsilon) ? 1.0 : from;
                child.root = current.right;
                child.g = current.g + distance(current.root, current.right);
                child.right = right_end + span * (right_a < -kEpsilon ? 0.0 : from);
                child.left = right_end + span * high;
                child.right_corner = right_a < -kEpsilon && right_turnable(poly, edge);
                child.left_corner = high == 1.0 && left_turnable(poly, edge);
                push_interval(child);
            }
        }
    }

    route.outcome = found >= 0 ? Outcome::reached : Outcome::partial;
    Point end = target;
    if (found < 0) {  // the closest point lies on the polygon's edge: a body's width short of it, so that the unit does not end up pressing on a wall
        const Poly& last = mesh.polys[static_cast<std::size_t>(nearest_node >= 0 ? nodes_[static_cast<std::size_t>(nearest_node)].poly : request.start_poly)];
        Point centre{0.0, 0.0};
        for (std::uint32_t i = 0; i < last.count; ++i) centre = centre + mesh.points[corner(last, i)];
        centre = centre * (1.0 / static_cast<double>(last.count));
        const double to_centre = distance(nearest, centre);
        end = to_centre > 0.0 ? nearest + (centre - nearest) * (std::min(request.radius, to_centre * kMostPartialInset) / to_centre) : nearest;
    }
    std::vector<std::int32_t> chain;
    for (std::int32_t at = found >= 0 ? found : nearest_node; at >= 0; at = nodes_[static_cast<std::size_t>(at)].parent) chain.push_back(at);
    std::reverse(chain.begin(), chain.end());

    // A partial route: from the last root through its interval to the closest point, around an end of the interval if need be.
    Point last_leg = end;
    bool extra_bend = false;
    if (found < 0 && nearest_node >= 0) {
        const Node& node = nodes_[static_cast<std::size_t>(nearest_node)];
        const double s = hit_fraction(node.root, end, node.right, node.left, -1.0);
        if (s < 0.0 || s > 1.0) {
            const double by_left = distance(node.root, node.left) + distance(node.left, end);
            const double by_right = distance(node.root, node.right) + distance(node.right, end);
            last_leg = by_left < by_right ? node.left : node.right;
            extra_bend = true;
        }
    }

    route.points.push_back(request.from);
    const auto add_point = [&](Point p) {
        if (!(route.points.back() == p)) route.points.push_back(p);
    };
    add_point(origin);
    for (std::size_t i = 0; i < chain.size(); ++i) {
        const Node& node = nodes_[static_cast<std::size_t>(chain[i])];
        add_point(node.root);
        if (node.final_node) continue;
        Point towards = last_leg;  // where this node's straight piece of the route goes
        for (std::size_t j = i + 1; j < chain.size(); ++j)
            if (!(nodes_[static_cast<std::size_t>(chain[j])].root == node.root)) {
                towards = nodes_[static_cast<std::size_t>(chain[j])].root;
                break;
            }
        const Poly& via = mesh.polys[static_cast<std::size_t>(node.via_poly)];
        const auto edge = static_cast<std::uint32_t>(node.via_edge);
        Portal portal;
        if (!portal_of(mesh, vertex_, corner(via, edge), corner(via, edge + 1), request.radius, portal)) continue;
        const double s = hit_fraction(node.root, towards, portal.right, portal.left, -1.0);
        const Point at = s >= 0.0 && s <= 1.0 ? portal.right + (portal.left - portal.right) * s : closest_on_segment(portal.right, portal.left, towards);
        route.crossings.push_back({at, portal.left, portal.right, corner(via, edge + 1), corner(via, edge)});
    }
    if (extra_bend) add_point(last_leg);
    add_point(end);
    return route;
}

}  // namespace joan::nav
