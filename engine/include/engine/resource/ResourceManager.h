#pragma once

#include <engine/core/Subsystem.h>
#include <engine/render/Texture.h>
#include <string>
#include <string_view>

namespace eng {

class ResourceManager : public Subsystem {
public:
    bool Init(const BootConfig& config) override;
    void Shutdown() override;
    static TextureRef LoadTexture(std::string_view virtualPath);
    static TextureRef MissingTexture();
    static std::size_t LoadedCount();
    static void PruneCache();
};

} // namespace eng
