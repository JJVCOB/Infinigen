#include <engine/core/Config.h>
#include <engine/core/Json.h>
#include <engine/core/Log.h>
#include <engine/fs/FileSystem.h>

namespace eng {

namespace {

const Json& Section(const Json& document, const char* name) {
    static const Json kEmpty = Json::object();
    if (!document.is_object()) {
        return kEmpty;
    }
    const auto it = document.find(name);
    return (it != document.end() && it->is_object()) ? *it : kEmpty;
}

} // namespace

bool LoadBootConfig(std::string_view virtualPath, BootConfig& outConfig, Json& outDocument, std::string& outError) {
    std::string text;
    std::string readError;

    if (!FileSystem::ReadTextFile(virtualPath, text, readError)) {
        outError = "No settings file at '" + std::string(virtualPath) + "', using built-in defaults.";
        ENGINE_LOG_WARN(Channels::kConfig, "{}", outError);
        return true;
    }

    std::string parseError;
    Json document = ParseJson(text, parseError);

    if (!parseError.empty()) {
        outError = std::string(virtualPath) + ": " + parseError;
        ENGINE_LOG_ERROR(Channels::kConfig, "{}", outError);
        return false;
    }

    // Window
    const Json& window = Section(document, "window");
    outConfig.windowWidth = ReadInt(window, "width", outConfig.windowWidth, "window");
    outConfig.windowHeight = ReadInt(window, "height", outConfig.windowHeight, "window");
    

    // Logging
    const Json& logging = Section(document, "logging");
    outConfig.logFile = ReadString(logging, "file", outConfig.logFile, logging);
    const std::string thresholdText = ReadString(logging, "threshold", ToString(outConfig.logThreshold), "logging");

    if (!ParseLogLevel(thresholdText, outConfig.logThreshold)) {
        ENGINE_LOG_WARN(Channels::kConfig, "Logging threshold is '{}', which is not Info, Warning, or Error. Using {}", thresholdText, ToString(outConfig.logThreshold));
    }

    // Tunables
    const Json& tunables = Section(document, "tunables");
    outConfig.logBufferCapacity = ReadInt(tunables, "logBufferCapacity", outConfig.logBufferCapacity, "tunables");
    outConfig.gizmoCircleSegments = ReadInt(tunables, "gizmoCircleSegments", outConfig.gizmoCircleSegments, "tunables");
    outConfig.fixedTimestepSeconds = ReadFloat(tunables, "fixedTimestepSeconds", outConfig.fixedTimestepSeconds, "tunables");
    outConfig.maxStepsPerFrame = ReadInt(tunables, "maxStepsPerFrame", outConfig.maxStepsPerFrame, "tunables");

    // Startup
    outConfig.startupScene = ReadString(Section(document, "startup"), "scene", outConfig.startupScene, "startup");

    outDocument = std::move(document);
    outError.clear();
    ENGINE_LOG_INFO(Channels::kConfig, "Settings successfully loaded from '{}'", virtualPath);

    return true;
}

} // namespace eng
