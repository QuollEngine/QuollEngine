#pragma once

#include "liquid/rhi/NativeDescriptor.h"

class MockDescriptor : public liquid::rhi::NativeDescriptor {
public:
  MockDescriptor(liquid::rhi::DescriptorLayoutHandle layout);

  void write(uint32_t binding,
             const std::vector<liquid::rhi::TextureHandle> &textures,
             liquid::rhi::DescriptorType type, uint32_t start) override;

  void write(uint32_t binding,
             const std::vector<TextureViewHandle> &textureViews,
             DescriptorType type, uint32_t start = 0) override;

  void write(uint32_t binding,
             const std::vector<liquid::rhi::BufferHandle> &buffers,
             liquid::rhi::DescriptorType type, uint32_t start) override;

public:
  liquid::rhi::DescriptorLayoutHandle mLayout{0};
};
