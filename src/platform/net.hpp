// SPDX-License-Identifier: GPL-3.0-or-later
// A plain TCP connection for co-op (docs/COOP.md): the host waits for a player, a player connects to the host. Blocking
// calls, one connection each; Windows sockets or POSIX sockets underneath.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace joan::platform::net {

/// One TCP connection. Moving it moves the connection; destroying it closes the connection.
class Connection {
   public:
    Connection() = default;
    ~Connection();
    Connection(Connection&& other) noexcept;
    Connection& operator=(Connection&& other) noexcept;
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    /// Whether the connection is open.
    [[nodiscard]] bool open() const { return handle_ != kNone; }
    /// Sends all `n` bytes at `data`; false when the connection broke (it is closed then).
    bool send_all(const void* data, std::size_t n);
    /// Receives up to `n` bytes into `data`, waiting for at least one: the number received, 0 when the other side closed
    /// the connection, -1 on an error (the connection is closed in both cases).
    long receive(void* data, std::size_t n);
    /// Closes the connection.
    void close();

    /// Waits on `port` (all interfaces) for one connection and returns it; closed, with `error` set, on failure.
    static Connection accept_one(std::uint16_t port, std::string& error);
    /// Connects to `host`:`port`, trying again for up to `wait_ms` milliseconds; closed, with `error` set, on failure.
    static Connection connect_to(const std::string& host, std::uint16_t port, int wait_ms, std::string& error);

   private:
    using Handle = std::intptr_t;
    static constexpr Handle kNone = -1;
    explicit Connection(Handle h) : handle_(h) {}
    Handle handle_ = kNone;
};

/// Splits "host:port" (a bare port means this computer); false when there is no port number.
bool split_address(const std::string& text, std::string& host, std::uint16_t& port);

}  // namespace joan::platform::net
