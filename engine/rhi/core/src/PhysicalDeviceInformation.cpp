#include "liquid/core/Base.h"
#include "PhysicalDeviceInformation.h"

namespace liquid::rhi {

PhysicalDeviceInformation::PhysicalDeviceInformation(
    StringView name, PhysicalDeviceType type,
    const UnorderedPropertyMap &properties,
    const UnorderedPropertyMap &rawLimits, const Limits &limits)
    : mName(name), mType(type), mProperties(properties), mRawLimits(rawLimits),
      mLimits(limits) {}

const String PhysicalDeviceInformation::getTypeString() const {
  switch (mType) {
  case PhysicalDeviceType::DiscreteGPU:
    return "Discrete GPU";
  case PhysicalDeviceType::IntegratedGPU:
    return "Integrated GPU";
  case PhysicalDeviceType::VirtualGPU:
    return "Virtual GPU";
  case PhysicalDeviceType::CPU:
    return "CPU";
  default:
    return "Unknown";
  }
}

} // namespace liquid::rhi
