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

// Edge `edge` of `poly` seen leaving the polygon, its ends moved `radius` inwards. False when the gap is too narrow.
bool portal_of(const NavMesh& mesh, const Poly& poly, std::uint32_t edge, double radius, Portal& out) {
    const Point right = mesh.points[mesh.corners[poly.first + edge]];
    const Point left = mesh.points[mesh.corners[poly.first + (edge + 1) % poly.count]];
    const double width = distance(left, right);
    if (width < 2.0 * radius || width == 0.0) return radius == 0.0 && width > 0.0;
    const Point along = (left - right) * (radius / width);
    out = {left - along, right + along};
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

}  // namespace

std::int32_t NavMesh::locate(Point p) const {
    for (std::size_t i = 0; i < polys.size(); ++i)
        if (!polys[i].blocked && contains(*this, polys[i], p)) return static_cast<std::int32_t>(i);
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
    if (request.start_poly < 0 || request.start_poly >= count || mesh.polys[request.start_poly].blocked) return route;
    const Point target = request.to;
    if (contains(mesh, mesh.polys[request.start_poly], target)) {
        route.outcome = Outcome::reached;
        route.points = {request.from, target};
        return route;
    }
    nodes_.clear();
    heap_.clear();
    order_ = 0;
    std::map<std::pair<double, double>, double> root_length;  // looked up only, never walked: no effect on the order

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
    const auto corner_pos = [&](const Poly& poly, std::int32_t next) {
        for (std::uint32_t e = 0; e < mesh.polys[next].count; ++e)
            if (mesh.links[mesh.polys[next].first + e] == static_cast<std::int32_t>(&poly - mesh.polys.data())) return static_cast<std::int32_t>(e);
        return -1;
    };

    // A route can only bend around something: the vertices touching a wall, a blocked polygon, a gap too narrow, or another
    // terrain class.
    turnable_.assign(mesh.points.size(), 0);
    for (const Poly& poly : mesh.polys) {
        if (poly.blocked) continue;
        for (std::uint32_t edge = 0; edge < poly.count; ++edge) {
            const std::int32_t next = mesh.links[poly.first + edge];
            Portal unused;
            if (next >= 0 && !mesh.polys[static_cast<std::size_t>(next)].blocked && mesh.polys[static_cast<std::size_t>(next)].weight == poly.weight &&
                portal_of(mesh, poly, edge, request.radius, unused))
                continue;
            turnable_[mesh.corners[poly.first + edge]] = 1;
            turnable_[mesh.corners[poly.first + (edge + 1) % poly.count]] = 1;
        }
    }
    const auto left_turnable = [&](const Poly& poly, std::uint32_t edge) { return turnable_[mesh.corners[poly.first + (edge + 1) % poly.count]] != 0; };
    const auto right_turnable = [&](const Poly& poly, std::uint32_t edge) { return turnable_[mesh.corners[poly.first + edge]] != 0; };

    const Poly& start = mesh.polys[request.start_poly];
    const std::int32_t start_worst = std::max(0, start.weight);
    for (std::uint32_t edge = 0; edge < start.count; ++edge) {
        const std::int32_t next = mesh.links[start.first + edge];
        if (next < 0 || mesh.polys[next].blocked) continue;
        Portal portal;
        if (!portal_of(mesh, start, edge, request.radius, portal)) continue;
        Node node;
        node.root = request.from;
        node.left = portal.left;
        node.right = portal.right;
        node.poly = next;
        node.entry = corner_pos(start, next);
        node.worst = std::max(start_worst, mesh.polys[next].weight);
        node.left_corner = left_turnable(start, edge);
        node.right_corner = right_turnable(start, edge);
        push_interval(node);
    }

    Point nearest = request.from;
    std::int32_t nearest_node = -1;
    double nearest_score = distance(request.from, target);
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

        const Point closest = closest_on_segment(current.right, current.left, target);
        const double score = distance(closest, target) + (current.g + distance(current.root, closest)) / 16.0;
        if (score < nearest_score) {
            nearest_score = score;
            nearest = closest;
            nearest_node = index;
        }

        const Poly& poly = mesh.polys[static_cast<std::size_t>(current.poly)];
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
            if (next < 0 || mesh.polys[static_cast<std::size_t>(next)].blocked) continue;
            Portal portal;
            if (!portal_of(mesh, poly, edge, request.radius, portal)) continue;
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
            child.entry = corner_pos(poly, next);
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
    Point end = found >= 0 ? target : nearest;
    std::int32_t at = found >= 0 ? found : nearest_node;
    route.points.push_back(end);
    while (at >= 0) {
        const Node& node = nodes_[static_cast<std::size_t>(at)];
        if (!(route.points.back() == node.root)) route.points.push_back(node.root);
        at = node.parent;
    }
    if (!(route.points.back() == request.from)) route.points.push_back(request.from);
    std::reverse(route.points.begin(), route.points.end());
    return route;
}

}  // namespace joan::nav
