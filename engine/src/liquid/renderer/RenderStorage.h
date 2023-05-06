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
   * @brief Get new texture handle
   *
   * @return Texture handle
   */
  rhi::TextureHandle getNewTextureHandle();

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

public:
  /**
   * @brief Add graphics pipeline
   *
   * @param description Graphics pipeline description
   * @return Virtual pipeline handle
   */
  rhi::PipelineHandle
  addPipeline(const rhi::GraphicsPipelineDescription &description);

  /**
   * @brief Add compute pipeline
   *
   * @param description Compute pipeline description
   * @return Virtual pipeline handle
   */
  rhi::PipelineHandle
  addPipeline(const rhi::ComputePipelineDescription &description);

  /**
   * @brief Get graphics pipeline description
   *
   * @param handle Pipeline handle
   * @return Graphics pipeline description
   */
  inline rhi::GraphicsPipelineDescription &
  getGraphicsPipelineDescription(rhi::PipelineHandle handle) {
    return std::get<rhi::GraphicsPipelineDescription>(
        mPipelineDescriptions.at(static_cast<size_t>(handle) - 1));
  }

  /**
   * @brief Get compute pipeline description
   *
   * @param handle Pipeline handle
   * @return Compute pipeline description
   */
  inline rhi::ComputePipelineDescription &
  getComputePipelineDescription(rhi::PipelineHandle handle) {
    return std::get<rhi::ComputePipelineDescription>(
        mPipelineDescriptions.at(static_cast<size_t>(handle) - 1));
  }

private:
  rhi::RenderDevice *mDevice = nullptr;

  rhi::DescriptorLayoutHandle mMaterialDescriptorLayout{0};

  rhi::Descriptor mGlobalTexturesDescriptor;
  rhi::Descriptor mGlobalBuffersDescriptor;

  size_t mResizeListener = 0;

  std::map<rhi::TextureHandle, rhi::TextureDescription>
      mFramebufferRelativeTextures;
  std::vector<rhi::TextureHandle>
      mFramebufferRelativeTexturesInGlobalDescriptor;

  bool mNeedsSwapchainResize = false;

  uint32_t mWidth = 0;
  uint32_t mHeight = 0;

  std::vector<std::variant<rhi::GraphicsPipelineDescription,
                           rhi::ComputePipelineDescription>>
      mPipelineDescriptions;
  std::vector<size_t> mGraphicsPipelineIndices;
  std::vector<size_t> mComputePipelineIndices;

  static constexpr uint32_t TextureStart = 0;
  uint32_t mLastTexture = TextureStart;
};

} // namespace liquid
