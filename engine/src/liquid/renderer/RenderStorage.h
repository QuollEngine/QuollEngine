#pragma once

#include "liquid/rhi/TextureDescription.h"
#include "liquid/rhi/RenderDevice.h"

namespace liquid {

/**
 * @brief Render storage
 *
 * Abstracts away low level render device
 * implementations from the codebase
 */
class RenderStorage {
public:
  /**
   * @brief Create render storage
   *
   * @param device Render device
   */
  RenderStorage(rhi::RenderDevice *device);

  RenderStorage(const RenderStorage &) = delete;
  RenderStorage &operator=(const RenderStorage &) = delete;
  RenderStorage(RenderStorage &&) = delete;
  RenderStorage &operator=(RenderStorage &&) = delete;

  /**
   * @brief Destroy render storage
   */
  ~RenderStorage();

  /**
   * @brief Create texture
   *
   * @param description Texture description
   * @return Texture handle
   */
  rhi::TextureHandle
  createTexture(const liquid::rhi::TextureDescription &description);

  /**
   * @brief Create buffer
   *
   * @param description Buffer description
   * @return Buffer
   */
  rhi::Buffer createBuffer(const liquid::rhi::BufferDescription &description);

  /**
   * @brief Get global textures descriptor
   *
   * @return Global textures descriptor
   */
  inline const rhi::n::Descriptor &getGlobalTexturesDescriptor() const {
    return mGlobalTexturesDescriptor;
  }

private:
  rhi::RenderDevice *mDevice = nullptr;

  rhi::n::Descriptor mGlobalTexturesDescriptor;

  size_t mResizeListener = 0;
};

} // namespace liquid
