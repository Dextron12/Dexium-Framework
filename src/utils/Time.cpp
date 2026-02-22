//
// Created by ethan on 22/2/26.
//

#include <utils/Time.hpp>

#include <chrono>

namespace Dexium::Utils {

    localTime::localTime() {
        reset();
    }

    void localTime::reset() {
        std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        localT = *std::localtime(&tt);

        Year = localT.tm_year + 1900;
        Month = localT.tm_mon + 1;
        Day = localT.tm_mday;

        Hr = Hour{localT.tm_hour};
        Minute = localT.tm_min;
        Second = localT.tm_sec;
    }

    MonoClock::MonoClock() : start_(std::chrono::steady_clock::now()), last_(start_) {

    }

    void MonoClock::update() {
        auto now = std::chrono::steady_clock::now();
        delta_ = std::chrono::duration<double>(now - last_).count();
        elapsed_ = std::chrono::duration<double>(now - start_).count();
        last_ = now;
    }

    double MonoClock::elapsed() const{ return elapsed_; }
    double MonoClock::delta() const{ return delta_; }



}