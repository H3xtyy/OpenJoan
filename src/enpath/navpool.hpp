// SPDX-License-Identifier: GPL-3.0-or-later
// Worker threads for the route search (docs/PATHFINDING.md). Routes are worked out ahead of the moment the game asks for them.
// A route depends only on the mesh and the request, so taking a finished one gives exactly what searching at that moment
// would have given: the threads change when the work is done, never its result.
#pragma once

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <unordered_map>
#include <vector>

#include "navsearch.hpp"

namespace joan::nav {

/// What makes two searches the same search.
struct SearchKey {
    const void* owner = nullptr;      ///< whose mesh (the PathSpace)
    std::uint64_t generation = 0;     ///< which state of its blocked polygons
    std::int32_t start_poly = -1;     ///< Request::start_poly
    double from_x = 0.0, from_y = 0.0, to_x = 0.0, to_y = 0.0;  ///< Request::from, Request::to
    double radius = 0.0;              ///< Request::radius
    std::uint32_t budget = 0;         ///< Request::budget
    bool avoid_narrow = false;        ///< Request::avoid_narrow
    friend bool operator==(const SearchKey&, const SearchKey&) = default;
};

/// The key of `request` on the mesh `generation` of `owner`.
SearchKey key_of(const void* owner, std::uint64_t generation, const Request& request);

/// A fixed set of worker threads and the routes they have found.
class RoutePool {
   public:
    /// @param threads how many workers to start, on the first submit
    explicit RoutePool(unsigned threads);
    ~RoutePool();
    RoutePool(const RoutePool&) = delete;
    RoutePool& operator=(const RoutePool&) = delete;

    /// Starts the search `request` on `mesh`, unless the same key is already known. The mesh is kept alive by the job and
    /// must not change after this call.
    void submit(const SearchKey& key, std::shared_ptr<const NavMesh> mesh, const Request& request);

    /// The route of `key`, if a worker has it or is working on it (this waits for it). Empty when nobody started it, or when
    /// it was still waiting for a worker: it is then cancelled and the caller searches itself, which is faster than waiting.
    std::optional<Route> take(const SearchKey& key);

    /// Forgets everything of `owner` that is finished or not started. Running searches finish on their own copy of the mesh.
    void forget(const void* owner);

    /// Number of worker threads running (0 before the first submit).
    [[nodiscard]] std::size_t workers() const { return threads_.size(); }

   private:
    enum class State { queued, running, done, cancelled };
    struct Job {
        SearchKey key;
        std::shared_ptr<const NavMesh> mesh;
        Request request;
        State state = State::queued;
        Route route;
    };
    struct Hash {
        std::size_t operator()(const SearchKey& key) const;
    };
    void work();
    void start_workers();

    unsigned wanted_;
    std::mutex mutex_;
    std::condition_variable work_ready_, job_done_;
    std::vector<std::shared_ptr<Job>> queue_;  // first in, first out
    std::size_t queue_head_ = 0;
    std::unordered_map<SearchKey, std::shared_ptr<Job>, Hash> jobs_;  // looked up only, or filtered by state
    std::vector<std::thread> threads_;
    bool stopping_ = false;
};

}  // namespace joan::nav
