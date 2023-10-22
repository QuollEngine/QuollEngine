#pragma once

#include "quoll/rhi/NativeDescriptor.h"

namespace quoll::rhi {

/**
 * @brief Mock descriptor
 */
class MockDescriptor : public NativeDescriptor {
public:
  /**
   * @brief Binding data
   */
  struct Binding {
    /**
     * Binding
     */
    u32 binding;

    /**
     * Descriptor type
     */
    DescriptorType type;

    /**
     * Start index
     */
    u32 start;

    /**
     * Binding data
     */
    std::variant<std::vector<TextureHandle>, std::vector<SamplerHandle>,
                 std::vector<BufferHandle>, std::vector<DescriptorBufferInfo>>
        data;
  };

public:
  /**
   * @brief Create mock descriptor
   *
   * @param layout Descriptor layout handle
   */
  MockDescriptor(DescriptorLayoutHandle layout);

  /**
   * @brief Bind texture descriptors
   *
   * @param binding Binding number
   * @param textures Textures
   * @param type Descriptor type
   * @param start Starting index
   */
  void write(u32 binding, std::span<TextureHandle> textures,
             DescriptorType type, u32 start) override;

  /**
   * @brief Bind sampler descriptors
   *
   * @param binding Binding number
   * @param samplers Samplers
   * @param start Starting index
   */
  void write(u32 binding, std::span<SamplerHandle> samplers,
             u32 start) override;

  /**
   * @brief Bind buffer descriptors
   *
   * @param binding Binding number
   * @param buffers Buffers
   * @param type Descriptor type
   * @param start Starting index
   */
  void write(u32 binding, std::span<BufferHandle> buffers, DescriptorType type,
             u32 start) override;

  /**
   * @brief Bind buffer descriptors
   *
   * @param binding Binding number
   * @param bufferInfos Buffer infos
   * @param type Descriptor type
   * @param start Starting index
   */
  void write(u32 binding, std::span<DescriptorBufferInfo> bufferInfos,
             DescriptorType type, u32 start) override;

private:
  std::vector<Binding> mBindings;
  DescriptorLayoutHandle mLayout;
};

} // namespace quoll::rhi
