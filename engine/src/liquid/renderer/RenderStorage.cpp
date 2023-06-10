#include "liquid/core/Base.h"
#include "RenderStorage.h"

namespace liquid {

static constexpr uint32_t Hundred = 100;

RenderStorage::RenderStorage(rhi::RenderDevice *device) : mDevice(device) {
  static constexpr uint32_t NumSamplers = 1000;
  static constexpr uint32_t MaxBuffers = 1000;

  {
    rhi::DescriptorLayoutBindingDescription binding{};
    binding.binding = 0;
    binding.type = rhi::DescriptorLayoutBindingType::Dynamic;
    binding.name = "uGlobalTextures";
    binding.descriptorCount = NumSamplers;
    binding.descriptorType = rhi::DescriptorType::CombinedImageSampler;
    binding.shaderStage = rhi::ShaderStage::Fragment;

    rhi::DescriptorLayoutDescription description{{binding}, "Global textures"};
    auto layout = mDevice->createDescriptorLayout(description);

    mGlobalTexturesDescriptor = mDevice->createDescriptor(layout);
  }

  // Material descriptor layout
  {
    rhi::DescriptorLayoutBindingDescription binding{};
    binding.binding = 0;
    binding.type = rhi::DescriptorLayoutBindingType::Static;
    binding.name = "uMaterialDataRaw";
    binding.descriptorCount = 1;
    binding.descriptorType = rhi::DescriptorType::UniformBuffer;
    binding.shaderStage = rhi::ShaderStage::Fragment;

    rhi::DescriptorLayoutDescription description{{binding}, "Material"};
    mMaterialDescriptorLayout = mDevice->createDescriptorLayout(description);
  }
}

rhi::TextureHandle
RenderStorage::createTexture(const liquid::rhi::TextureDescription &description,
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
  mGlobalTexturesDescriptor.write(0, textures,
                                  rhi::DescriptorType::CombinedImageSampler,
                                  rhi::castHandleToUint(handle));
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

rhi::Descriptor RenderStorage::createMaterialDescriptor(rhi::Buffer buffer) {
  auto descriptor = mDevice->createDescriptor(mMaterialDescriptorLayout);
  std::array<rhi::BufferHandle, 1> buffers{buffer.getHandle()};
  descriptor.write(0, buffers, rhi::DescriptorType::UniformBuffer, 0);

  return descriptor;
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

} // namespace liquid
