#pragma once

#include <string>
#include <string_view>
#include <engine/core/Subsystem.h>

namespace eng {

class ScriptLibrary : public Subsystem {
public:
    bool Init(const BootConfig& config) override;
    void Shutdown() override;
    static std::string DefaultVirtualPath();
    static bool Load(std::string_view virtualPath, std::string& outError);
    static void Unload();
    static bool IsLoaded();
    static const std::string& LoadedPath();
    static std::size_t ScriptCount();
};

} // namespace eng
