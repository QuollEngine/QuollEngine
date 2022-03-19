#include "liquid/core/Base.h"
#include "PhysicalDeviceInformation.h"

namespace liquid {

PhysicalDeviceInformation::PhysicalDeviceInformation(
    const String &name, PhysicalDeviceType type,
    const UnorderedPropertyMap &properties, const UnorderedPropertyMap &limits)
    : mName(name), mType(type), mProperties(properties), mLimits(limits) {}

const String PhysicalDeviceInformation::getTypeString() const {
  switch (mType) {
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
