#pragma once

#include "DescriptorType.h"
#include "RenderHandle.h"

namespace quoll::rhi {

/**
 * @brief Descriptor buffer info
 */
struct DescriptorBufferInfo {
  /**
   * Buffer offset
   */
  u32 offset = 0;

  /**
   * Buffer range
   */
  u32 range = 0;

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
  virtual void write(u32 binding, std::span<TextureHandle> textures,
                     DescriptorType type, u32 start) = 0;

  /**
   * @brief Bind sampler descriptors
   *
   * @param binding Binding number
   * @param samplers Samplers
   * @param start Starting index
   */
  virtual void write(u32 binding, std::span<SamplerHandle> samplers,
                     u32 start) = 0;

  /**
   * @brief Bind buffer descriptors
   *
   * @param binding Binding number
   * @param buffers Buffers
   * @param type Descriptor type
   * @param start Starting index
   */
  virtual void write(u32 binding, std::span<BufferHandle> buffers,
                     DescriptorType type, u32 start) = 0;

  /**
   * @brief Bind buffer descriptors
   *
   * @param binding Binding number
   * @param bufferInfos Buffer infos
   * @param type Descriptor type
   * @param start Starting index
   */
  virtual void write(u32 binding, std::span<DescriptorBufferInfo> bufferInfos,
                     DescriptorType type, u32 start) = 0;
};

} // namespace quoll::rhi
