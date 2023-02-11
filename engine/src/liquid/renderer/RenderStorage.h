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
  inline const rhi::Descriptor &getGlobalTexturesDescriptor() const {
    return mGlobalTexturesDescriptor;
  }

  /**
   * @brief Get global buffers descriptor
   *
   * @return Global buffers descriptor
   */
  inline const rhi::Descriptor &getGlobalBuffersDescriptor() const {
    return mGlobalBuffersDescriptor;
  }

  /**
   * @brief Create material descriptor
   *
   * @param buffer Material buffer
   * @return Material descriptor
   */
  rhi::Descriptor createMaterialDescriptor(rhi::Buffer buffer);

  /**
   * @brief Get render device
   *
   * @return Render device
   */
  inline rhi::RenderDevice *getDevice() { return mDevice; }

private:
  rhi::RenderDevice *mDevice = nullptr;

  rhi::DescriptorLayoutHandle mMaterialDescriptorLayout{0};

  rhi::Descriptor mGlobalTexturesDescriptor;
  rhi::Descriptor mGlobalBuffersDescriptor;

  size_t mResizeListener = 0;
};

} // namespace liquid
