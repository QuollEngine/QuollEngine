#pragma once

#include "Descriptor.h"
#include "StageFlags.h"

namespace liquid::rhi {

enum DescriptorLayoutBindingType { Static, Dynamic };

/**
 * @brief Descriptor layout binding description
 */
struct DescriptorLayoutBindingDescription {
  /**
   * Binding name
   */
  String name;

  /**
   * Binding type
   */
  DescriptorLayoutBindingType type{Static};

  /**
   * Binding number
   */
  uint32_t binding = 0;

  /**
   * Descriptor type
   */
  DescriptorType descriptorType;

  /**
   * Number of descriptors
   */
  uint32_t descriptorCount = 0;

  /**
   * Shader stage
   */
  ShaderStage shaderStage = ShaderStage::All;
};

/**
 * @brief Descriptor layout description
 */
struct DescriptorLayoutDescription {
  /**
   * Descriptor layout bindings
   */
  std::vector<DescriptorLayoutBindingDescription> bindings;

  /**
   * Debug name
   */
  String debugName;
};

} // namespace liquid::rhi
