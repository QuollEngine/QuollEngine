#pragma once

#include "DescriptorType.h"
#include "RenderHandle.h"

namespace quoll::rhi {

struct DescriptorBufferInfo {
  u32 offset = 0;

  u32 range = 0;

  BufferHandle buffer = rhi::BufferHandle::Null;
};

class NativeDescriptor {
public:
  virtual void write(u32 binding, std::span<TextureHandle> textures,
                     DescriptorType type, u32 start) = 0;

  virtual void write(u32 binding, std::span<SamplerHandle> samplers,
                     u32 start) = 0;

  virtual void write(u32 binding, std::span<BufferHandle> buffers,
                     DescriptorType type, u32 start) = 0;

  virtual void write(u32 binding, std::span<DescriptorBufferInfo> bufferInfos,
                     DescriptorType type, u32 start) = 0;
};

} // namespace quoll::rhi
