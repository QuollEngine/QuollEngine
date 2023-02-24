#include "liquid/core/Base.h"
#include "MockDescriptor.h"

MockDescriptor::MockDescriptor(liquid::rhi::DescriptorLayoutHandle layout)
    : mLayout(layout) {}

void MockDescriptor::write(
    uint32_t binding, const std::vector<liquid::rhi::TextureHandle> &textures,
    liquid::rhi::DescriptorType type, uint32_t start) {}

void MockDescriptor::write(
    uint32_t binding,
    const std::vector<liquid::rhi::TextureViewHandle> &textureViews,
    liquid::rhi::DescriptorType type, uint32_t start) {}

void MockDescriptor::write(
    uint32_t binding, const std::vector<liquid::rhi::BufferHandle> &buffers,
    liquid::rhi::DescriptorType type, uint32_t start) {}
