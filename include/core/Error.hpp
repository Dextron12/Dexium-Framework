//
// Created by Dextron12 on 5/10/2025.
//

#ifndef DEXIUM_ERROR_HPP
#define DEXIUM_ERROR_HPP

#include <fmt/core.h>
#include <fmt/color.h>



namespace Dexium {

    enum LogLevel {
        LOG_TRACE = 1,      //< TRACE: debug or tracking messages
        LOG_DEBUG = 2,      //< DEBUG: messages only output in debug mode
        LOG_INFO = 3,       //< INFO: general engine state information
        LOG_WARNING = 4,    //< WARNING: minor failure of engine, or incorrect usage of function/class
        LOG_ERROR = 5,      //< ERROR: major failure, affected data likely lost(but engine/application can still operate without lost data)
        LOG_FATAL = 6,      //< FATAL-ERROR: resource/state could not load, application will likely fail without this data (likely through a seg fault). In rel mode, will call abort() after dumping a logFile
        LOG_NONE = 0        //< Disable/Enable all logging (engine-wide)
    };

    template <typename... Args>
    void TraceLog(LogLevel level, fmt::format_string<Args...> fmtStr, Args&&... args);


}


#endif //DEXIUM_ERROR_HPP