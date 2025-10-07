//
// Created by Dextron12 on 5/10/2025.
//

#ifndef DEXIUM_ERROR_HPP
#define DEXIUM_ERROR_HPP

#pragma once
#include <fmt/core.h>
#include <fmt/color.h>
#include <string>

namespace Dexium {

    enum LogLevel {
        LOG_TRACE,
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR,
        LOG_FATAL,
        LOG_NONE
    };

    template<typename... Args>
    void TraceLog(LogLevel level, fmt::format_string<Args...> fmtStr, Args&&... args) {
        fmt::color col;
        std::string prefix;

        switch (level) {
            case LOG_TRACE:   prefix = "[TRACE] ";   col = fmt::color::white; break;
            case LOG_DEBUG:   prefix = "[DEBUG] ";   col = fmt::color::lime_green; break;
            case LOG_INFO:    prefix = "[INFO] ";    col = fmt::color::white; break;
            case LOG_WARNING: prefix = "[WARNING] "; col = fmt::color::yellow; break;
            case LOG_ERROR:   prefix = "[ERROR] ";   col = fmt::color::red; break;
            case LOG_FATAL:   prefix = "[FATAL] ";   col = fmt::color::purple; break;
            case LOG_NONE:    return;
        }

        // Option 1: Simple and safe
        fmt::print(fmt::fg(col), "{}\n", fmt::format("{}{}", prefix, fmt::format(fmtStr, std::forward<Args>(args)...)));

        // Option 2: Efficient (replace above line if you prefer)
        // fmt::memory_buffer buf;
        // fmt::format_to(buf, "{}{}", prefix, fmt::format(fmtStr, std::forward<Args>(args)...));
        // fmt::print(fmt::fg(col), "{}\n", fmt::to_string(buf));
    }

} // namespace Dexium


#endif //DEXIUM_ERROR_HPP