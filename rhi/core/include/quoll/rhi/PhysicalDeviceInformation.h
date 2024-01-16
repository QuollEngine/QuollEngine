#pragma once

#include "quoll/core/Property.h"
#include "PhysicalDeviceLimits.h"

namespace quoll::rhi {

enum class PhysicalDeviceType {
  DiscreteGPU,
  IntegratedGPU,
  VirtualGPU,
  CPU,
  Unknown
};

struct PhysicalDeviceProperties {
  PhysicalDeviceType type;

  String apiName;

  u32 apiVersion;

  u32 driverVersion;

  u32 vendorId;

  u32 deviceId;
};

class PhysicalDeviceInformation {
public:
  PhysicalDeviceInformation(StringView name,
                            const PhysicalDeviceProperties &properties,
                            const PhysicalDeviceLimits &limits);

  inline const PhysicalDeviceType getType() const { return mProperties.type; }

  inline const String &getName() const { return mName; }

  inline const PhysicalDeviceProperties &getProperties() const {
    return mProperties;
  }

  inline const PhysicalDeviceLimits &getLimits() const { return mLimits; }

  const String getTypeString() const;

private:
  String mName;
  PhysicalDeviceProperties mProperties;
  PhysicalDeviceLimits mLimits;
};

} // namespace quoll::rhi
