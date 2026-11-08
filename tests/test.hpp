// Tiny self-contained test harness (no external dependencies).
#pragma once

#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace jt {

struct Case {
    const char* name;
    std::function<void()> fn;
};

inline std::vector<Case>& registry() {
    static std::vector<Case> r;
    return r;
}

struct Failure {
    std::string what;
};

struct Skip {
    std::string why;
};

struct Register {
    Register(const char* name, std::function<void()> fn) { registry().push_back({name, std::move(fn)}); }
};

inline int run_all() {
    int failed = 0, skipped = 0;
    for (auto& c : registry()) {
        try {
            c.fn();
            std::printf("[ ok ] %s\n", c.name);
        } catch (const Skip& s) {
            ++skipped;
            std::printf("[skip] %s (%s)\n", c.name, s.why.c_str());
        } catch (const Failure& f) {
            ++failed;
            std::printf("[FAIL] %s: %s\n", c.name, f.what.c_str());
        } catch (const std::exception& e) {
            ++failed;
            std::printf("[FAIL] %s: exception: %s\n", c.name, e.what());
        }
    }
    std::printf("%zu tests, %d failed, %d skipped\n", registry().size(), failed, skipped);
    return failed ? 1 : 0;
}

}  // namespace jt

#define JT_CAT2(a, b) a##b
#define JT_CAT(a, b) JT_CAT2(a, b)
#define TEST(name)                                                        \
    static void JT_CAT(test_fn_, __LINE__)();                             \
    static ::jt::Register JT_CAT(test_reg_, __LINE__)(name, JT_CAT(test_fn_, __LINE__)); \
    static void JT_CAT(test_fn_, __LINE__)()

#define CHECK(cond)                                                                               \
    do {                                                                                          \
        if (!(cond)) throw ::jt::Failure{std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " #cond}; \
    } while (0)

#define CHECK_THROWS(expr)                         \
    do {                                           \
        bool threw_ = false;                       \
        try {                                      \
            (void)(expr);                          \
        } catch (...) {                            \
            threw_ = true;                         \
        }                                          \
        CHECK(threw_ && "expected exception: " #expr); \
    } while (0)

#define SKIP(why) throw ::jt::Skip{why}
