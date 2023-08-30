#include "quoll/core/Base.h"

#include "Descriptor.h"

namespace quoll::rhi {

Descriptor::Descriptor(NativeDescriptor *nativeDescriptor,
                       DescriptorHandle handle)
    : mNativeDescriptor(nativeDescriptor), mHandle(handle) {}

Descriptor &Descriptor::write(uint32_t binding,
                              std::span<TextureHandle> textures,
                              DescriptorType type, uint32_t start) {
  mNativeDescriptor->write(binding, textures, type, start);

  return *this;
}

Descriptor &Descriptor::write(uint32_t binding,
                              std::span<SamplerHandle> samplers,
                              uint32_t start) {
  mNativeDescriptor->write(binding, samplers, start);
  return *this;
}

Descriptor &Descriptor::write(uint32_t binding, std::span<BufferHandle> buffers,
                              DescriptorType type, uint32_t start) {
  mNativeDescriptor->write(binding, buffers, type, start);

  return *this;
}

Descriptor &Descriptor::write(uint32_t binding,
                              std::span<DescriptorBufferInfo> bufferInfos,
                              DescriptorType type, uint32_t start) {
  mNativeDescriptor->write(binding, bufferInfos, type, start);

  return *this;
}

} // namespace quoll::rhi
