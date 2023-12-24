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

/**
 * @brief Physical device properties
 */
struct PhysicalDeviceProperties {
  /**
   * Device type
   */
  PhysicalDeviceType type;

  /**
   * API name
   */
  String apiName;

  /**
   * API version
   */
  u32 apiVersion;

  /**
   * Driver version
   */
  u32 driverVersion;

  /**
   * Vendor ID
   */
  u32 vendorId;

  /**
   * Device ID
   */
  u32 deviceId;
};

/**
 * @brief Physical device information
 */
class PhysicalDeviceInformation {
public:
  /**
   * @brief Create physical device information
   *
   * @param name Device name
   * @param properties Device properties
   * @param limits Device limits
   */
  PhysicalDeviceInformation(StringView name,
                            const PhysicalDeviceProperties &properties,
                            const PhysicalDeviceLimits &limits);

  /**
   * @brief Get device type
   *
   * @return Physical device type
   */
  inline const PhysicalDeviceType getType() const { return mProperties.type; }

  /**
   * @brief Get device name
   *
   * @return Physical device name
   */
  inline const String &getName() const { return mName; }

  /**
   * @brief Get device properties
   *
   * @return Device properties
   */
  inline const PhysicalDeviceProperties &getProperties() const {
    return mProperties;
  }

  /**
   * @brief Get device limits
   *
   * @return Device limits
   */
  inline const PhysicalDeviceLimits &getLimits() const { return mLimits; }

  /**
   * @brief Get type string proeprties
   *
   * @return Type string
   */
  const String getTypeString() const;

private:
  String mName;
  PhysicalDeviceProperties mProperties;
  PhysicalDeviceLimits mLimits;
};

} // namespace quoll::rhi
