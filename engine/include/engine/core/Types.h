#pragma once

// =============================================================================
//  Fixed-width types and a few aliases used across the whole engine.
//
//  Coming from C#: `int` in C# is always 32 bits. In C++ it is "at least 16,
//  usually 32, and the standard will not promise you more than that." When the
//  width matters - and in an engine it usually does - say the width.
//
//  You will revisit this file in Week 4 when you start measuring sizeof().
// =============================================================================

#include <cstddef>
#include <cstdint>

namespace eng {

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
using f64 = double;

using usize = std::size_t;

} // namespace eng
