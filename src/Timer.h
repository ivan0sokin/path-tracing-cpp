#ifndef _TIMER_H
#define _TIMER_H

#include <chrono>
#include <functional>

class Timer {
public:
    constexpr Timer() noexcept = delete;
    constexpr Timer(const Timer&) = delete;
    constexpr Timer(Timer&&) = delete;

    inline static double MeasureInMillis(std::function<void()> f) {
        auto t1 = std::chrono::high_resolution_clock::now();
        f();
        auto t2 = std::chrono::high_resolution_clock::now();
        static_cast<std::chrono::duration<float, std::milli>>(t2 - t1).count();
        return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(t2 - t1).count();
    }
};

#endif