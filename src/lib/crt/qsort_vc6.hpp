// The VC6 qsort algorithm (lib/crt/mem_intrinsics.cpp crt_qsort, 0x007b2d55) as a template over a typed array
// with an inline comparison: the same comparisons in the same order and the same swaps, so the same resulting
// order for any input (ties and unordered keys included) - without the call through a function pointer (and the
// hook's jump) per comparison and the byte-by-byte swaps. `comp(a, b)` returns what the qsort callback would.
#pragma once

#include <cstdint>
#include <utility>

namespace joan::crt {

template <class T, class Comp>
void qsort_vc6(T* base, std::uint32_t num, Comp comp) {
    if (num < 2) return;
    auto swap = [](T* a, T* b) {
        if (a != b) std::swap(*a, *b);
    };
    T* lostk[30] = {};
    T* histk[30] = {};
    int stkptr = 0;
    T* lo = base;
    T* hi = base + (num - 1);
    for (;;) {
        const std::uint32_t size = static_cast<std::uint32_t>(hi - lo) + 1;
        if (size <= 8) {
            while (hi > lo) {  // shortsort 0x7b2ea9
                T* max = lo;
                for (T* p = lo + 1; p <= hi; ++p)
                    if (comp(*p, *max) > 0) max = p;
                swap(max, hi);
                --hi;
            }
        } else {
            T* const mid = lo + (size >> 1);
            swap(mid, lo);
            T* loguy = lo;
            T* higuy = hi + 1;
            for (;;) {
                do {
                    ++loguy;
                } while (loguy <= hi && comp(*loguy, *lo) <= 0);
                do {
                    --higuy;
                } while (higuy > lo && comp(*higuy, *lo) >= 0);
                if (higuy < loguy) break;
                swap(loguy, higuy);
            }
            swap(lo, higuy);
            if (higuy - lo - 1 >= hi - loguy) {
                if (lo + 1 < higuy) {
                    lostk[stkptr] = lo;
                    histk[stkptr] = higuy - 1;
                    ++stkptr;
                }
                if (loguy < hi) {
                    lo = loguy;
                    continue;
                }
            } else {
                if (loguy < hi) {
                    lostk[stkptr] = loguy;
                    histk[stkptr] = hi;
                    ++stkptr;
                }
                if (lo + 1 < higuy) {
                    hi = higuy - 1;
                    continue;
                }
            }
        }
        --stkptr;
        if (stkptr < 0) return;
        lo = lostk[stkptr];
        hi = histk[stkptr];
    }
}

}  // namespace joan::crt
