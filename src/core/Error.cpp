//
// Created by Dextron12 on 5/10/2025.
//

#include <core/Error.hpp>


template<typename... Args>
void Dexium::TraceLog(LogLevel level, fmt::format_string<Args...> fmtStr, Args &&... args) {
    fmt::color col;
    std::string prefix;
    switch (level) {
        case LOG_TRACE:
            prefix = "[TRACE] ";
            col = fmt::color::white;
            break;
        case LOG_DEBUG:
            prefix = "[DEBUG] ";
            col = fmt::color::lime_green;
            break;
        case LOG_INFO:
            prefix = "[INFO] ";
            col = fmt::color::white;
            break;
        case LOG_WARNING:
            prefix = "[WARNING] ";
            col = fmt::color::yellow;
            break;
        case LOG_ERROR:
            prefix = "[ERROR] ";
            col = fmt::color::red;
            break;
        case LOG_FATAL:
            prefix = "[FATAL] ";
            col = fmt::color::purple;
            break;
        case LOG_NONE:
            // Flag to enable/disable errorlogging functionality
            // Needs a global state bool stored in engine for reference
            break;
    }

    // Depending on configurations will, this func can output log to terminal, file, or the debug shell overlay.
    // The debug shell overlay will require ImGui and hence will fallback to terminal or logFile if the lib isnt included

    fmt::print(fmt::fg(col), "{}\n", fmt::format(prefix + fmtStr, std::forward<Args>(args)...));
}
