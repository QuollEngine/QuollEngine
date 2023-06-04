#pragma once

#include "DescriptorType.h"
#include "RenderHandle.h"

namespace liquid::rhi {

/**
 * @brief Descriptor buffer info
 */
struct DescriptorBufferInfo {
  /**
   * Buffer offset
   */
  uint32_t offset = 0;

  /**
   * Buffer range
   */
  uint32_t range = 0;

  /**
   * Buffer
   */
  BufferHandle buffer = rhi::BufferHandle::Null;
};

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
  virtual void write(uint32_t binding, std::span<TextureHandle> textures,
                     DescriptorType type, uint32_t start) = 0;

  /**
   * @brief Bind buffer descriptors
   *
   * @param binding Binding number
   * @param buffers Buffers
   * @param type Descriptor type
   * @param start Starting index
   */
  virtual void write(uint32_t binding, std::span<BufferHandle> buffers,
                     DescriptorType type, uint32_t start) = 0;

  /**
   * @brief Bind buffer descriptors
   *
   * @param binding Binding number
   * @param bufferInfos Buffer infos
   * @param type Descriptor type
   * @param start Starting index
   */
  virtual void write(uint32_t binding,
                     std::span<DescriptorBufferInfo> bufferInfos,
                     DescriptorType type, uint32_t start) = 0;
};

} // namespace liquid::rhi
