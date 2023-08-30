#pragma once

#include "quoll/rhi/RenderDevice.h"
#include "MockBuffer.h"
#include "MockTexture.h"
#include "MockDescriptor.h"
#include "MockPipeline.h"
#include "MockCommandList.h"
#include "MockResourceMap.h"

namespace quoll::rhi {

/**
 * @brief Mock render device
 */
class MockRenderDevice : public RenderDevice {
public:
  /**
   * @brief Create mock render device
   */
  MockRenderDevice();

  /**
   * @brief Get buffer
   *
   * @param handle Buffer handle
   * @return Buffer
   */
  inline MockBuffer *getBuffer(BufferHandle handle) {
    return mBuffers.at(handle).get();
  }

public:
  /**
   * @brief Request immediate command list
   *
   * @return Immediate command list
   */
  RenderCommandList requestImmediateCommandList() override;

  /**
   * @brief Submit immediate command list
   *
   * @param commandList Immediate command list
   */
  void submitImmediate(RenderCommandList &commandList) override;

  /**
   * @brief Begin frame
   *
   * @return Render frame
   */
  RenderFrame beginFrame() override;

  /**
   * @brief End frame
   *
   * @param renderFrame Render frame
   */
  void endFrame(const RenderFrame &renderFrame) override;

  /**
   * @brief Wait for idle
   */
  void waitForIdle() override;

  /**
   * @brief Get physical device information
   *
   * @return Physical device information
   */
  const PhysicalDeviceInformation getDeviceInformation() override;

  /**
   * @brief Get device stats
   *
   * @return Device stats
   */
  const DeviceStats &getDeviceStats() const override;

  /**
   * @brief Destroy all resources
   */
  void destroyResources() override;

  /**
   * @brief Get swapchain
   *
   * @return Get swapchain
   */
  Swapchain getSwapchain() override;

  /**
   * @brief Recreate swapchain
   */
  void recreateSwapchain() override;

  /**
   * @brief Create shader
   *
   * @param description Shader description
   * @param handle Shader handle
   */
  void createShader(const ShaderDescription &description,
                    ShaderHandle handle) override;

  /**
   * @brief Create descriptor layout
   *
   * @param description Descriptor layout description
   * @return Descriptor layout
   */
  DescriptorLayoutHandle createDescriptorLayout(
      const DescriptorLayoutDescription &description) override;

  /**
   * @brief Create descriptor
   *
   * @param layout Descriptor layout
   * @return Descriptor
   */
  Descriptor createDescriptor(DescriptorLayoutHandle layout) override;

  /**
   * @brief Create hardware buffer
   *
   * @param description Buffer description
   * @return Buffer
   */
  Buffer createBuffer(const BufferDescription &description) override;

  /**
   * @brief Destroy buffer
   *
   * @param handle Buffer handle
   */
  void destroyBuffer(BufferHandle handle) override;

  /**
   * @brief Create texture
   *
   * @param description Texture description
   * @param handle Texture handle
   */
  void createTexture(const TextureDescription &description,
                     rhi::TextureHandle handle) override;

  /**
   * @brief Get texture description
   *
   * @param handle Texture handle
   * @return Texture description
   */
  const TextureDescription
  getTextureDescription(TextureHandle handle) const override;

  /**
   * @brief Get texture view description
   *
   * @param handle Texture handle
   * @return Texture view description
   */
  const TextureViewDescription
  getTextureViewDescription(TextureHandle handle) const;

  /**
   * @brief Check if texture exists in device
   *
   * @param handle Texture handle
   * @retval true Texture exists
   * @retval false Texture does not exist
   */
  inline bool hasTexture(TextureHandle handle) const {
    return mTextures.exists(handle);
  }

  /**
   * @brief Destroy texture
   *
   * @param handle Texture handle
   */
  void destroyTexture(TextureHandle handle) override;

  /**
   * @brief Get texture updates
   *
   * @param handle Texture handle
   * @return Texture updates
   */
  uint32_t getTextureUpdates(TextureHandle handle);

  /**
   * @brief Create texture view
   *
   * @param description Texture view description
   * @param handle Texture handle
   */
  void createTextureView(const TextureViewDescription &description,
                         TextureHandle handle) override;

  /**
   * @brief Create sampler
   *
   * @param description Sampler description
   * @param handle Sampler handle
   */
  void createSampler(const SamplerDescription &description,
                     SamplerHandle handle) override;

  /**
   * @brief Destroy sampler
   *
   * @param handle Sampler handle
   */
  void destroySampler(SamplerHandle handle) override;

  /**
   * @brief Create render pass
   *
   * @param description Render pass description
   * @param handle Render pass handle
   */
  void createRenderPass(const RenderPassDescription &description,
                        RenderPassHandle handle) override;

  /**
   * @brief Destroy render pass
   *
   * @param handle Render pass handle
   */
  void destroyRenderPass(RenderPassHandle handle) override;

  /**
   * @brief Get render pass description
   *
   * @param handle Render pass handle
   * @return Render pass description
   */
  const RenderPassDescription
  getRenderPassDescription(RenderPassHandle handle) const;

  /**
   * @brief Check if render pass exists in device
   *
   * @param handle Render pass handle
   * @retval true Render pass exists
   * @retval false Render pass does not exist
   */
  inline bool hasRenderPass(RenderPassHandle handle) const {
    return mRenderPasses.exists(handle);
  }

  /**
   * @brief Create framebuffer
   *
   * @param description Framebuffer description
   * @param handle Framebuffer handle
   */
  void createFramebuffer(const FramebufferDescription &description,
                         FramebufferHandle handle) override;

  /**
   * @brief Destroy framebuffer
   *
   * @param handle Framebuffer handle
   */
  void destroyFramebuffer(FramebufferHandle handle) override;

  /**
   * @brief Get framebuffer description
   *
   * @param handle Framebuffer handle
   * @return Framebuffer description
   */
  const FramebufferDescription
  getFramebufferDescription(FramebufferHandle handle) const;

  /**
   * @brief Check if framebuffer exists in device
   *
   * @param handle Framebuffer handle
   * @retval true Framebuffer exists
   * @retval false Framebuffer does not exist
   */
  inline bool hasFramebuffer(FramebufferHandle handle) const {
    return mFramebuffers.exists(handle);
  }

  /**
   * @brief Create graphics pipeline
   *
   * @param description Pipeline description
   * @param handle Graphics pipeline handle
   */
  void createPipeline(const GraphicsPipelineDescription &description,
                      PipelineHandle handle) override;

  /**
   * @brief Create compute pipeline
   *
   * @param description Compute pipeline description
   * @param handle Compute pipeline handle
   */
  void createPipeline(const ComputePipelineDescription &description,
                      PipelineHandle handle) override;

  /**
   * @brief Destroy pipeline
   *
   * @param handle Pipeline handle
   */
  void destroyPipeline(PipelineHandle handle) override;

  /**
   * @brief Get pipeline
   *
   * @param handle Pipeline handle
   * @return Mock pipeline
   */
  const MockPipeline &getPipeline(PipelineHandle handle) const;

  /**
   * @brief Check if device has pipeline
   *
   * @param handle Pipeline handle
   * @retval true Device has pipeline
   * @retval false Device does not have pipeline
   */
  inline bool hasPipeline(PipelineHandle handle) override {
    return mPipelines.exists(handle);
  }

private:
  MockResourceMap<BufferHandle, std::unique_ptr<MockBuffer>> mBuffers;
  MockResourceMap<TextureHandle, MockTexture> mTextures;
  MockResourceMap<SamplerHandle, SamplerDescription> mSamplers;
  MockResourceMap<FramebufferHandle, FramebufferDescription> mFramebuffers;
  MockResourceMap<RenderPassHandle, RenderPassDescription> mRenderPasses;

  MockResourceMap<ShaderHandle, ShaderDescription> mShaders;
  MockResourceMap<DescriptorLayoutHandle, DescriptorLayoutDescription>
      mDescriptorLayouts;
  MockResourceMap<DescriptorHandle, std::unique_ptr<MockDescriptor>>
      mDescriptors;
  MockResourceMap<PipelineHandle, MockPipeline> mPipelines;

  std::array<RenderCommandList, NumFrames> mCommandLists;
  std::vector<MockCommandList> mSubmittedCommandLists;
  uint32_t mFrameIndex = 0;

  DeviceStats mDeviceStats;
};

} // namespace quoll::rhi
