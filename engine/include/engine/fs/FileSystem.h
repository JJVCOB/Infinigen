#pragma once

#include <engine/core/Subsystem.h>
#include <string>
#include <string_view>
#include <vector>

namespace eng {

class FileSystem : public Subsystem {
public:
    bool Init(const BootConfig& config) override;
    void Shutdown() override;
    static const std::string& AssetRoot();
    static std::string Resolve(std::string_view virtualPath);
    static bool Exists(std::string_view virtualPath);
    static bool ListFiles(std::string_view virtualDirectory, std::string_view extension, std::vector<std::string>& out);

    struct DirEntry {
        std::string       name;          // just the last part, for display
        std::string       virtualPath;   // ready to pass back to ReadFile
        bool              isDirectory = false;
        unsigned long long byteSize   = 0;   // 0 for folders
    };

    static bool ListDirectory(std::string_view virtualDirectory, std::vector<DirEntry>& out);
    static bool CreateDirectory(std::string_view virtualDirectory, std::string& outError);
    static bool ReadTextFile(std::string_view virtualPath, std::string& outText, std::string& outError);
    static bool ReadFile(std::string_view virtualPath, std::vector<unsigned char>& outBytes, std::string& outError);
    static bool WriteTextFile(std::string_view virtualPath, std::string_view text, std::string& outError);
};

} // namespace eng