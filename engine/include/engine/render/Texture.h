#pragma once

#include <memory>
#include <string>

namespace eng {

struct Texture {
    std::string path;
    int width = 0;
    int height = 0;
    void* native = nullptr;
    bool isPlaceholder = false;
    ~Texture();
};

using TextureRef = std::shared_ptr<Texture>;

} // namespace eng
