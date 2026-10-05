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
    const Json* value = Lookup(object, key);

    if (value == nullptr) {
        return fallback;
    }

    if (!value->is_number_integer()) {
        ENGINE_LOG_WARN(Channels::kConfig, "{} should be a whole number. Using {}.", Describe(where, key), fallback);
        return fallback;
    }

    return value->get<int>();
}

float ReadFloat(const Json& object, std::string_view key, float fallback, std::string_view where) {
    const Json* value = Lookup(object, key);

    if (value == nullptr) {
        return fallback;
    }

    if (!value->is_number()) {
        ENGINE_LOG_WARN(Channels::kConfig, "{} should be a number. Using {}.", Describe(where, key), fallback);
        return fallback;
    }

    return value->get<float>();
}

bool ReadBool(const Json& object, std::string_view key, bool fallback, std::string_view where) {
    const Json* value = Lookup(object, key);

    if (value == nullptr) {
        return fallback;
    }

    if (!value->is_boolean()) {
        ENGINE_LOG_WARN(Channels::kConfig, "{} should be a boolean. Using {}.", Describe(where, key), fallback);
        return fallback;
    }

    return value->get<bool>();
}

std::string ReadString(const Json& object, std::string_view key, std::string_view fallback, std::string_view where) {
    const Json* value = Lookup(object, key);

    if (value == nullptr) {
        return std::string(fallback);
    }

    if (!value->is_string()) {
        ENGINE_LOG_WARN(Channels::kConfig, "{} should be a string. Using {}.", Describe(where, key), fallback);
        return std::string(fallback);
    }

    return value->get<std::string>();
}

Vec2 ReadVec2(const Json& object, std::string_view key, Vec2 fallback, std::string_view where) {
    const Json* value = Lookup(object, key);

    if (value == nullptr) {
        return fallback;
    }

    if (!value->is_array() || value->size() != 2 || !(*value)[0].is_number() || !(*value)[1].is_number()) {
        ENGINE_LOG_WARN(Channels::kConfig, "{} should be a Vector2 like [1, 2]. Using [{}, {}].", Describe(where, key), fallback.x, fallback.y);
        return fallback;
    }

    return Vec2{(*value)[0].get<float>(), (*value)[1].get<float>()};
}

bool HasKey(const Json& object, std::string_view key) { return Lookup(object, key) != nullptr; }

void WriteVec2(Json& object, std::string_view key, Vec2 value) {
    Json pair = Json::array();
    pair.push_back(value.x);
    pair.push_back(value.y);
    object[std::string(key)] = std::move(pair);
}

} // namespace eng
