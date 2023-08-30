#include "liquid/core/Base.h"
#include "MockDescriptor.h"

namespace quoll::rhi {

template <class T>
static constexpr std::vector<T> vectorFrom(std::span<T> data) {
  return std::vector<T>(data.begin(), data.end());
}

MockDescriptor::MockDescriptor(DescriptorLayoutHandle layout)
    : mLayout(layout) {}

void MockDescriptor::write(uint32_t binding, std::span<TextureHandle> textures,
                           DescriptorType type, uint32_t start) {
  mBindings.push_back({binding, type, start, vectorFrom(textures)});
}

void MockDescriptor::write(uint32_t binding, std::span<SamplerHandle> samplers,
                           uint32_t start) {
  mBindings.push_back(
      {binding, DescriptorType::Sampler, start, vectorFrom(samplers)});
}

void MockDescriptor::write(uint32_t binding, std::span<BufferHandle> buffers,
                           DescriptorType type, uint32_t start) {
  mBindings.push_back({binding, type, start, vectorFrom(buffers)});
}

void MockDescriptor::write(uint32_t binding,
                           std::span<DescriptorBufferInfo> bufferInfos,
                           DescriptorType type, uint32_t start) {
  mBindings.push_back({binding, type, start, vectorFrom(bufferInfos)});
}

} // namespace quoll::rhi
