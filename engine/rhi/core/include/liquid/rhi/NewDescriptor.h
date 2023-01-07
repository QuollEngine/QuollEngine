#pragma once

#include "Descriptor.h"
#include "NativeDescriptor.h"

namespace liquid::rhi::n {

/**
 * @brief Descriptor
 *
 * Proxies all calls to native descriptor
 */
class Descriptor {
public:
  /**
   * @brief Create descriptor
   */
  Descriptor() = default;

  /**
   * @brief Create descriptor
   *
   * @param nativeDescriptor Native descriptor
   * @param handle Descriptor handle
   */
  Descriptor(NativeDescriptor *nativeDescriptor, DescriptorHandle handle);

  /**
   * @brief Bind texture descriptors
   *
   * @param binding Binding number
   * @param textures List of textures
   * @param type Descriptor type
   * @param start Starting index
   * @return Current object
   */
  Descriptor &write(uint32_t binding,
                    const std::vector<TextureHandle> &textures,
                    DescriptorType type, uint32_t start = 0);

  /**
   * @brief Bind buffer descriptors
   *
   * @param binding Binding number
   * @param buffers Buffers
   * @param type Descriptor type
   * @param start Starting index
   * @return Current object
   */
  Descriptor &write(uint32_t binding, const std::vector<BufferHandle> &buffers,
                    DescriptorType type, uint32_t start = 0);

  /**
   * @brief Get descriptor handle
   *
   * @return Descriptor handle
   */
  inline DescriptorHandle getHandle() const { return mHandle; }

private:
  NativeDescriptor *mNativeDescriptor = nullptr;
  DescriptorHandle mHandle{0};
};

} // namespace liquid::rhi::n
