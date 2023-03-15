#pragma once

#include "liquid/entity/Entity.h"

namespace glm {

template <length_t L, typename T, qualifier Q>
void PrintTo(const vec<L, T, Q> &value, std::ostream *out) {
  *out << to_string(value);
}

template <length_t C, length_t R, typename T, qualifier Q>
void PrintTo(const mat<C, R, T, Q> &value, std::ostream *out) {
  *out << to_string(value);
}

template <typename T, qualifier Q>
void PrintTo(const qua<T, Q> &value, std::ostream *out) {
  *out << to_string(value);
}

} // namespace glm

namespace liquid {

static void PrintTo(Entity entity, std::ostream *out) {
  *out << static_cast<uint32_t>(entity);
}

} // namespace liquid

#include <gtest/gtest.h>
#include <gmock/gmock.h>
