#pragma once

#include "quoll/rhi/NativeDescriptor.h"

namespace quoll::rhi {

class MockDescriptor : public NativeDescriptor {
public:
  struct Binding {
    u32 binding;

    DescriptorType type;

    u32 start;

    std::variant<std::vector<TextureHandle>, std::vector<SamplerHandle>,
                 std::vector<BufferHandle>, std::vector<DescriptorBufferInfo>>
        data;
  };

public:
  MockDescriptor(DescriptorLayoutHandle layout);

  void write(u32 binding, std::span<TextureHandle> textures,
             DescriptorType type, u32 start) override;

  void write(u32 binding, std::span<SamplerHandle> samplers,
             u32 start) override;

  void write(u32 binding, std::span<BufferHandle> buffers, DescriptorType type,
             u32 start) override;

  void write(u32 binding, std::span<DescriptorBufferInfo> bufferInfos,
             DescriptorType type, u32 start) override;

private:
  std::vector<Binding> mBindings;
  DescriptorLayoutHandle mLayout;
};

} // namespace quoll::rhi
