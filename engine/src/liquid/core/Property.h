#pragma once

#include "liquid/core/Base.h"

namespace liquid {

class Property {
public:
  enum PropertyType {
    INT32,
    UINT32,
    UINT64,
    REAL,
    VECTOR2,
    VECTOR3,
    VECTOR4,
    MATRIX4
  };

public:
  /**
   * @brief Create 32bit integer property
   *
   * @param value 32bit integer value
   */
  Property(int32_t value);

  /**
   * @brief Create 32bit unsigned  integer property
   *
   * @param value 32bit unsigned integer value
   */
  Property(uint32_t value);

  /**
   * @brief Create 64bit unsigned integer property
   *
   * @param value 64bit unsigned integer value
   */
  Property(uint64_t value);

  /**
   * @brief Create real property
   *
   * @param value Real value
   */
  Property(float value);

  /**
   * @brief Create vector2 property
   *
   * @param value Vector2 value
   */
  Property(const glm::vec2 &value);

  /**
   * @brief Create vector3 property
   *
   * @param value Vector3 value
   */
  Property(const glm::vec3 &value);

  /**
   * @brief Create vector4 property
   *
   * @param value Vector4 value
   */
  Property(const glm::vec4 &value);

  /**
   * @brief Create matrix4 property
   *
   * @param value Matrix4 value
   */
  Property(const glm::mat4 &value);

  /**
   * @brief Get type
   *
   * @return Property type
   */
  inline PropertyType getType() const { return type; }

  /**
   * @brief Get size
   *
   * @return Property size
   */
  size_t getSize() const;

  /**
   * @brief Get value in string
   *
   * @return Property value string
   */
  const String toString() const;

  /**
   * @brief Get value
   *
   * @tvalue VertexType Value type
   * @return Property value
   */
  template <class ValueType> inline const ValueType getValue() const {
    throw std::invalid_argument("Property type is not supported");
  }

private:
  std::any value;

  PropertyType type;
};

template <> inline const int32_t Property::getValue() const {
  if (type != INT32) {
    throw std::runtime_error("Property type is not int32");
  }

  return std::any_cast<int32_t>(value);
}

template <> inline const uint32_t Property::getValue() const {
  if (type != UINT32) {
    throw std::runtime_error("Property type is not uint32");
  }

  return std::any_cast<uint32_t>(value);
}

template <> inline const uint64_t Property::getValue() const {
  if (type != UINT64) {
    throw std::runtime_error("Property type is not uint64");
  }

  return std::any_cast<uint64_t>(value);
}

template <> inline const float Property::getValue() const {
  if (type != REAL) {
    throw std::runtime_error("Property type is not a real number");
  }

  return std::any_cast<float>(value);
}

template <> inline const glm::vec2 Property::getValue() const {
  if (type != VECTOR2) {
    throw std::runtime_error("Property type is not Vector2");
  }
  return std::any_cast<glm::vec2>(value);
}

template <> inline const glm::vec3 Property::getValue() const {
  if (type != VECTOR3) {
    throw std::runtime_error("Property type is not Vector3");
  }

  return std::any_cast<glm::vec3>(value);
}

template <> inline const glm::vec4 Property::getValue() const {
  if (type != VECTOR4) {
    throw std::runtime_error("Property type is not Vector4");
  }
  return std::any_cast<glm::vec4>(value);
}

template <> inline const glm::mat4 Property::getValue() const {
  if (type != MATRIX4) {
    throw std::runtime_error("Property type is not Matrix4");
  }
  return std::any_cast<glm::mat4>(value);
}

} // namespace liquid
