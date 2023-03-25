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

  /**
   * @brief Device limits
   */
  struct Limits {
    /**
     * Minimum uniform buffer alignment offset
     */
    uint32_t minUniformBufferOffsetAlignment = 0;

    /**
     * Framebuffer color sample counts
     */
    uint32_t framebufferColorSampleCounts = 0;

    /**
     * Framebuffer depth sample counts
     */
    uint32_t framebufferDepthSampleCounts = 0;
  };

public:
  /**
   * @brief Create physical device information
   *
   * @param name Device name
   * @param type Device type
   * @param properties Device properties
   * @param rawLimits Device limits (raw)
   * @param limits Device limits
   */
  PhysicalDeviceInformation(StringView name, PhysicalDeviceType type,
                            const UnorderedPropertyMap &properties,
                            const UnorderedPropertyMap &rawLimits,
                            const Limits &limits);

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
  inline const UnorderedPropertyMap &getRawLimits() const { return mRawLimits; }

  /**
   * @brief Get device limits
   *
   * @return Device limits
   */
  inline const Limits &getLimits() const { return mLimits; }

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
  UnorderedPropertyMap mRawLimits;
  Limits mLimits;
};

} // namespace liquid::rhi
