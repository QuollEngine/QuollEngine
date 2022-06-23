#pragma once

#include "liquid/core/Property.h"

namespace liquid::rhi {

enum class PhysicalDeviceType {
  DiscreteGPU,
  IntegratedGPU,
  VirtualGPU,
  CPU,
  Unknown
};

/**
 * @brief Physical device information
 */
class PhysicalDeviceInformation {
public:
  /**
   * Unordered property map
   */
  using UnorderedPropertyMap = std::vector<std::pair<String, Property>>;

public:
  /**
   * @brief Create physical device information
   *
   * @param name Device name
   * @param type Device type
   * @param properties Device properties
   * @param limits Device limits
   */
  PhysicalDeviceInformation(StringView name, PhysicalDeviceType type,
                            const UnorderedPropertyMap &properties,
                            const UnorderedPropertyMap &limits);

  /**
   * @brief Get device type
   *
   * @return Physical device type
   */
  inline const PhysicalDeviceType getType() const { return mType; }

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
  inline const UnorderedPropertyMap &getProperties() const {
    return mProperties;
  }

  /**
   * @brief Get device limits
   *
   * @return Device limits
   */
  inline const UnorderedPropertyMap &getLimits() const { return mLimits; }

  /**
   * @brief Get type string proeprties
   *
   * @return Type string
   */
  const String getTypeString() const;

private:
  String mName;
  PhysicalDeviceType mType;

  UnorderedPropertyMap mProperties;
  UnorderedPropertyMap mLimits;
};

} // namespace liquid::rhi
