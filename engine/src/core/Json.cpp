#include <engine/core/Json.h>

namespace eng {

Json ParseJson(std::string_view text, std::string& outError) {
    return Json::object();
}

int ReadInt(const Json& object, std::string_view key, int fallback, std::string_view where) {
    return fallback;
}

float ReadFloat(const Json& object, std::string_view key, float fallback, std::string_view where) {
    return fallback;
}

bool ReadBool(const Json& object, std::string_view key, bool fallback, std::string_view where) {
    return fallback;
}

std::string ReadString(const Json& object, std::string_view key, std::string_view fallback, std::string_view where) {
    return std::string(fallback);
}

Vec2 ReadVec2(const Json& object, std::string_view key, Vec2 fallback, std::string_view where) {
    return fallback;
}

bool HasKey(const Json& object, std::string_view key) {
    return false;
}

void WriteVec2(Json& object, std::string_view key, Vec2 value) {
}

} // namespace eng
