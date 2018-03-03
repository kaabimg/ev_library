#pragma once

#include <string>

namespace evt {

enum class LogCategory { Info, Warning, Error, Debug };

struct LogMessage {
    std::string msg;
    LogCategory category;
};

inline const char* toString(LogCategory c)
{
    switch (c) {
        case LogCategory::Info: return "Info";
        case LogCategory::Warning: return "Warning";
        case LogCategory::Error: return "Error";
        case LogCategory::Debug: return "Debug";
        default: break;
    }
}
}
