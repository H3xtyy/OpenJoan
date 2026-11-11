// SPDX-License-Identifier: GPL-3.0-or-later
// net.hpp.
#include "net.hpp"

#include <chrono>
#include <cstring>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace joan::platform::net {

namespace {

#ifdef _WIN32
using Native = SOCKET;
constexpr Native kBad = INVALID_SOCKET;
void close_native(Native s) { closesocket(s); }
bool start_sockets() {
    static const bool ok = [] {
        WSADATA data;
        return WSAStartup(MAKEWORD(2, 2), &data) == 0;
    }();
    return ok;
}
std::string last_error() { return "socket error " + std::to_string(WSAGetLastError()); }
#else
using Native = int;
constexpr Native kBad = -1;
void close_native(Native s) { ::close(s); }
bool start_sockets() { return true; }
std::string last_error() { return std::strerror(errno); }
#endif

Native native(std::intptr_t h) { return static_cast<Native>(h); }

void no_delay(Native s) {
    int on = 1;
    setsockopt(s, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&on), sizeof on);
}

}  // namespace

Connection::~Connection() { close(); }

Connection::Connection(Connection&& other) noexcept : handle_(other.handle_) { other.handle_ = kNone; }

Connection& Connection::operator=(Connection&& other) noexcept {
    if (this != &other) {
        close();
        handle_ = other.handle_;
        other.handle_ = kNone;
    }
    return *this;
}

void Connection::close() {
    if (handle_ != kNone) close_native(native(handle_));
    handle_ = kNone;
}

bool Connection::send_all(const void* data, std::size_t n) {
    const char* p = static_cast<const char*>(data);
    while (n > 0 && open()) {
        const int chunk = n > (1u << 30) ? (1 << 30) : static_cast<int>(n);
#ifdef _WIN32
        const int sent = ::send(native(handle_), p, chunk, 0);
#else
        const auto sent = ::send(native(handle_), p, static_cast<std::size_t>(chunk), MSG_NOSIGNAL);
#endif
        if (sent <= 0) {
            close();
            return false;
        }
        p += sent;
        n -= static_cast<std::size_t>(sent);
    }
    return open();
}

long Connection::receive(void* data, std::size_t n) {
    if (!open()) return -1;
    const int chunk = n > (1u << 30) ? (1 << 30) : static_cast<int>(n);
#ifdef _WIN32
    const int got = ::recv(native(handle_), static_cast<char*>(data), chunk, 0);
#else
    const auto got = ::recv(native(handle_), data, static_cast<std::size_t>(chunk), 0);
#endif
    if (got <= 0) {
        close();
        return got == 0 ? 0 : -1;
    }
    return static_cast<long>(got);
}

long Connection::receive_now(void* data, std::size_t n) {
    if (!open()) return -1;
#ifdef _WIN32
    WSAPOLLFD p{native(handle_), POLLRDNORM, 0};
    const int ready = WSAPoll(&p, 1, 0);
#else
    pollfd p{native(handle_), POLLIN, 0};
    const int ready = ::poll(&p, 1, 0);
#endif
    if (ready < 0) {
        close();
        return -1;
    }
    if (ready == 0) return 0;
    const long got = receive(data, n);  // readable: data, or the other side closed
    return got > 0 ? got : -1;
}

Connection Connection::accept_one(std::uint16_t port, std::string& error) {
    if (!start_sockets()) {
        error = "the system's sockets cannot be started";
        return {};
    }
    const Native listener = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (listener == kBad) {
        error = last_error();
        return {};
    }
    int on = 1, off = 0;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&on), sizeof on);
    setsockopt(listener, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&off), sizeof off);  // IPv4 too
    sockaddr_in6 address{};
    address.sin6_family = AF_INET6;
    address.sin6_addr = in6addr_any;
    address.sin6_port = htons(port);
    if (::bind(listener, reinterpret_cast<const sockaddr*>(&address), sizeof address) != 0 || ::listen(listener, 1) != 0) {
        error = last_error();
        close_native(listener);
        return {};
    }
    const Native s = ::accept(listener, nullptr, nullptr);
    if (s == kBad) error = last_error();
    close_native(listener);
    if (s == kBad) return {};
    no_delay(s);
    return Connection(static_cast<Handle>(s));
}

Connection Connection::connect_to(const std::string& host, std::uint16_t port, int wait_ms, std::string& error) {
    if (!start_sockets()) {
        error = "the system's sockets cannot be started";
        return {};
    }
    const auto until = std::chrono::steady_clock::now() + std::chrono::milliseconds(wait_ms);
    const std::string service = std::to_string(port);
    for (;;) {
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        addrinfo* found = nullptr;
        if (getaddrinfo(host.empty() ? "localhost" : host.c_str(), service.c_str(), &hints, &found) == 0) {
            for (const addrinfo* a = found; a != nullptr; a = a->ai_next) {
                const Native s = ::socket(a->ai_family, a->ai_socktype, a->ai_protocol);
                if (s == kBad) continue;
                if (::connect(s, a->ai_addr, static_cast<int>(a->ai_addrlen)) == 0) {
                    freeaddrinfo(found);
                    no_delay(s);
                    return Connection(static_cast<Handle>(s));
                }
                error = last_error();
                close_native(s);
            }
            freeaddrinfo(found);
        } else {
            error = "cannot find the host " + host;
        }
        if (std::chrono::steady_clock::now() >= until) return {};
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

bool split_address(const std::string& text, std::string& host, std::uint16_t& port) {
    const std::size_t colon = text.rfind(':');
    const std::string number = colon == std::string::npos ? text : text.substr(colon + 1);
    if (number.empty() || number.find_first_not_of("0123456789") != std::string::npos || number.size() > 5) return false;
    const unsigned long value = std::stoul(number);
    if (value == 0 || value > 65535) return false;
    port = static_cast<std::uint16_t>(value);
    host = colon == std::string::npos ? std::string() : text.substr(0, colon);
    if (host.size() >= 2 && host.front() == '[' && host.back() == ']') host = host.substr(1, host.size() - 2);  // [IPv6]
    return true;
}

}  // namespace joan::platform::net
