#pragma once

#include "quoll/rhi/RenderDevice.h"
#include "quoll/rhi/TextureDescription.h"
#include "HandleCounter.h"

namespace quoll {

class MetricsCollector;

/**
 * @brief Render storage
 *
 * Abstracts away low level render device
 * implementations from the codebase
 */
class RenderStorage {
public:
  RenderStorage(rhi::RenderDevice *device, MetricsCollector &metricsCollector);

  RenderStorage(const RenderStorage &) = delete;
  RenderStorage &operator=(const RenderStorage &) = delete;
  RenderStorage(RenderStorage &&) = delete;
  RenderStorage &operator=(RenderStorage &&) = delete;

  ~RenderStorage() = default;

  rhi::TextureHandle createTexture(const rhi::TextureDescription &description,
                                   bool addToDescriptor = true);

  rhi::TextureHandle
  createTextureView(const rhi::TextureViewDescription &description,
                    bool addToDescriptor = true);

  rhi::SamplerHandle createSampler(const rhi::SamplerDescription &description,
                                   bool addToDescriptor = true);

  void destroyTexture(rhi::TextureHandle handle);

  rhi::ShaderHandle createShader(const String &name,
                                 const rhi::ShaderDescription &description);

  rhi::ShaderHandle getShader(const String &name);

  void addToDescriptor(rhi::TextureHandle handle);

  void addToDescriptor(rhi::SamplerHandle handle);

  rhi::TextureHandle getNewTextureHandle();

  rhi::RenderPassHandle getNewRenderPassHandle();

  rhi::FramebufferHandle getNewFramebufferHandle();

  rhi::Buffer createBuffer(const rhi::BufferDescription &description);

  inline const rhi::Descriptor &getGlobalTexturesDescriptor() const {
    return mGlobalTexturesDescriptor;
  }

  inline rhi::SamplerHandle getDefaultSampler() const {
    return mDefaultSampler;
  }

  inline rhi::RenderDevice *getDevice() { return mDevice; }

public:
  rhi::PipelineHandle
  addPipeline(const rhi::GraphicsPipelineDescription &description);

  rhi::PipelineHandle
  addPipeline(const rhi::ComputePipelineDescription &description);

  inline rhi::GraphicsPipelineDescription &
  getGraphicsPipelineDescription(rhi::PipelineHandle handle) {
    return std::get<rhi::GraphicsPipelineDescription>(
        mPipelineDescriptions.at(static_cast<usize>(handle) - 1));
  }

  inline rhi::ComputePipelineDescription &
  getComputePipelineDescription(rhi::PipelineHandle handle) {
    return std::get<rhi::ComputePipelineDescription>(
        mPipelineDescriptions.at(static_cast<usize>(handle) - 1));
  }

  inline MetricsCollector &getMetricsCollector() { return mMetricsCollector; }

private:
  rhi::RenderDevice *mDevice = nullptr;

  rhi::Descriptor mGlobalTexturesDescriptor;

  std::vector<std::variant<rhi::GraphicsPipelineDescription,
                           rhi::ComputePipelineDescription>>
      mPipelineDescriptions;
  std::vector<usize> mGraphicsPipelineIndices;
  std::vector<usize> mComputePipelineIndices;

  static constexpr u32 TextureStart = 10;
  HandleCounter<rhi::ShaderHandle> mShaderCounter;
  HandleCounter<rhi::TextureHandle, TextureStart> mTextureCounter;
  HandleCounter<rhi::RenderPassHandle> mRenderPassCounter;
  HandleCounter<rhi::FramebufferHandle> mFramebufferCounter;
  HandleCounter<rhi::SamplerHandle> mSamplerCounter;

  rhi::SamplerHandle mDefaultSampler = rhi::SamplerHandle::Null;

  std::unordered_map<String, rhi::ShaderHandle> mShaderMap;

  MetricsCollector &mMetricsCollector;
};

} // namespace quoll
