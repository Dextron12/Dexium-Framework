//
// Created by ethan on 17/2/26.
//


#include <core/Error.hpp>
#include <core/VFS.hpp> // To get the project working dir

#include <array>

// Both libs sued for writing to log files
#include <iostream>
#include <fstream>


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

    // Check if LoggerFormat::ImmediateMode is disabled
    // If so, cache log and skip outputting
    m_CacheClock.update();
    if (!hasFlag(finalFormat, LoggerFormat::ImmediateMode)) {
        auto now = m_CacheClock.elapsed();

        // Cleanup expired entries
        for (auto it = m_cachedLogs.begin(); it != m_cachedLogs.end();) {
            if (it->second.expTime <= now) {
                it = m_cachedLogs.erase(it);
            } else {
                ++it;
            }
        }

        //Handle incoming log
        auto it = m_cachedLogs.find(finalLog);

        if (it == m_cachedLogs.end()) {
            // First time seen --> output immediately
            //logToSink(finalLog);
            writeToSinks(finalLog, finalCol, finalOutputs);

            m_cachedLogs.emplace(finalLog, LogCacheEntry{
            now + delayCahceTime,
            false       // Repeat that not emitted yet
            });

            return;
        } else {
            // Seen again within time window
            auto& entry = it->second;

            if (!entry.hasOutput) {
                finalLog.insert(0, "[~]");
                //logToSinks(finalLog);
                writeToSinks(finalLog, finalCol, finalOutputs);

                entry.hasOutput = true;
            }

            // Refresh suppression window
            entry.expTime = now + delayCahceTime;
        }

        return;
    }

    writeToSinks(finalLog, finalCol, finalOutputs);

}

void Dexium::Core::Logger::writeToSinks(const std::string &logMsg, fmt::color color, LoggerOutput sinks) {
    // Formatting complete, output to ALL provided output streams
    if (hasFlag(sinks, LoggerOutput::Stderr)) {
        fmt::print(stderr, fg(color), logMsg + "\n");
    }
    if (hasFlag(sinks, LoggerOutput::Stdout)) {
        fmt::print(stdout, fg(color), logMsg + "\n");
    }
    if (hasFlag(sinks, LoggerOutput::DevConsole)) {
        // Currently No DevConsole impelented, relog msg onto Stderr and provide warning
        //TraceLog(LogLevel::ERROR, LoggerOutput::Stderr, "[Logger]: DevConsole si currently not implemented. Falling back to Stderr as the output");
        fmt::print(stderr, fg(TColours.error), "[Logger]: DevConsole is currently not implemented. falling back to Stderr(Default)");
        // Relog the raw msg
        //TraceLog(type, LoggerOutput::Stderr, finalFormat, msg);
        fmt::print(stderr, fg(color), logMsg + "\n");
    }
    if (hasFlag(sinks, LoggerOutput::File)) {
        //this->writeLog(type, finalLog);
    }
    // No need to check for LoggerOutput::None, this is done at the start of the fn as an early exit(Logging is ingored on this flag)

}


void Dexium::Core::Logger::writeLog(LogLevel type, const std::string &msg) {
    // Takes a formatted log msg, prepends with date & time and outputs to a log file
    // If the Log Folder doesnt exist, it will create it.

    // Check LogFolder existence or create existence
    std::filesystem::path logDir(VFS::getExecutablePath().string() + LogFolderName + "/");
    logDir = logDir.lexically_normal();

    if (!std::filesystem::exists(logDir)) {
        // Log Folde rnot found, create it
        std::error_code ec;
        if (!std::filesystem::create_directories(logDir, ec)) {
            // Failed to create path
            if (ec) {
                // Genuine error, report it
                fmt::print(stderr, fg(TColours.error), "[Error]: [Logger][WriteToFile]: Failed to create directory '{}'\nReason: {}", logDir.string(), ec.message());
            }
            // File/Path already exists, continue
        }
        //Directory created sucessfully, continue
    }

    // Create logFileName from todays dat and filePrefix
    Utils::localTime l_time{};
    const std::string fileName = fmt::format("{}-{}-{}-{}.log", logfilePrefix, l_time.Day, l_time.Month, l_time.Year);

    // Check if a log for today already created, if not make a new one
    logDir /= std::filesystem::path("/" + fileName).lexically_normal();

    std::ofstream logFile(logDir.string());

    if (logFile.is_open()) {
        //File open, now write log
        logFile << fmt::format("[{}:{}:{} {}]: {}\n", l_time.Hr.to12(), l_time.Minute, l_time.Second, l_time.Hr.am_pm(), msg);
    } else {
        // file failed to open, fallback method
        fmt::print(stderr, fg(TColours.error), "[Error]: Failed to write to log file! Using fallback sink!(Stderr)\n");
        fmt::print(stderr, fg(TColours.warning), "{}\n", msg);
    }

    // CLose file
    logFile.close();



}
