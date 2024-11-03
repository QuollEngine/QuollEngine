#include "quoll/core/Base.h"
#include "MockDescriptor.h"

namespace quoll::rhi {

template <class T> constexpr std::vector<T> vectorFrom(std::span<T> data) {
  return std::vector<T>(data.begin(), data.end());
}

MockDescriptor::MockDescriptor(DescriptorLayoutHandle layout)
    : mLayout(layout) {}

void MockDescriptor::write(u32 binding, std::span<TextureHandle> textures,
                           DescriptorType type, u32 start) {
  mBindings.push_back({binding, type, start, vectorFrom(textures)});
}

void MockDescriptor::write(u32 binding, std::span<SamplerHandle> samplers,
                           u32 start) {
  mBindings.push_back(
      {binding, DescriptorType::Sampler, start, vectorFrom(samplers)});
}

void MockDescriptor::write(u32 binding, std::span<BufferHandle> buffers,
                           DescriptorType type, u32 start) {
  mBindings.push_back({binding, type, start, vectorFrom(buffers)});
}

void MockDescriptor::write(u32 binding,
                           std::span<DescriptorBufferInfo> bufferInfos,
                           DescriptorType type, u32 start) {
  mBindings.push_back({binding, type, start, vectorFrom(bufferInfos)});
}

} // namespace quoll::rhi
