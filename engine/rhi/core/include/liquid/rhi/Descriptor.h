#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid::rhi {

enum class DescriptorType {
  UniformBuffer,
  StorageBuffer,
  CombinedImageSampler,

};

/**
 * @brief Descriptor binding
 */
struct DescriptorBinding {
  /**
   * Descriptor type
   */
  DescriptorType type;

  /**
   * Descriptor data
   */
  std::variant<std::vector<TextureHandle>, BufferHandle> data;
};

/**
 * @brief Descriptor
 *
 * Creates descriptor information
 * int the CPU and is used by
 * descriptor manager to get the hash
 * data
 */
class Descriptor {
public:
  /**
   * @brief Bind texture descriptor
   *
   * @param binding Binding number
   * @param textures List of textures
   * @param type Descriptor type
   * @return Current object
   */
  Descriptor &bind(uint32_t binding, const std::vector<TextureHandle> &textures,
                   DescriptorType type);

  /**
   * @brief Bind buffer descriptor
   *
   * @param binding Binding number
   * @param buffer Hardware buffer
   * @param type Descriptor type
   * @return Current object
   */
  Descriptor &bind(uint32_t binding, BufferHandle buffer, DescriptorType type);

  /**
   * @brief Get bindings
   *
   * @return Bindings
   */
  inline const std::map<uint32_t, DescriptorBinding> &getBindings() const {
    return bindings;
  }

  /**
   * @brief Get hash code
   *
   * @return Hash code
   */
  inline const String &getHashCode() const { return hashCode; }

private:
  std::map<uint32_t, DescriptorBinding> bindings;
  String hashCode;
};

} // namespace liquid::rhi
