#include "PhysicalDeviceInformation.h"

namespace liquid {

PhysicalDeviceInformation::PhysicalDeviceInformation(
    const String &name_, PhysicalDeviceType type_,
    const UnorderedPropertyMap &properties_,
    const UnorderedPropertyMap &limits_)
    : name(name_), type(type_), properties(properties_), limits(limits_) {}

const String PhysicalDeviceInformation::getTypeString() const {
  switch (type) {
  case PhysicalDeviceType::DISCRETE_GPU:
    return "Discrete GPU";
  case PhysicalDeviceType::INTEGRATED_GPU:
    return "Integrated GPU";
  case PhysicalDeviceType::VIRTUAL_GPU:
    return "Virtual GPU";
  case PhysicalDeviceType::CPU:
    return "CPU";
  default:
    return "Unknown";
  }
}

} // namespace liquid
