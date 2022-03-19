#pragma once

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
  inline PropertyType getType() const { return mType; }

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
   * @tvalue ValueType Value type
   * @return Property value
   */
  template <class ValueType> inline const ValueType getValue() const {
    LIQUID_ASSERT(false, "Property type is not supported");

    return std::any_cast<ValueType>(mValue);
  }

private:
  std::any mValue;

  PropertyType mType;
};

template <> inline const int32_t Property::getValue() const {
  LIQUID_ASSERT(mType == INT32, "Property type is not int32");

  return std::any_cast<int32_t>(mValue);
}

template <> inline const uint32_t Property::getValue() const {
  LIQUID_ASSERT(mType == UINT32, "Property type is not uint32");

  return std::any_cast<uint32_t>(mValue);
}

template <> inline const uint64_t Property::getValue() const {
  LIQUID_ASSERT(mType == UINT64, "Property type is not uint64");

  return std::any_cast<uint64_t>(mValue);
}

template <> inline const float Property::getValue() const {
  LIQUID_ASSERT(mType == REAL, "Property type is not a real number");

  return std::any_cast<float>(mValue);
}

template <> inline const glm::vec2 Property::getValue() const {
  LIQUID_ASSERT(mType == VECTOR2, "Property type is not Vector2");

  return std::any_cast<glm::vec2>(mValue);
}

template <> inline const glm::vec3 Property::getValue() const {
  LIQUID_ASSERT(mType == VECTOR3, "Property type is not Vector3");

  return std::any_cast<glm::vec3>(mValue);
}

template <> inline const glm::vec4 Property::getValue() const {
  LIQUID_ASSERT(mType == VECTOR4, "Property type is not Vector4");

  return std::any_cast<glm::vec4>(mValue);
}

template <> inline const glm::mat4 Property::getValue() const {
  LIQUID_ASSERT(mType == MATRIX4, "Property type is not Matrix4");

  return std::any_cast<glm::mat4>(mValue);
}

} // namespace liquid
