//
// Created by ethan on 10/2/26.
//

#ifndef DEXIUM_ERROR_HPP
#define DEXIUM_ERROR_HPP

// Standard Libs
#include <string>
#include <unordered_map>

// FMT, only the best library out there! Used for CXX20 style print and colour formatting
#include <fmt/printf.h>
#include <fmt/format.h>
#include <fmt/color.h>

// Templated Bitmask operator class
#include <core/BitwiseFlag.hpp>
// Defines MonoClock, used for caching logs by time
#include <utils/Time.hpp>


#include <memory>
#include <atomic> // For use for atomic_bool in TraceLog(), avoids mutexes, but we should test to see if atomic_bool is any slower than a mutex

/*
 * This Header defines the Dexium-Framework Logger implementation
 * It supports:
 * - multiple streamed outputs (Stdout, Stderr, IO:File, DevConsole) Default is Stderr as its forces flushing after each ln on all systems
 * - Priority Level flagging per log
 * - Time-based buffering of logs to prevent repetitive output while ensuring logger accuracy
 */

namespace Dexium::Core::Detail {
    // Defines the colours used with LoggerFormat::PrettyPrint enabled AND LoggerOutput = Stderr | Stdout
    struct TerminalColourOutputs {
        fmt::color def = fmt::color::cornsilk; // The default colour used when PrettyPrint is NOT enabled
        fmt::color status = fmt::color::cadet_blue;
        fmt::color debug = fmt::color::deep_sky_blue;
        fmt::color warning = fmt::color::yellow;
        fmt::color error = fmt::color::red;
        fmt::color fatal = fmt::color::purple;
    };
}

enum class LogLevel {
    STATUS,
    DEBUG,
    WARNING,
    ERROR,
    FATAL
};

namespace Dexium::Core {

    enum class LoggerOutput {
        None = 0,
        Stdout = 1 << 0,
        Stderr = 1 << 1,
        File = 1 << 2,
        DevConsole = 1 << 3
    };

    // Enable Bitwise flag operations for LoggerOutput
    template <>
    struct EnableBitmaskOperators<LoggerOutput> {
        static constexpr bool value = true;
    };

    enum class LoggerFormat {
        None = 0,
        PrettyPrint = 1 << 0, // Determines if msg should print in colour according to its LogLevel
        PrefixLogLevels = 1 << 1, // Pre-pends the LogLevel as a tag onto the log when enabled. EG: [LogLevel]: LogMsg.
        StripAllTags = 1 << 2, // Checks msg for any system locator tags, EG '[Texture]: ' and removes it from the final output.
        ImmediateMode = 1 << 3, // Skips the buffering of logs and immediately outputs the log. WARNING: This disables any attempt at spam prevention
    };
    // Enable Bitwise mask oeprations for LoggerFormat
    template<>
    struct EnableBitmaskOperators<LoggerFormat> {
        static constexpr bool value = true;
    };

    // Overwrite template to deduce if one bitwise obj can override the other
    /*enum class OverrideMode {
        Inherit,        // Uses pre-defined state-level values(Often the default, unless Logger::outputs and Logger::format is explicitly edited
        Override,       // Use the provided values
    };*/
    template<typename T>
    struct Override {
        bool enabled = false;
        T value{};

        Override() = default;

        //Constructor to implcitly convert a wrapped enum into an Override
        Override(T v): enabled(true), value(v) {}

        Override(bool e, T v) : enabled(e), value(v) {}

        //Allow for empty construction of Override wrapper
        static Override Inherit() {
            return {};
        }
    };

    struct LogCacheEntry {
        double expTime;
        bool hasOutput = false; // HAs the Logger already otputt he cached log?
    };

    class Logger {
    public:
        // State-level bitmasks
        LoggerOutput outputs = LoggerOutput::Stderr; // By defualt, use Stderr as it flushes the buffer after every print on ALL systems
        LoggerFormat format = LoggerFormat::None; // Leave the formating up to the usr

        Detail::TerminalColourOutputs TColours; // Only is used if LoggerFormat::PrettyPrint & (LoggerOutput::Stderr(default) | LoggerOutput::Stdout) is present
        std::string LogFolderName = "Logs"; // The name of the folder in where to store all logs. Allows overwriting per Logger
        std::string logfilePrefix = "Dexium-Crash";

        float delayCahceTime = 1.5; // Measured in seconds, determines how long a messaged is delayed before outputting to the requested sinks, allows tracking multiples of same messages

        // The internal logging func to 'TraceLog' and handles all output logic for TraceLog(Which is a thin globally accessible wrapper of this func)
        void log(LogLevel type, const std::string& msg, Override<LoggerOutput> l_output = Override<LoggerOutput>::Inherit(), Override<LoggerFormat> l_format = Override<LoggerFormat>::Inherit());
    private:
        // Records the logged stream to the last ln of the dated log file, or creates a new logfile if one didnt previsouly exist
        void writeLog(LogLevel type, const std::string& msg);

        void writeToSinks(const std::string& logMsg, fmt::color color, LoggerOutput sinks);

        std::unordered_map<std::string, LogCacheEntry> m_cachedLogs;
        Utils::MonoClock m_CacheClock;
    };
}

// Declares the storage point for the logger sub-system
namespace Dexium::Core::LogService {
    // This fn is intentionally mutable as its msotly the engien accessing it. A deleteLogger fn should be expsoed to prevent end-users from having to call this in the future
    inline std::unique_ptr<Logger>& use() {
        static std::unique_ptr<Logger> logger; // ODR & multi-thread safe since CXX11
        return logger;
    }

}

template<typename... Args>
void TraceLog(LogLevel type, fmt::format_string<Args...> fmt_str, Args&&... args) {
    // Check if a logger exists:
    auto& logSvs = Dexium::Core::LogService::use();
    if (logSvs) {
        // Construct formatted msg
        std::string msg = fmt::format(fmt_str, std::forward<Args>(args)...);
        // Pass the msg onto the GLogger for processing
        logSvs->log(type, msg);
    }
    else {
#ifdef DEBUG
        static std::atomic_bool LoggerPanic = false; // Prevents the following loggerPanic output being output more than once

        bool expected = false;
        if (LoggerPanic.compare_exchange_strong(expected, true)) {
            fmt::print(stderr, "[PANIC]: A call to TraceLog was executed, but no Logger sub-system is initialised.\n"
                               "All calls to TraceLog are suppressed\n");
        }
#endif
    }
}

// Provides OPTIONAL outoput overrides per log
template<typename... Args>
void TraceLog(LogLevel type, Dexium::Core::Override<Dexium::Core::LoggerOutput> output, fmt::format_string<Args...> fmt_str, Args&&... args) {
    auto& logSys = Dexium::Core::LogService::use();
    //Construct formatted msg
    if (logSys) {
        std::string msg = fmt::format(fmt_str, std::forward<Args>(args)...);
        // Pass the msg & overridden outputStream to GLogger for processing
        //Dexium::Globals::GLogger.log(type, msg, output);
        logSys->log(type, msg, output);
    } else {
        #ifdef DEBUG
        static std::atomic_bool LoggerPanic = false; // Prevents the following loggerPanic output being output more than once

        bool expected = false;
        if (LoggerPanic.compare_exchange_strong(expected, true)) {
            fmt::print(stderr, "[PANIC]: A call to TraceLog was executed, but no Logger sub-system is initialised.\n"
                               "All calls to TraceLog are suppressed\n");
        }
        #endif
    }
}

// Provides OPTIONAL format overrides per log
template<typename... Args>
void TraceLog(LogLevel type, Dexium::Core::Override<Dexium::Core::LoggerFormat> format, fmt::format_string<Args...> fmt_str, Args&&... args) {
    auto& sysLog = Dexium::Core::LogService::use();

    if (sysLog) {
        std::string msg = fmt::format(fmt_str, std::forward<Args>(args)...);
        /*Dexium::Globals::GLogger.log(
            type,
            msg,
            Dexium::Core::Override<Dexium::Core::LoggerOutput>{false, Dexium::Core::LoggerOutput::Stderr},
            format
            );*/
        sysLog->log(type, msg, {}, format);
    } else {
        #ifdef DEBUG
        static std::atomic_bool LoggerPanic = false; // Prevents the following loggerPanic output being output more than once

        bool expected = false;
        if (LoggerPanic.compare_exchange_strong(expected, true)) {
            fmt::print(stderr, "[PANIC]: A call to TraceLog was executed, but no Logger sub-system is initialised.\n"
                               "All calls to TraceLog are suppressed\n");
        }
        #endif
    }
}

// Provides OPTIONAL input + format overrides per log
template<typename... Args>
void TraceLog(LogLevel type, Dexium::Core::LoggerOutput output, Dexium::Core::LoggerFormat format, fmt::format_string<Args...>fmt_str,  Args&&... args) {
    auto& sysLog = Dexium::Core::LogService::use();
    if (sysLog) {
        std::string msg = fmt::format(fmt_str, std::forward<Args>(args)...);
        //Dexium::Globals::GLogger.log(type, msg, output, format);
        sysLog->log(type, msg, output, format);
    } else {
        #ifdef DEBUG
        static std::atomic_bool LoggerPanic = false; // Prevents the following loggerPanic output being output more than once

        bool expected = false;
        if (LoggerPanic.compare_exchange_strong(expected, true)) {
            fmt::print(stderr, "[PANIC]: A call to TraceLog was executed, but no Logger sub-system is initialised.\n"
                               "All calls to TraceLog are suppressed\n");
        }
        #endif
    }
}

// IF the state-level Output bitmask DOES NOT contain stderr as an output, ...
// it will use the state-level output even when an override is provided. Because Stderr is set to the output default
// So by not providing an override to Output through TraceLog will set it to the default (Stderr) when using a format override
// so this should prevent format overrides from outputting to stderr when a state-level output is expected to be used.

#endif //DEXIUM_ERROR_HPP