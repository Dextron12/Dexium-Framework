#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/color.h>

#include <chrono>

#include "fmt/chrono.h"

struct Hour {
    int hour24; // 0 - 23 range

    std::string to24() const {
        // Prepend 0 to make double digit
        return fmt::format("{:02}", hour24);
    }

    std::string to12() const {
        int h12 = hour24 % 12;
        if (h12 == 0) h12 = 12;

        // Prepend 0 to make double sdigit
        return fmt::format("{:02}", h12); // 01-12 format
    }

    std::string am_pm() const {
        return hour24 < 12 ? "AM" : "PM";
    }
};

class localTime {
public:
    localTime(const std::chrono::time_point<std::chrono::system_clock>& tp) {
        //Convert time_point to local time
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);

        localT = *std::localtime(&tt);

        year = localT.tm_year + 1900;
        month = localT.tm_mon + 1;
        day = localT.tm_mday;

        hr = Hour{localT.tm_hour};
        min = localT.tm_min;
        sec = localT.tm_sec;
    }

    int year = 0;
    int month = 0;
    int day = 0;

    Hour hr = {}; // A Simple 24h to 12hr covnerter
    int min = 0;
    int sec = 0;

private:
    std::tm localT;
};


int main() {
    localTime tm(std::chrono::system_clock::now());

    fmt::print(stderr, fg(fmt::color::blue), "Day: {}, H:{} {}, M:{}, S:{}\n", tm.day, tm.hr.to12(), tm.hr.am_pm(), tm.min, tm.sec);
}


/*

int main() {
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);

    std::tm local = *std::localtime(&tt);

    fmt::print(stderr, fg(fmt::color::red), "{:%Y-%m-%d %H:%M:%S}\n", local);

    fmt::print(stderr, fg(fmt::color::blue), "Year: {:%Y}, Month: {:%m}, Day: {:%d}\n"
        "Time: {:%H}:{:%M}:{:%S}\n",
        local, local, local,
        local, local, local);
}

*/
