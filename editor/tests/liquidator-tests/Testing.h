#pragma once

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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
