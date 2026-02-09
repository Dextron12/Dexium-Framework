//
// Created by Dextron12 on 3/12/25.
//

#ifndef DEXIUM_ERROR_H
#define DEXIUM_ERROR_H

#include <string>
#include <array>

//For Spam prevention of logs
#include <chrono> // For use of monotonic timer (std::chrono::steady_cloc)
#include <unordered_map> // To store hashed logs as the key and its last seen time as the value

#include <fmt/args.h>

namespace Dexium::Core {
    enum class ErrorType {
        STATUS,
        DEBUG,
        WARNING,
        ERROR,
        FATAL
    };

    enum class LogOutput {

        None = 0,               // Disable logging altogether
        Stdout = 1 << 0,        // Outputs to terminal (if accessible)
        DevTerm = 1 << 1,       // Outputs to the planned DevConsole (if enabled/supported)

        PrettyPrint = 1 << 2,  // Determines if messages should be output in colour
        /* --- CURRENT COLOUR SCHEME --- *
         * ErrorType    |       Colour
         * STATUS:      |       Light_Green
         * DEBUG:       |       Blue
         * WARNING:     |       Yellow
         * ERROR:       |       Light_Red
         * FATAL:       |       Purple
         *
         * Note: PRETTY_PRINT only applies to Stdout & DevTerm
         */

        PrefixErrorType = 1 << 3, // Prepends the ErrorType as [ErrorType]: msg

        StackLogs = 1 << 4 // Essentially prevents spam to the target output by storing all sent logs for a given period, if they are are seen multiple times within a given period, the concurrent logs are suppressed and a '[~]' is prepended to the message
        // Add other output options here

    };

    inline LogOutput operator|(LogOutput a, LogOutput b) {
        return static_cast<LogOutput>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    inline LogOutput operator&(LogOutput a, LogOutput b) {
        return static_cast<LogOutput>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    }

    inline LogOutput& operator|=(LogOutput& a, LogOutput b) {
        a = a | b;
        return a;
    }

    inline LogOutput& operator&=(LogOutput& a, LogOutput b) {
        a = a & b;
        return a;
    }

    // helper: checks if the value ahs the same set of flags as requested from flag
    inline bool hasFlag(LogOutput value, LogOutput flag) {
        return static_cast<uint8_t>(value & flag) != 0;
    }



    class Logger {
    public:
        // Buffer settings
        static constexpr size_t MaxMessages = 1024;
        size_t head = 0; // the location of the 'cursor' within the buffer
        size_t count = 0; // THe total message count

        // Log Spam Prevention variables
        struct m_spamTimer {
            std::chrono::steady_clock::time_point storedTime;
            bool hasDuplicates = false;

            m_spamTimer() {
                storedTime = std::chrono::steady_clock::now();
            }
        };

        std::unordered_map<std::string, m_spamTimer> m_storedLogs;
        double m_SpamTimeout = 4.0; // The time(in seconds) that a log will be monitored by the spam prevention system. If it exeeds this limit, the log will no longer be watched, until it re-triggers the spam prevention system
        // When a Log is stored in m_storedLogs and it is seen again before the timer reaches m_SpamTimeout, the timer resets and log is suppressed

        /* --- BITWISE ENUM FOR OUTPUT STREAMS ---
         * STORES THE REQUESTED STREAM OUTPUT
         * to add: outputs |= LogOutput::Stdout;  <-- Adds a flag
         * to remove: outputs &= ~LogOutput::Stdout; <-- removes a flag
         */
        LogOutput outputs = LogOutput::None;

        void log(ErrorType type, const std::string& msg);

        const std::array<std::string, MaxMessages>& getLog() const;

        // Dev Console func should go here too!

    private:
        std::array<std::string, MaxMessages> m_messages;
    };

    extern Logger GLogger; // Defined in .cpp

};



template <typename... Args>
void TraceLog(Dexium::Core::ErrorType type, fmt::format_string<Args...> fmt_str, Args&&... args) {
    // Construct formatted mesg
    std::string msg = fmt::format(fmt_str, std::forward<Args>(args)...);

    // The Global Logger now handles output for TraceLog
    Dexium::Core::GLogger.log(type, msg);
}


#endif //DEXIUM_ERROR_H