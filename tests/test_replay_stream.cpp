// SPDX-License-Identifier: GPL-3.0-or-later
// Reading a recording as it arrives in pieces (co-op, docs/COOP.md) gives the same events as reading the file.
#include <cstdio>
#include <algorithm>
#include <filesystem>
#include <random>
#include <string>
#include <vector>

#include "../src/runtime/replay/stream.hpp"

using namespace joan::replay;

namespace {

int g_failures = 0;
void check(bool ok, const char* what) {
    if (!ok) {
        std::printf("FAILED: %s\n", what);
        ++g_failures;
    }
}

std::string describe(const Event& e) {
    std::string s = std::to_string(static_cast<int>(e.type)) + ":" + std::to_string(e.offset) + ":" + std::to_string(e.id) +
                    ":" + std::to_string(e.check) + ":" + std::to_string(e.size) + ":" + std::to_string(e.a) + ":" +
                    std::to_string(e.b) + ":" + std::to_string(e.c) + ":" + std::to_string(e.frame.frame) + ":" +
                    std::to_string(e.frame.image) + ":" + std::to_string(e.frame.pages.size()) + ":" + std::to_string(e.words.size());
    if (e.payload != nullptr) s.append(reinterpret_cast<const char*>(e.payload), e.size);
    return s;
}

std::vector<std::string> read_all(Reader& r) {
    std::vector<std::string> out;
    Event e;
    while (r.peek(e)) {
        out.push_back(describe(e));
        r.next();
    }
    return out;
}

}  // namespace

int main() {
    const std::filesystem::path file = std::filesystem::temp_directory_path() / "openjoan_replay_stream_test.rec";
    {
        Writer w;
        Header h;
        h.data_va = 0x1000;
        h.data_size = 0x2000;
        h.page_size = 0x1000;
        check(w.open(file.string(), h), "the recording is written");
        w.env("first environment block");
        std::mt19937 rng(5);
        const auto R = [&] { return static_cast<std::uint32_t>(rng()); };
        for (std::uint32_t frame = 0; frame < 300; ++frame) {
            for (int k = 0; k < 7; ++k) {
                std::vector<std::uint8_t> payload(R() % 40);
                for (auto& b : payload) b = static_cast<std::uint8_t>(R());
                w.call(static_cast<std::uint16_t>(R() % 50), R(), payload.data(), static_cast<std::uint32_t>(payload.size()));
            }
            w.wndproc(R(), R(), R());
            if (frame % 9 == 0) w.apc(frame, 0, R());
            FrameHash f;
            f.frame = frame;
            f.image = (static_cast<std::uint64_t>(R()) << 32) | R();
            f.pages = {R(), R()};
            w.frame(f);
            if (frame % 50 == 0) w.snapshot(frame, std::vector<std::uint32_t>(16, frame));
        }
        w.end(300);
        w.close();
    }
    Reader from_file;
    check(from_file.load(file.string()) == nullptr, "the file loads");
    const std::vector<std::string> expected = read_all(from_file);
    check(expected.size() > 2000, "the recording has its events");

    std::vector<std::uint8_t> bytes(std::filesystem::file_size(file));
    std::FILE* f = std::fopen(file.string().c_str(), "rb");
    check(f != nullptr && std::fread(bytes.data(), 1, bytes.size(), f) == bytes.size(), "the file is read back");
    if (f != nullptr) std::fclose(f);
    std::filesystem::remove(file);

    for (const std::size_t largest : {std::size_t{1}, std::size_t{7}, std::size_t{4096}}) {
        std::mt19937 rng(static_cast<unsigned>(largest));
        std::size_t sent = 0;
        Reader live;
        const char* error = live.follow([&](std::vector<std::uint8_t>& data) {
            if (sent >= bytes.size()) return false;
            const std::size_t n = std::min(bytes.size() - sent, std::size_t{1} + static_cast<std::size_t>(rng()) % largest);
            data.insert(data.end(), bytes.begin() + static_cast<std::ptrdiff_t>(sent), bytes.begin() + static_cast<std::ptrdiff_t>(sent + n));
            sent += n;
            return true;
        });
        check(error == nullptr, "a followed recording starts");
        check(read_all(live) == expected, largest == 1 ? "byte by byte, every event is the same"
                                                       : "in pieces, every event is the same");
        check(live.error() == nullptr, "the end of the stream is not an error");
    }
    if (g_failures == 0) std::printf("replay stream: all checks passed\n");
    return g_failures == 0 ? 0 : 1;
}
