#pragma once

#include "liquid/rhi/TextureDescription.h"
#include "liquid/rhi/RenderDevice.h"
#include "HandleCounter.h"

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
   * @brief Create texture view
   *
   * @param description Texture view description
   * @param addToDescriptor Add texture to global descriptor
   * @return Texture view handle
   */
  rhi::TextureHandle
  createTextureView(const liquid::rhi::TextureViewDescription &description,
                    bool addToDescriptor = true);

  /**
   * @brief Destroy texture
   *
   * @param handle Texture handle
   */
  void destroyTexture(rhi::TextureHandle handle);

  /**
   * @brief Create shader
   *
   * @param name Shader name
   * @param description Shader description
   * @return Shader handle
   */
  rhi::ShaderHandle
  createShader(const String &name,
               const liquid::rhi::ShaderDescription &description);

  /**
   * @brief Get shader by name
   *
   * @param name Shader name
   * @return Shader handle
   */
  rhi::ShaderHandle getShader(const String &name);

  /**
   * @brief Add texture to global descriptor
   *
   * @param handle Texture handle
   */
  void addToDescriptor(rhi::TextureHandle handle);

  /**
   * @brief Get new texture handle
   *
   * @return Texture handle
   */
  rhi::TextureHandle getNewTextureHandle();

  /**
   * @brief Get new render pass handle
   *
   * @return Render pass handle
   */
  rhi::RenderPassHandle getNewRenderPassHandle();

  /**
   * @brief Get new framebuffer handle
   *
   * @return Framebuffer handle
   */
  rhi::FramebufferHandle getNewFramebufferHandle();

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

  std::vector<std::variant<rhi::GraphicsPipelineDescription,
                           rhi::ComputePipelineDescription>>
      mPipelineDescriptions;
  std::vector<size_t> mGraphicsPipelineIndices;
  std::vector<size_t> mComputePipelineIndices;

  static constexpr uint32_t TextureStart = 10;
  HandleCounter<rhi::ShaderHandle> mShaderCounter;
  HandleCounter<rhi::TextureHandle, TextureStart> mTextureCounter;
  HandleCounter<rhi::RenderPassHandle> mRenderPassCounter;
  HandleCounter<rhi::FramebufferHandle> mFramebufferCounter;

  std::unordered_map<String, rhi::ShaderHandle> mShaderMap;
};

} // namespace liquid
