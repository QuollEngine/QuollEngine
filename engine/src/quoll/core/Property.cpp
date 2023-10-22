#include "quoll/core/Base.h"
#include "Property.h"

namespace quoll {

Property::Property(i32 value) : mType(INT32), mValue(value) {}
Property::Property(u32 value) : mType(UINT32), mValue(value) {}
Property::Property(u64 value) : mType(UINT64), mValue(value) {}
Property::Property(f32 value) : mType(REAL), mValue(value) {}
Property::Property(const glm::vec2 &value) : mType(VECTOR2), mValue(value) {}
Property::Property(const glm::vec3 &value) : mType(VECTOR3), mValue(value) {}
Property::Property(const glm::vec4 &value) : mType(VECTOR4), mValue(value) {}
Property::Property(const glm::mat4 &value) : mType(MATRIX4), mValue(value) {}

usize Property::getSize() const {
  switch (mType) {
  case INT32:
    return sizeof(i32);
  case UINT32:
    return sizeof(u32);
  case UINT64:
    return sizeof(u64);
  case REAL:
    return sizeof(f32);
  case VECTOR2:
    return sizeof(glm::vec2);
  case VECTOR3:
    return sizeof(glm::vec3);
  case VECTOR4:
    return sizeof(glm::vec4);
  case MATRIX4:
    return sizeof(glm::mat4);
  default:
    return 0;
  }
}

const String Property::toString() const {
  switch (mType) {
  case INT32:
    return std::to_string(getValue<i32>());
  case UINT32:
    return std::to_string(getValue<u32>());
  case UINT64:
    return std::to_string(getValue<u64>());
  case REAL: {
    std::stringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(2);
    ss << getValue<f32>();
    return ss.str();
  }
  case VECTOR2: {
    auto &val = getValue<glm::vec2>();
    std::stringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(2);
    ss << "(" << val[0] << ", " << val[1] << ")";
    return ss.str();
  }
  case VECTOR3: {
    auto &val = getValue<glm::vec3>();
    std::stringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(2);
    ss << "(" << val[0] << ", " << val[1] << ", " << val[2] << ")";
    return ss.str();
  }
  case VECTOR4: {
    auto &val = getValue<glm::vec4>();
    std::stringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(2);
    ss << "(" << val[0] << ", " << val[1] << ", " << val[2] << ", " << val[3]
       << ")";
    return ss.str();
  }
  case MATRIX4: {
    auto &val = getValue<glm::mat4>();
    std::stringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(2);
    ss << "[";
    for (glm::mat4::length_type i = 0; i < 4; ++i) {
      ss << "[" << val[i][0] << ", " << val[i][1] << ", " << val[i][2] << ", "
         << val[i][3] << "]";

      if (i < 3) {
        ss << ", ";
      }
    }
    ss << "]";

    return ss.str();
  }

  default:
    return "(unknown value)";
  }
}

} // namespace quoll
