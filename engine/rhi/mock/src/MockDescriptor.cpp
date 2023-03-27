#include "liquid/core/Base.h"
#include "MockDescriptor.h"

namespace liquid::rhi {

MockDescriptor::MockDescriptor(DescriptorLayoutHandle layout)
    : mLayout(layout) {}

void MockDescriptor::write(uint32_t binding,
                           const std::vector<TextureHandle> &textures,
                           DescriptorType type, uint32_t start) {
  mBindings.push_back({binding, type, start, textures});
}

void MockDescriptor::write(uint32_t binding,
                           const std::vector<TextureViewHandle> &textureViews,
                           DescriptorType type, uint32_t start) {
  mBindings.push_back({binding, type, start, textureViews});
}

void MockDescriptor::write(uint32_t binding,
                           const std::vector<BufferHandle> &buffers,
                           DescriptorType type, uint32_t start) {
  mBindings.push_back({binding, type, start, buffers});
}

void MockDescriptor::write(uint32_t binding,
                           const std::vector<DescriptorBufferInfo> &bufferInfos,
                           DescriptorType type, uint32_t start) {
  mBindings.push_back({binding, type, start, bufferInfos});
}

} // namespace liquid::rhi
