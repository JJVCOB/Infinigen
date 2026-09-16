#pragma once

#include <engine/core/Subsystem.h>
#include <format>
#include <string_view>

namespace eng {

enum class LogLevel {
    Info,      // ordinary progress: "scene loaded", "window created"
    Warning,   // something looks wrong but the program continues
    Error,     // something failed; a feature will not work
};

const char* ToString(LogLevel level);
bool ParseLogLevel(std::string_view text, LogLevel& out);

namespace Channels {
inline constexpr std::string_view kCore     = "Core";
inline constexpr std::string_view kPlatform = "Platform";
inline constexpr std::string_view kInput    = "Input";
inline constexpr std::string_view kRender   = "Render";
inline constexpr std::string_view kFileSys  = "FileSystem";
inline constexpr std::string_view kResource = "Resource";
inline constexpr std::string_view kScene    = "Scene";
inline constexpr std::string_view kPhysics  = "Physics";
inline constexpr std::string_view kConfig   = "Config";
inline constexpr std::string_view kEditor   = "Editor";
inline constexpr std::string_view kGame     = "Game";
} // namespace Channels

class Log : public Subsystem {
public:
    bool Init(const BootConfig& config) override;
    void Shutdown() override;
    static bool IsInitialised();
    static void Write(std::string_view channel, LogLevel level, std::string_view message);
    static void SetThreshold(LogLevel level);
    static LogLevel GetThreshold();
    static bool ShouldLog(LogLevel level);
    static void Flush();
};

} // namespace eng

#define ENGINE_LOG(channel, level, ...)                                        \
    do {                                                                       \
        if (::eng::Log::ShouldLog(level)) {                                    \
            ::eng::Log::Write((channel), (level), ::std::format(__VA_ARGS__));  \
        }                                                                      \
    } while (false)

#define ENGINE_LOG_INFO(channel, ...)  ENGINE_LOG(channel, ::eng::LogLevel::Info,    __VA_ARGS__)
#define ENGINE_LOG_WARN(channel, ...)  ENGINE_LOG(channel, ::eng::LogLevel::Warning, __VA_ARGS__)
#define ENGINE_LOG_ERROR(channel, ...) ENGINE_LOG(channel, ::eng::LogLevel::Error,   __VA_ARGS__)
