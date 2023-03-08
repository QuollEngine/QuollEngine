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
  ~RenderStorage() = default;

  /**
   * @brief Create texture
   *
   * @param description Texture description
   * @param addToDescriptor Add texture to global descriptor
   * @return Texture handle
   */
  rhi::TextureHandle
  createTexture(const liquid::rhi::TextureDescription &description,
                bool addToDescriptor = true);

  /**
   * @brief Create framebuffer relative texture
   *
   * @param description Texture description
   * @param addToDescriptor Add texture to global descriptor
   * @return Texture handle
   */
  rhi::TextureHandle createFramebufferRelativeTexture(
      const liquid::rhi::TextureDescription &description,
      bool addToDescriptor = true);

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

  /**
   * @brief Recreate framebuffer relative textures
   *
   * @retval true Framebuffer relative textures recreated
   * @retval false Framebuffer relative textures do not require recreate
   */
  bool recreateFramebufferRelativeTextures();

  /**
   * @brief Check if texture is framebuffer relative
   *
   * @param handle Texture handle
   * @retval true Texture is framebuffer relative
   * @retval false Texture is not framebuffer relative
   */
  bool isFramebufferRelative(rhi::TextureHandle handle);

  /**
   * @brief Set framebuffer size
   *
   * @param width Framebuffer width
   * @param height Framebuffer height
   */
  void setFramebufferSize(uint32_t width, uint32_t height);

private:
  rhi::RenderDevice *mDevice = nullptr;

  rhi::DescriptorLayoutHandle mMaterialDescriptorLayout{0};

  rhi::Descriptor mGlobalTexturesDescriptor;
  rhi::Descriptor mGlobalBuffersDescriptor;

  size_t mResizeListener = 0;

  std::map<rhi::TextureHandle, rhi::TextureDescription>
      mFramebufferRelativeTextures;

  bool mNeedsSwapchainResize = false;

  uint32_t mWidth = 0;
  uint32_t mHeight = 0;
};

} // namespace liquid
