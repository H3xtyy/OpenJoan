// SPDX-License-Identifier: GPL-3.0-or-later
// docs/PATHFINDING.md.
#include "navpool.hpp"

#include <bit>

namespace joan::nav {

namespace {
constexpr std::size_t kMaxKept = 1024;  // finished routes nobody asked for are dropped beyond this many
}

SearchKey key_of(const void* owner, std::uint64_t generation, const Request& request) {
    SearchKey key;
    key.owner = owner;
    key.generation = generation;
    key.start_poly = request.start_poly;
    key.from_x = request.from.x;
    key.from_y = request.from.y;
    key.to_x = request.to.x;
    key.to_y = request.to.y;
    key.radius = request.radius;
    key.budget = request.budget;
    key.avoid_narrow = request.avoid_narrow;
    return key;
}

std::size_t RoutePool::Hash::operator()(const SearchKey& key) const {
    std::uint64_t h = 1469598103934665603ull;
    const auto add = [&](std::uint64_t v) { h = (h ^ v) * 1099511628211ull; };
    add(std::bit_cast<std::uintptr_t>(key.owner));
    add(key.generation);
    add(static_cast<std::uint32_t>(key.start_poly));
    for (const double v : {key.from_x, key.from_y, key.to_x, key.to_y, key.radius}) add(std::bit_cast<std::uint64_t>(v));
    add(key.budget);
    add(key.avoid_narrow ? 1u : 0u);
    return static_cast<std::size_t>(h);
}

RoutePool::RoutePool(unsigned threads) : wanted_(threads) {}

RoutePool::~RoutePool() {
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        stopping_ = true;
    }
    work_ready_.notify_all();
    for (std::thread& t : threads_) t.join();
}

void RoutePool::start_workers() {
    if (!threads_.empty()) return;
    threads_.reserve(wanted_);
    for (unsigned i = 0; i < wanted_; ++i) threads_.emplace_back([this] { work(); });
}

void RoutePool::submit(const SearchKey& key, std::shared_ptr<const NavMesh> mesh, const Request& request) {
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        start_workers();
        if (jobs_.size() >= kMaxKept) std::erase_if(jobs_, [](const auto& entry) { return entry.second->state == State::done; });
        if (jobs_.size() >= kMaxKept) return;  // every kept job is still queued or running: this one is searched when asked for
        const auto [it, added] = jobs_.try_emplace(key, nullptr);
        if (!added) return;
        auto job = std::make_shared<Job>();
        job->key = key;
        job->mesh = std::move(mesh);
        job->request = request;
        it->second = job;
        queue_.push_back(std::move(job));
    }
    work_ready_.notify_one();
}

std::optional<Route> RoutePool::take(const SearchKey& key) {
    std::unique_lock<std::mutex> lock(mutex_);
    const auto it = jobs_.find(key);
    if (it == jobs_.end()) return std::nullopt;
    const std::shared_ptr<Job> job = it->second;
    jobs_.erase(it);
    if (job->state == State::queued) {
        job->state = State::cancelled;
        return std::nullopt;
    }
    job_done_.wait(lock, [&] { return job->state == State::done; });
    return std::move(job->route);
}

void RoutePool::forget(const void* owner) {
    const std::lock_guard<std::mutex> lock(mutex_);
    std::erase_if(jobs_, [&](const auto& entry) {
        if (entry.first.owner != owner || entry.second->state == State::running) return false;
        if (entry.second->state == State::queued) entry.second->state = State::cancelled;
        return true;
    });
}

void RoutePool::work() {
    NavSearch search;
    std::unique_lock<std::mutex> lock(mutex_);
    for (;;) {
        work_ready_.wait(lock, [&] { return stopping_ || queue_head_ < queue_.size(); });
        if (stopping_) return;
        const std::shared_ptr<Job> job = std::move(queue_[queue_head_++]);
        if (queue_head_ == queue_.size()) {
            queue_.clear();
            queue_head_ = 0;
        }
        if (job->state != State::queued) continue;
        job->state = State::running;
        lock.unlock();
        Route route = search.find(*job->mesh, job->request);
        lock.lock();
        job->route = std::move(route);
        job->state = State::done;
        job_done_.notify_all();
    }
}

}  // namespace joan::nav
