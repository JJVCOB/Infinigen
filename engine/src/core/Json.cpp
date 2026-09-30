#include <engine/core/Json.h>
#include <engine/core/Log.h>

namespace eng {
namespace {

std::string Describe(std::string_view where, std::string_view key) {
    if (where.empty()) { return std::string(key); }
    return std::string(where) + "." + std::string(key);
}

const Json* Lookup(const Json& object, std::string_view key) {
    if (!object.is_object()) { return nullptr; }
    const auto it = object.find(std::string(key));
    return (it != object.end()) ? &(*it) : nullptr;
}

} // namespace

Json ParseJson(std::string_view text, std::string& outError) {
    Json document = Json::parse(text, nullptr, false, true);

    if (document.is_discarded()) {
        outError = "The file is not a valid JSON. Check formatting and try again.";
        return Json::object();
    }

    outError.clear();
    return document;
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
