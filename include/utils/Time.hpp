//
// Created by ethan on 22/2/26.
//

#ifndef DEXIUM_TIME_HPP
#define DEXIUM_TIME_HPP

#include <fmt/chrono.h>
#include <fmt/base.h>

namespace Dexium::Utils {

    // A storage type(int) that repesents an hour in a 2 digit format in respect to 12h and 24h time
    struct Hour {
        int hour24;

        std::string to24() const {
            //Prepend 0 toi make double digit
            return fmt::format("{:02}", hour24);
        }

        std::string to12() const {
            int h12 = hour24 % 12;
            if (h12 == 0) h12 = 12;

            //Prepend 0 to make double digit format
            return fmt::format("{:02}", h12); // 01-12 format
        }

        std::string am_pm() const {
            return hour24 < 12 ? "AM" : "PM";
        }
    };

    // Fetches and stores the HW local time into various variables for formating (Uses chrono::hardware_clock)
    class localTime {
    public:
        localTime();

        //Date
        int Year = 0;
        int Month = 0;
        int Day = 0;

        //Time
        Hour Hr{};
        int Minute = 0;
        int Second = 0; // (recorded in 0-60)

        // Fetches local HW time and updates stored time
        void reset();
    private:
        std::tm localT = {};
    };

    // An indepentant monotonic clock that allows deltaTime and timeelapsed captures
    class MonoClock {
        /*
         * I use chrono::steady_clock for a library independent(aside from std) clock
         * It also avoids GLFWGetTime() that can be manipulated by resetting the global epoch with GLFWSetTime()
         * However, it should be noted that chrono_steady clcok is dependent on OS implementation and is significantly slower on Windows (up to 2x slower, and may even maek a SysCall to read OS clock time)
        */
    public:
        MonoClock();

        // Updates the delta between last called and current call times
        void update();

        double delta() const;
        double elapsed() const;

    private:
        std::chrono::steady_clock::time_point start_;
        std::chrono::steady_clock::time_point last_;
        double delta_{};
        double elapsed_{};
    };

}

#endif //DEXIUM_TIME_HPP