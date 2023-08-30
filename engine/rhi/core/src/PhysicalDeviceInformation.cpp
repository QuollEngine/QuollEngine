#include "quoll/core/Base.h"
#include "PhysicalDeviceInformation.h"

namespace quoll::rhi {

PhysicalDeviceInformation::PhysicalDeviceInformation(
    StringView name, const PhysicalDeviceProperties &properties,
    const PhysicalDeviceLimits &limits)
    : mName(name), mProperties(properties), mLimits(limits) {}

const String PhysicalDeviceInformation::getTypeString() const {
  switch (mProperties.type) {
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

} // namespace quoll::rhi
