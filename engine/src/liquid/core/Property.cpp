#include "liquid/core/Base.h"
#include "Property.h"

namespace liquid {

Property::Property(int32_t value) : mType(INT32), mValue(value) {}
Property::Property(uint32_t value) : mType(UINT32), mValue(value) {}
Property::Property(uint64_t value) : mType(UINT64), mValue(value) {}
Property::Property(float value) : mType(REAL), mValue(value) {}
Property::Property(const glm::vec2 &value) : mType(VECTOR2), mValue(value) {}
Property::Property(const glm::vec3 &value) : mType(VECTOR3), mValue(value) {}
Property::Property(const glm::vec4 &value) : mType(VECTOR4), mValue(value) {}
Property::Property(const glm::mat4 &value) : mType(MATRIX4), mValue(value) {}

size_t Property::getSize() const {
  switch (mType) {
  case INT32:
    return sizeof(int32_t);
  case UINT32:
    return sizeof(uint32_t);
  case UINT64:
    return sizeof(uint64_t);
  case REAL:
    return sizeof(float);
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
    return std::to_string(getValue<int32_t>());
  case UINT32:
    return std::to_string(getValue<uint32_t>());
  case UINT64:
    return std::to_string(getValue<uint64_t>());
  case REAL: {
    std::stringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(2);
    ss << getValue<float>();
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

} // namespace liquid
