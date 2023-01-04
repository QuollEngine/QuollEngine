#pragma once

#include "Descriptor.h"
#include "RenderHandle.h"

namespace liquid::rhi {

/**
 * @brief Native descriptor interface
 */
class NativeDescriptor {
public:
  /**
   * @brief Bind texture descriptors
   *
   * @param binding Binding number
   * @param textures Textures
   * @param type Descriptor type
   * @param start Starting index
   */
  virtual void write(uint32_t binding,
                     const std::vector<TextureHandle> &textures,
                     DescriptorType type, uint32_t start) = 0;

  /**
   * @brief Bind buffer descriptors
   *
   * @param binding Binding number
   * @param buffers Buffers
   * @param type Descriptor type
   * @param start Starting index
   */
  virtual void write(uint32_t binding, const std::vector<BufferHandle> &buffers,
                     DescriptorType type, uint32_t start) = 0;
};

} // namespace liquid::rhi
