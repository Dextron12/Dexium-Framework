//
// Created by Dextron12 on 3/12/25.
//

#include "core/Error.h"

#include <fmt/color.h>


namespace Dexium::Core {

    void Logger::log(ErrorType type, const std::string &msg) {
        // Store message into buffer + update buffer head
        m_messages[head] = msg;
        head = (head + 1) % MaxMessages;
        if (count < MaxMessages) count++;

        std::string output;

        // If SpamPrevention system is enabled (LogOutput::StackLogs)
        if (hasFlag(outputs, LogOutput::StackLogs)) {
            // Spam Prevention system is enabled
            auto key = m_storedLogs.find(msg); // Searches for raw msg
            if (key != m_storedLogs.end()) {
                // Message exists, check if duplicateFlags has been triggered

            } else {
                // Track raw log
            }
        }

        // Output the latest message (if enabled)
        if (hasFlag(outputs, LogOutput::Stdout)) {
            // Logger has Stdout enabled:

            // Prepends the ErrorType onto the msg
            if (hasFlag(outputs, LogOutput::PrefixErrorType)) {
                switch (type) {
                    case ErrorType::STATUS:
                        output = "[Status] ";
                        break;
                    case ErrorType::DEBUG:
                        output = "[Debug] ";
                        break;
                    case ErrorType::WARNING:
                        output = "[Warning] ";
                        break;
                    case ErrorType::ERROR:
                        output = "[Error] ";
                        break;
                    case ErrorType::FATAL:
                        output = "[Fatal] ";
                        break;
                    default:
                        fmt::print(stderr, fg(fmt::color::purple), "[Engine Notice]: Unsupported ErrorType has been defined!");
                }
            }

            if (hasFlag(outputs, LogOutput::PrettyPrint)) {
                // Print message in colour according to the colour scheme defined under the PRETTY_PRINT definition in the header
                if (type == ErrorType::STATUS) {
                    fmt::print(stderr, fg(fmt::color::light_green), output + msg + "\n");
                }
                if (type == ErrorType::DEBUG) {
                    fmt::print(stderr, fg(fmt::color::sky_blue), output + msg + "\n");
                }
                if (type == ErrorType::WARNING) {
                    fmt::print(stderr, fg(fmt::color::yellow), output + msg + "\n");
                }
                if (type == ErrorType::ERROR) {
                    fmt::print(stderr, fg(fmt::color::pale_violet_red), output + msg + "\n");
                }
                if (type == ErrorType::FATAL) {
                    fmt::print(stderr, fg(fmt::color::medium_purple), output + msg + "\n");
                }
            } else {
                fmt::print(stderr, fg(fmt::color::white), output + msg + "\n");
            }
        }

        // Forces a stdout buffer flush. Some consoles need manual flushing to show fmt output
        //fmt::print(stderr, "");
        // Outputting to stderr should automatically flsuh on ALL systems
    }

    const std::array<std::string, Logger::MaxMessages>& Logger::getLog() const {
        return m_messages;
    }

    // Static declaration in header
    Logger GLogger = {};



}

