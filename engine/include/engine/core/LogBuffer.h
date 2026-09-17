#pragma once

#include <engine/core/Log.h>
#include <string>
#include <vector>

namespace eng {

struct LogRecord {
    unsigned long long sequence = 0;
    double timeSeconds = 0.0; // seconds since Log::Init
    LogLevel level = LogLevel::Info;
    std::string channel;
    std::string message;
};

class LogBuffer {
public:
    static constexpr std::size_t kDefaultCapacity = 4096;
    static void        SetCapacity(std::size_t capacity);
    static std::size_t Capacity();
    static void Append(const LogRecord& record);
    static void Snapshot(std::vector<LogRecord>& out);
    static void Channels(std::vector<std::string>& out);
    static std::size_t        Count();
    static unsigned long long TotalWritten();   // includes dropped messages
    static void               Clear();
};

} // namespace eng
