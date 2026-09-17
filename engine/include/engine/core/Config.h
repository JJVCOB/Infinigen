#pragma once

#include <engine/core/Json.h>
#include <engine/core/Log.h>
#include <string>
#include <string_view>

namespace eng {

struct BootConfig {
    // window
    int windowWidth  = 1280;
    int windowHeight = 720;
    std::string windowTitle = "Infinigen";

    // logging
    LogLevel logThreshold = LogLevel::Info;
    std::string logFile = "logs/engine.log";

    // tunables
    int logBufferCapacity = 4096; // messages kept for the Console
    int gizmoCircleSegments  = 24; // how round a drawn circle looks
    float fixedTimestepSeconds = 1.0f / 60.0f;
    int maxStepsPerFrame = 5;

    // startup
    std::string startupScene = "scenes/orbit_test.json";
};

bool LoadBootConfig(std::string_view virtualPath, BootConfig& outConfig, Json& outDocument, std::string& outError);

} // namespace eng