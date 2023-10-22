#pragma once

#include "Descriptor.h"
#include "StageFlags.h"

namespace quoll::rhi {

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
  u32 binding = 0;

  /**
   * Descriptor type
   */
  DescriptorType descriptorType;

  /**
   * Number of descriptors
   */
  u32 descriptorCount = 0;

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

} // namespace quoll::rhi
