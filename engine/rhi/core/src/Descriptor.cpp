#include "quoll/core/Base.h"

#include "Descriptor.h"

namespace quoll::rhi {

Descriptor::Descriptor(NativeDescriptor *nativeDescriptor,
                       DescriptorHandle handle)
    : mNativeDescriptor(nativeDescriptor), mHandle(handle) {}

Descriptor &Descriptor::write(u32 binding, std::span<TextureHandle> textures,
                              DescriptorType type, u32 start) {
  mNativeDescriptor->write(binding, textures, type, start);

  return *this;
}

Descriptor &Descriptor::write(u32 binding, std::span<SamplerHandle> samplers,
                              u32 start) {
  mNativeDescriptor->write(binding, samplers, start);
  return *this;
}

Descriptor &Descriptor::write(u32 binding, std::span<BufferHandle> buffers,
                              DescriptorType type, u32 start) {
  mNativeDescriptor->write(binding, buffers, type, start);

  return *this;
}

Descriptor &Descriptor::write(u32 binding,
                              std::span<DescriptorBufferInfo> bufferInfos,
                              DescriptorType type, u32 start) {
  mNativeDescriptor->write(binding, bufferInfos, type, start);

  return *this;
}

} // namespace quoll::rhi
