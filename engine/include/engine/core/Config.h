#pragma once

#include <engine/core/Json.h>
#include <engine/core/Log.h>
#include <string>
#include <string_view>

namespace eng {

struct BootConfig {
    // Window
    int windowWidth = 1280;
    int windowHeight = 720;
    std::string windowTitle = "Infinigen";

    // Logging
    LogLevel logThreshold = LogLevel::Info;
    std::string logFile = "logs/engine.log";

    // Tunables
    int logBufferCapacity = 4096; // messages kept for the Console
    int gizmoCircleSegments = 24; // how round a drawn circle looks
    float fixedTimestepSeconds = 1.0f / 60.0f;
    int maxStepsPerFrame = 5;

    // Startup
    std::string startupScene = "scenes/orbit_test.json";
};

bool LoadBootConfig(std::string_view virtualPath, BootConfig& outConfig, Json& outDocument, std::string& outError);

} // namespace eng