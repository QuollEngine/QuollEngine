#pragma once

#include "NativeDescriptor.h"

namespace quoll::rhi {

class Descriptor {
public:
  Descriptor() = default;

  Descriptor(NativeDescriptor *nativeDescriptor, DescriptorHandle handle);

  Descriptor &write(u32 binding, std::span<TextureHandle> textures,
                    DescriptorType type, u32 start = 0);

  Descriptor &write(u32 binding, std::span<SamplerHandle> samplers,
                    u32 start = 0);

  Descriptor &write(u32 binding, std::span<BufferHandle> buffers,
                    DescriptorType type, u32 start = 0);

  Descriptor &write(u32 binding, std::span<DescriptorBufferInfo> bufferInfos,
                    DescriptorType type, u32 start = 0);

  inline DescriptorHandle getHandle() const { return mHandle; }

private:
  NativeDescriptor *mNativeDescriptor = nullptr;
  DescriptorHandle mHandle{0};
};

} // namespace quoll::rhi
