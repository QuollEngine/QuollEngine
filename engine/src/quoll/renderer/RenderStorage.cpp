#include "quoll/core/Base.h"
#include "RenderStorage.h"

namespace quoll {

static constexpr u32 Hundred = 100;

RenderStorage::RenderStorage(rhi::RenderDevice *device,
                             MetricsCollector &metricsCollector)
    : mDevice(device), mMetricsCollector(metricsCollector) {
  static constexpr u32 NumSamplers = 1000;
  static constexpr u32 MaxBuffers = 1000;

  {
    rhi::DescriptorLayoutBindingDescription binding0{};
    binding0.binding = 0;
    binding0.type = rhi::DescriptorLayoutBindingType::Dynamic;
    binding0.name = "uGlobalTextures";
    binding0.descriptorCount = NumSamplers;
    binding0.descriptorType = rhi::DescriptorType::SampledImage;
    binding0.shaderStage = rhi::ShaderStage::All;

    rhi::DescriptorLayoutBindingDescription binding1{};
    binding1.binding = 1;
    binding1.type = rhi::DescriptorLayoutBindingType::Dynamic;
    binding1.name = "uGlobalSamplers";
    binding1.descriptorCount = NumSamplers;
    binding1.descriptorType = rhi::DescriptorType::Sampler;
    binding1.shaderStage = rhi::ShaderStage::All;

    rhi::DescriptorLayoutBindingDescription binding2{};
    binding2.binding = 2;
    binding2.type = rhi::DescriptorLayoutBindingType::Dynamic;
    binding2.name = "uGlobalImages";
    binding2.descriptorCount = NumSamplers;
    binding2.descriptorType = rhi::DescriptorType::StorageImage;
    binding2.shaderStage = rhi::ShaderStage::All;

    rhi::DescriptorLayoutDescription description{{binding0, binding1, binding2},
                                                 "Global textures"};
    auto layout = mDevice->createDescriptorLayout(description);

    mGlobalTexturesDescriptor = mDevice->createDescriptor(layout);
  }

  {
    rhi::SamplerDescription description{};
    description.debugName = "default";
    mDefaultSampler = createSampler(description);
  }
}

rhi::TextureHandle
RenderStorage::createTexture(const quoll::rhi::TextureDescription &description,
                             bool addToDescriptor) {
  auto handle = getNewTextureHandle();

  mDevice->createTexture(description, handle);

  if (addToDescriptor) {
    this->addToDescriptor(handle);
  }

  return handle;
}

rhi::TextureHandle
RenderStorage::createTextureView(const rhi::TextureViewDescription &description,
                                 bool addToDescriptor) {
  auto handle = getNewTextureHandle();

  mDevice->createTextureView(description, handle);

  if (addToDescriptor) {
    this->addToDescriptor(handle);
  }

  return handle;
}

rhi::SamplerHandle
RenderStorage::createSampler(const rhi::SamplerDescription &description,
                             bool addToDescriptor) {
  auto handle = mSamplerCounter.create();

  mDevice->createSampler(description, handle);

  if (addToDescriptor) {
    this->addToDescriptor(handle);
  }

  return handle;
}

void RenderStorage::destroyTexture(rhi::TextureHandle handle) {
  mDevice->destroyTexture(handle);
}

rhi::ShaderHandle
RenderStorage::createShader(const String &name,
                            const rhi::ShaderDescription &description) {
  auto handle = mShaderCounter.create();
  mDevice->createShader(description, handle);

  mShaderMap.insert({name, handle});
  return handle;
}

rhi::ShaderHandle RenderStorage::getShader(const String &name) {
  return mShaderMap.at(name);
}

void RenderStorage::addToDescriptor(rhi::TextureHandle handle) {
  std::array<rhi::TextureHandle, 1> textures{handle};

  auto usage = mDevice->getTextureDescription(handle).usage;

  if (BitwiseEnumContains(usage, rhi::TextureUsage::Sampled)) {
    mGlobalTexturesDescriptor.write(0, textures,
                                    rhi::DescriptorType::SampledImage,
                                    rhi::castHandleToUint(handle));
  }

  if (BitwiseEnumContains(usage, rhi::TextureUsage::Storage)) {
    mGlobalTexturesDescriptor.write(2, textures,
                                    rhi::DescriptorType::StorageImage,
                                    rhi::castHandleToUint(handle));
  }
}

void RenderStorage::addToDescriptor(rhi::SamplerHandle handle) {
  std::array<rhi::SamplerHandle, 1> samplers{handle};

  mGlobalTexturesDescriptor.write(1, samplers, rhi::castHandleToUint(handle));
}

rhi::TextureHandle RenderStorage::getNewTextureHandle() {
  return mTextureCounter.create();
}

rhi::RenderPassHandle RenderStorage::getNewRenderPassHandle() {
  return mRenderPassCounter.create();
}

rhi::FramebufferHandle RenderStorage::getNewFramebufferHandle() {
  return mFramebufferCounter.create();
}

rhi::Buffer
RenderStorage::createBuffer(const rhi::BufferDescription &description) {
  return mDevice->createBuffer(description);
}

rhi::PipelineHandle RenderStorage::addPipeline(
    const rhi::GraphicsPipelineDescription &description) {
  mPipelineDescriptions.push_back(description);
  mGraphicsPipelineIndices.push_back(mPipelineDescriptions.size() - 1);

  return static_cast<rhi::PipelineHandle>(mPipelineDescriptions.size());
}

rhi::PipelineHandle
RenderStorage::addPipeline(const rhi::ComputePipelineDescription &description) {
  mPipelineDescriptions.push_back(description);
  mComputePipelineIndices.push_back(mPipelineDescriptions.size() - 1);

  return static_cast<rhi::PipelineHandle>(mPipelineDescriptions.size());
}

} // namespace quoll
