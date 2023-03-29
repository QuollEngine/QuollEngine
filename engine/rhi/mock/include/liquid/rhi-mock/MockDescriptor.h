#pragma once

#include "liquid/rhi/NativeDescriptor.h"

namespace liquid::rhi {

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
    uint32_t binding;

    /**
     * Descriptor type
     */
    DescriptorType type;

    /**
     * Start index
     */
    uint32_t start;

    /**
     * Binding data
     */
    std::variant<std::vector<TextureHandle>, std::vector<TextureViewHandle>,
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
  void write(uint32_t binding, std::span<TextureHandle> textures,
             DescriptorType type, uint32_t start) override;

  /**
   * @brief Bind texture descriptors
   *
   * @param binding Binding number
   * @param textureViews Texture views
   * @param type Descriptor type
   * @param start Starting index
   */
  void write(uint32_t binding, std::span<TextureViewHandle> textureViews,
             DescriptorType type, uint32_t start) override;

  /**
   * @brief Bind buffer descriptors
   *
   * @param binding Binding number
   * @param buffers Buffers
   * @param type Descriptor type
   * @param start Starting index
   */
  void write(uint32_t binding, std::span<BufferHandle> buffers,
             DescriptorType type, uint32_t start) override;

  /**
   * @brief Bind buffer descriptors
   *
   * @param binding Binding number
   * @param bufferInfos Buffer infos
   * @param type Descriptor type
   * @param start Starting index
   */
  void write(uint32_t binding, std::span<DescriptorBufferInfo> bufferInfos,
             DescriptorType type, uint32_t start) override;

private:
  std::vector<Binding> mBindings;
  DescriptorLayoutHandle mLayout;
};

} // namespace liquid::rhi
