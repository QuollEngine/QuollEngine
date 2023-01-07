#include "liquid/core/Base.h"

#include "NewDescriptor.h"

namespace liquid::rhi::n {

Descriptor::Descriptor(NativeDescriptor *nativeDescriptor,
                       DescriptorHandle handle)
    : mNativeDescriptor(nativeDescriptor), mHandle(handle) {}

Descriptor &Descriptor::write(uint32_t binding,
                              const std::vector<TextureHandle> &textures,
                              DescriptorType type, uint32_t start) {
  mNativeDescriptor->write(binding, textures, type, start);

  return *this;
}

Descriptor &Descriptor::write(uint32_t binding,
                              const std::vector<BufferHandle> &buffers,
                              DescriptorType type, uint32_t start) {
  mNativeDescriptor->write(binding, buffers, type, start);

  return *this;
}

} // namespace liquid::rhi::n
