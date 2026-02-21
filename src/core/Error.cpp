//
// Created by ethan on 17/2/26.
//


#include <core/Error.hpp>

#include <array>


void Dexium::Core::Logger::log(LogLevel type, const std::string &msg, Override<LoggerOutput> output, Override<LoggerFormat> format) {
    // Deduce outputs
    LoggerOutput finalOutputs = output.enabled ? output.value : outputs;

    // Ignore logging instance(or a single call) if no outputs are defined
    if (finalOutputs == LoggerOutput::None) {
        return;
    }

    // Deduce formatting scheme
    LoggerFormat finalFormat = format.enabled ? format.value : this->format; // Use of `this` to access class member Logger::format is a little dubious

    // Stores the final/resultant log msg
    std::string finalLog;
    fmt::color finalCol = TColours.def;
    finalLog.reserve(msg.length()); // tries toa void reallocs, but if excessive tags are used it may occur anyway

    //Firstly, we want to strip any tags if the formatting optin is enabled
    // This way if LoggerFormat::PrefixLogLevels si also enabled, this is then added afetr the strip
    // So this will strip any hard-written tags from the logs (usually internal system locations from where the log is being emitted from)

    //LoggerFormat::StripAllTags functionality
    if (hasFlag(finalFormat, LoggerFormat::StripAllTags)) {
        // WARNING: This a simple char stripper, it can break on nested lookups chars('[' | ']')
        bool inside = false;
        for (char c : msg) {
            if (c == '[') {
                inside = true;
                continue;
            }

            if (c == ':') {
                inside = false;
                continue;
            }

            if (!inside) {
                // Just append the character outside the label into the final buffer
                finalLog += c;
            }
        }
    } else {
        // Ensure we are now capturing on finalresult
        finalLog = msg;
    }

    if (hasFlag(finalFormat, LoggerFormat::PrefixLogLevels)) {

        // Compile-time array for faster output and less verbosity
        static constexpr std::array<std::string_view, 5> prefixes = {
            "[Status]: ",
            "[Debug]: ",
            "[Warning]: ",
            "[Error]: ",
            "[Fatal]: "
        };

        auto prefix = prefixes[static_cast<size_t>(type)]; // Converts the enum to its underlaying value [0=5] and mapts it to the prefixes array
        finalLog.insert(0, prefix);
    }

    if (hasFlag(finalFormat, LoggerFormat::PrettyPrint)) {
        // Only allow colour definitions when LoggerOutput = Stderr(defualt) or Stdout. Colours cannot be defined in file & DevTerminal will handle its own colours
        if (hasFlag(finalOutputs, LoggerOutput::Stderr) || hasFlag(finalOutputs, LoggerOutput::Stdout)) {
            // This is what makes fmt so cool!!

            switch (type) {
                case LogLevel::STATUS:
                    finalCol = TColours.status;
                    break;
                case LogLevel::DEBUG:
                    finalCol = TColours.debug;
                    break;
                case LogLevel::WARNING:
                    finalCol = TColours.warning;
                    break;
                case LogLevel::ERROR:
                    finalCol = TColours.error;
                    break;
                case LogLevel::FATAL:
                    finalCol = TColours.fatal;
                    break;
            }
        }
    }


    // Formatting complete, output to ALL provided output streams
    if (hasFlag(finalOutputs, LoggerOutput::Stderr)) {
        fmt::print(stderr, fg(finalCol), finalLog + "\n");
    }
    if (hasFlag(finalOutputs, LoggerOutput::Stdout)) {
        fmt::print(stdout, fg(finalCol), finalLog + "\n");
    }
    if (hasFlag(finalOutputs, LoggerOutput::DevConsole)) {
        // Currently No DevConsole impelented, relog msg onto Stderr and provide warning
        TraceLog(LogLevel::ERROR, LoggerOutput::Stderr, "[Logger]: DevConsole si currently not implemented. Falling back to Stderr as the output");
        // Relog the raw msg
        TraceLog(type, LoggerOutput::Stderr, finalFormat, msg);
    }
    if (hasFlag(finalOutputs, LoggerOutput::File)) {
        // Also currently NOT implemented, reroute to fallback
        TraceLog(LogLevel::ERROR, LoggerOutput::Stderr, "[Logger]: File outputs are currently not implemented! falling back to Stderr(default)");
        TraceLog(type, LoggerOutput::Stderr, finalFormat, msg);
        // BAD!!!! Real risk of recursive failure and violates cor eprinciples of acyclic logging systems!!
    }
    // No need to check for LoggerOutput::None, this is done at the start of the fn as an early exit(Logging is ingored on this flag)

}
