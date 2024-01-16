#pragma once

namespace quoll {

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
  Property(i32 value);

  Property(u32 value);

  Property(u64 value);

  Property(f32 value);

  Property(const glm::vec2 &value);

  Property(const glm::vec3 &value);

  Property(const glm::vec4 &value);

  Property(const glm::mat4 &value);

  inline PropertyType getType() const { return mType; }

  usize getSize() const;

  const String toString() const;

  template <class TValueType> inline const TValueType getValue() const {
    QuollAssert(false, "Property type is not supported");

    return std::any_cast<TValueType>(mValue);
  }

private:
  std::any mValue;

  PropertyType mType;
};

template <> inline const i32 Property::getValue() const {
  QuollAssert(mType == INT32, "Property type is not int32");

  return std::any_cast<i32>(mValue);
}

template <> inline const u32 Property::getValue() const {
  QuollAssert(mType == UINT32, "Property type is not uint32");

  return std::any_cast<u32>(mValue);
}

template <> inline const u64 Property::getValue() const {
  QuollAssert(mType == UINT64, "Property type is not uint64");

  return std::any_cast<u64>(mValue);
}

template <> inline const f32 Property::getValue() const {
  QuollAssert(mType == REAL, "Property type is not a real number");

  return std::any_cast<f32>(mValue);
}

template <> inline const glm::vec2 Property::getValue() const {
  QuollAssert(mType == VECTOR2, "Property type is not Vector2");

  return std::any_cast<glm::vec2>(mValue);
}

template <> inline const glm::vec3 Property::getValue() const {
  QuollAssert(mType == VECTOR3, "Property type is not Vector3");

  return std::any_cast<glm::vec3>(mValue);
}

template <> inline const glm::vec4 Property::getValue() const {
  QuollAssert(mType == VECTOR4, "Property type is not Vector4");

  return std::any_cast<glm::vec4>(mValue);
}

template <> inline const glm::mat4 Property::getValue() const {
  QuollAssert(mType == MATRIX4, "Property type is not Matrix4");

  return std::any_cast<glm::mat4>(mValue);
}

} // namespace quoll
