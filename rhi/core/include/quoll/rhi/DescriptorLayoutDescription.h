#pragma once

#include "Descriptor.h"
#include "StageFlags.h"

namespace quoll::rhi {

enum DescriptorLayoutBindingType { Static, Dynamic };

struct DescriptorLayoutBindingDescription {
  String name;

  DescriptorLayoutBindingType type{Static};

  u32 binding = 0;

  DescriptorType descriptorType;

  u32 descriptorCount = 0;

  ShaderStage shaderStage = ShaderStage::All;
};

struct DescriptorLayoutDescription {
  std::vector<DescriptorLayoutBindingDescription> bindings;

  String debugName;
};

} // namespace quoll::rhi
