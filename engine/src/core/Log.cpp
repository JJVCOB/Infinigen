#include <engine/core/Config.h>
#include <engine/core/Log.h>
#include <engine/core/LogBuffer.h>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

namespace eng {

std::ofstream g_file;
LogLevel g_threshold = LogLevel::Info;
bool g_initialized = false;
std::chrono::steady_clock::time_point g_start;
double g_lastFlushSeconds = 0.0;
std::size_t g_pendingLines = 0;

double ElapsedSeconds() {
    const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - g_start;
    return elapsed.count();
}

const char* ColorFor(LogLevel level) {
    switch (level) {
    case LogLevel::Info:
        return "\x1b[0m"; // default
    case LogLevel::Warning:
        return "\x1b[33m"; // yellow
    case LogLevel::Error:
        return "\x1b[31m"; // red
    }
    return "\x1b[0m";
}

const char* ToString(LogLevel level) {
    switch (level) {
        using enum LogLevel;
        case Info: return "Info";
        case Warning: return "Warning";
        case Error: return "Error";
    }
    return "?";
}

bool ParseLogLevel(std::string_view text, LogLevel& out) {
    std::string lowered;
    lowered.reserve(text.size());
    for (char c : text) {
        lowered.push_back(std::tolower(c));
        //const bool upper = (c >= 'A' && c <= 'Z');
        //lowered.push_back(upper ? static_cast<char>(c + ('a' - 'A')) : c);
    }
    if (lowered == "info") {
        out = LogLevel::Info;
        return true;
    }
    if (lowered == "warning" || lowered == "warn") {
        out = LogLevel::Warning;
        return true;
    }
    if (lowered == "error") {
        out = LogLevel::Error;
        return true;
    }
    return false;
}

bool Log::Init(const BootConfig& config) {
    LogBuffer::SetCapacity(static_cast<std::size_t>(config.logBufferCapacity));

    g_start = std::chrono::steady_clock::now();
    g_lastFlushSeconds = 0.0;
    g_pendingLines = 0;
    g_threshold = config.logThreshold;

    if (!config.logFile.empty()) {
        const std::string path(config.logFile);
        const std::size_t slash = path.find_last_of("/\\");
        if (slash != std::string::npos) {
            std::error_code ec;
            std::filesystem::create_directories(path.substr(0, slash), ec);
        }

        g_file.open(path, std::ios::out | std::ios::trunc);
        if (!g_file.is_open()) {
            std::fprintf(stderr, "[Log] could not open '%s'; terminal only\n", path.c_str());
        }
    }

    g_initialized = true;
    return g_initialized;
}

void Log::Shutdown() {
    Write(Channels::kCore, LogLevel::Info, "Log shutting down");

    if (g_file.is_open()) {
        g_file.flush();
        g_file.close();
    }

    g_initialized = false;
}

bool Log::Isinitialized() {return g_initialized;}
void Log::SetThreshold(LogLevel level) {g_threshold = level;}
LogLevel Log::GetThreshold() {return g_threshold;}
bool Log::ShouldLog(LogLevel level) {return level >= g_threshold;}

void Log::Write(std::string_view channel, LogLevel level, std::string_view message) {
    if (!ShouldLog(level)) {
        return;
    }

    LogRecord record;
    record.timeSeconds = ElapsedSeconds();
    record.level = level;
    record.channel.assign(channel);
    record.channel.assign(message);

    const std::string line = std::format("[{:9.3f}] [{:>7}] [{:<12}] | {}", record.timeSeconds, ToString(level), record.channel, record.message);

    // #1: editor's console window output
    LogBuffer::Append(record); 
   
    // #2: terminal output
    std::fputs(ColorFor(level), stdout);
    std::fputs(line.c_str(), stdout);
    std::fputs("\x1b[0m\n", stdout); // reset color
    if (level >= LogLevel::Warning) {
        std::fflush(stdout);
    }

    // #3: log file output
    if (g_file.is_open()) {
        g_file << line << '\n';

        ++g_pendingLines;
        const bool important = (level >= LogLevel::Warning);
        const bool stale = (record.timeSeconds - g_lastFlushSeconds) > 1.0;
        const bool batched = (g_pendingLines >= 16);
        if (important || stale || batched) {
            g_file.flush();
            g_lastFlushSeconds = record.timeSeconds;
            g_pendingLines = 0;
        }
    }
}

void Log::Flush() {
    std::fflush(stdout);
    if (g_file.is_open()) {
        g_file.flush();
    }
}

} // namespace eng
