#pragma once

#include "Buffer.h"
#include "BufferDescription.h"
#include "DescriptorLayoutDescription.h"
#include "DeviceStats.h"
#include "FramebufferDescription.h"
#include "PhysicalDeviceInformation.h"
#include "PipelineDescription.h"
#include "RenderFrame.h"
#include "RenderPassDescription.h"
#include "SamplerDescription.h"
#include "ShaderDescription.h"
#include "Swapchain.h"
#include "TextureDescription.h"
#include "TextureViewDescription.h"

namespace quoll::rhi {

class RenderDevice : NoCopyMove {
public:
  static constexpr usize NumFrames = 2;

public:
  RenderDevice() = default;

  virtual ~RenderDevice() = default;

  virtual RenderCommandList requestImmediateCommandList() = 0;

  virtual void submitImmediate(RenderCommandList &commandList) = 0;

  virtual RenderFrame beginFrame() = 0;

  virtual void endFrame(const RenderFrame &renderFrame) = 0;

  virtual void collectTimestamps(std::vector<u64> &timestamps) = 0;

  virtual void waitForIdle() = 0;

  virtual const PhysicalDeviceInformation getDeviceInformation() = 0;

  virtual const DeviceStats &getDeviceStats() const = 0;

  virtual void destroyResources() = 0;

  virtual Swapchain getSwapchain() = 0;

  virtual void recreateSwapchain() = 0;

  virtual void createShader(const ShaderDescription &description,
                            ShaderHandle handle) = 0;

  virtual DescriptorLayoutHandle
  createDescriptorLayout(const DescriptorLayoutDescription &description) = 0;

  virtual Descriptor createDescriptor(DescriptorLayoutHandle layout) = 0;

  virtual Buffer createBuffer(const BufferDescription &description) = 0;

  virtual void destroyBuffer(BufferHandle handle) = 0;

  virtual void createTexture(const TextureDescription &description,
                             TextureHandle handle) = 0;

  virtual const TextureDescription
  getTextureDescription(TextureHandle handle) const = 0;

  virtual void destroyTexture(TextureHandle handle) = 0;

  virtual void createTextureView(const TextureViewDescription &description,
                                 TextureHandle handle) = 0;

  virtual void createSampler(const SamplerDescription &description,
                             SamplerHandle handle) = 0;

  virtual void destroySampler(SamplerHandle handle) = 0;

  virtual void createRenderPass(const RenderPassDescription &description,
                                RenderPassHandle handle) = 0;

  virtual void destroyRenderPass(RenderPassHandle handle) = 0;

  virtual void createFramebuffer(const FramebufferDescription &description,
                                 FramebufferHandle handle) = 0;

  virtual void destroyFramebuffer(FramebufferHandle handle) = 0;

  virtual void createPipeline(const GraphicsPipelineDescription &description,
                              PipelineHandle handle) = 0;

  virtual void createPipeline(const ComputePipelineDescription &description,
                              PipelineHandle handle) = 0;

  virtual void destroyPipeline(PipelineHandle handle) = 0;

  virtual bool hasPipeline(PipelineHandle handle) = 0;
};

} // namespace quoll::rhi
