#pragma once

#include "PhysicalDeviceInformation.h"

#include "DeviceStats.h"
#include "RenderFrame.h"
#include "Buffer.h"
#include "Swapchain.h"

#include "DescriptorLayoutDescription.h"
#include "BufferDescription.h"
#include "TextureDescription.h"
#include "TextureViewDescription.h"
#include "ShaderDescription.h"
#include "RenderPassDescription.h"
#include "FramebufferDescription.h"
#include "PipelineDescription.h"

namespace liquid::rhi {

/**
 * @brief Render device interface
 */
class RenderDevice {
public:
  /**
   * @brief Number of frames
   */
  static constexpr size_t NumFrames = 2;

public:
  RenderDevice(const RenderDevice &) = delete;
  RenderDevice &operator=(const RenderDevice &) = delete;
  RenderDevice(RenderDevice &&) = delete;
  RenderDevice &operator=(RenderDevice &&) = delete;

  /**
   * @brief Default constructor
   */
  RenderDevice() = default;

  /**
   * @brief Destroy render device
   */
  virtual ~RenderDevice() = default;

  /**
   * @brief Request immediate command list
   *
   * @return New command list
   */
  virtual RenderCommandList requestImmediateCommandList() = 0;

  /**
   * @brief Submit commands immediately
   *
   * @param commandList Command list
   */
  virtual void submitImmediate(RenderCommandList &commandList) = 0;

  /**
   * @brief Begin frame
   *
   * @return Frame data
   */
  virtual RenderFrame beginFrame() = 0;

  /**
   * @brief End frame
   *
   * @param renderFrame Render frame
   */
  virtual void endFrame(const RenderFrame &renderFrame) = 0;

  /**
   * @brief Wait for device to be idle
   */
  virtual void waitForIdle() = 0;

  /**
   * @brief Get physical device information
   *
   * @return Physical device information
   */
  virtual const PhysicalDeviceInformation getDeviceInformation() = 0;

  /**
   * @brief Get device stats
   *
   * @return Device stats
   */
  virtual const DeviceStats &getDeviceStats() const = 0;

  /**
   * @brief Destroy all resources in the device
   *
   * This does not destroy the device
   */
  virtual void destroyResources() = 0;

  /**
   * @brief Get swapchain
   *
   * @return Swapchain
   */
  virtual Swapchain getSwapchain() = 0;

  /**
   * @brief Recreate swapchain
   */
  virtual void recreateSwapchain() = 0;

  /**
   * @brief Create shader
   *
   * @param description Shader description
   * @return Shader
   */
  virtual ShaderHandle createShader(const ShaderDescription &description) = 0;

  /**
   * @brief Create descriptor layout
   *
   * @param description Descriptor layout description
   * @return Descriptor layout
   */
  virtual DescriptorLayoutHandle
  createDescriptorLayout(const DescriptorLayoutDescription &description) = 0;

  /**
   * @brief Create descriptor
   *
   * @param layout Descriptor layout
   * @return Descriptor
   */
  virtual Descriptor createDescriptor(DescriptorLayoutHandle layout) = 0;

  /**
   * @brief Create hardware buffer
   *
   * @param description Buffer description
   * @return Buffer
   */
  virtual Buffer createBuffer(const BufferDescription &description) = 0;

  /**
   * @brief Destroy buffer
   *
   * @param handle Buffer handle
   */
  virtual void destroyBuffer(BufferHandle handle) = 0;

  /**
   * @brief Create texture
   *
   * @param description Texture description
   * @param handle Texture handle
   */
  virtual void createTexture(const TextureDescription &description,
                             TextureHandle handle) = 0;

  /**
   * @brief Get texture description
   *
   * @param handle Texture handle
   * @return Texture description
   */
  virtual const TextureDescription
  getTextureDescription(TextureHandle handle) const = 0;

  /**
   * @brief Destroy texture
   *
   * @param handle Texture handle
   */
  virtual void destroyTexture(TextureHandle handle) = 0;

  /**
   * @brief Create texture view
   *
   * @param description Texture view description
   * @return Texture view
   */
  virtual TextureViewHandle
  createTextureView(const TextureViewDescription &description) = 0;

  /**
   * @brief Destroy texture view
   *
   * @param handle Texture view handle
   */
  virtual void destroyTextureView(TextureViewHandle handle) = 0;

  /**
   * @brief Create render pass
   *
   * @param description Render pass description
   * @return Render pass
   */
  virtual RenderPassHandle
  createRenderPass(const RenderPassDescription &description) = 0;

  /**
   * @brief Destroy render pass
   *
   * @param handle Render pass handle
   */
  virtual void destroyRenderPass(RenderPassHandle handle) = 0;

  /**
   * @brief Create framebuffer
   *
   * @param description Framebuffer description
   * @return Framebuffer
   */
  virtual FramebufferHandle
  createFramebuffer(const FramebufferDescription &description) = 0;

  /**
   * @brief Destroy framebuffer
   *
   * @param handle Framebuffer handle
   */
  virtual void destroyFramebuffer(FramebufferHandle handle) = 0;

  /**
   * @brief Create graphics pipeline
   *
   * @param description Graphics pipeline description
   * @param handle Pipeline handle
   */
  virtual void createPipeline(const GraphicsPipelineDescription &description,
                              PipelineHandle handle) = 0;

  /**
   * @brief Create compute pipeline
   *
   * @param description Compute pipeline description
   * @param handle Pipeline handle
   */
  virtual void createPipeline(const ComputePipelineDescription &description,
                              PipelineHandle handle) = 0;

  /**
   * @brief Destroy pipeline
   *
   * @param handle Pipeline handle
   */
  virtual void destroyPipeline(PipelineHandle handle) = 0;

  /**
   * @brief Check if device has pipeline
   *
   * @param handle Pipeline handle
   * @retval true Device has pipeline
   * @retval false Device does not have pipeline
   */
  virtual bool hasPipeline(PipelineHandle handle) = 0;
};

} // namespace liquid::rhi
