#pragma once

#include <engine/math/Vec2.h>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

namespace eng {

using Json = nlohmann::json;
Json ParseJson(std::string_view text, std::string& outError);
int ReadInt(const Json& object, std::string_view key, int fallback, std::string_view where = "");
float ReadFloat(const Json& object, std::string_view key, float fallback, std::string_view where = "");
bool ReadBool(const Json& object, std::string_view key, bool fallback, std::string_view where = "");
std::string ReadString(const Json& object, std::string_view key, std::string_view fallback, std::string_view where = "");
Vec2 ReadVec2(const Json& object, std::string_view key, Vec2 fallback, std::string_view where = "");
bool HasKey(const Json& object, std::string_view key);
void WriteVec2(Json& object, std::string_view key, Vec2 value);

} // namespace eng
