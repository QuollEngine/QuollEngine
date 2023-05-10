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

    rhi::DescriptorLayoutDescription description{{binding}};
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

    rhi::DescriptorLayoutDescription description{{binding}};
    mMaterialDescriptorLayout = mDevice->createDescriptorLayout(description);
  }
}

rhi::TextureHandle
RenderStorage::createTexture(const liquid::rhi::TextureDescription &description,
                             bool addToDescriptor) {
  auto handle = getNewTextureHandle();

  mDevice->createTexture(description, handle);

  if (addToDescriptor) {
    std::array<rhi::TextureHandle, 1> textures{handle};
    mGlobalTexturesDescriptor.write(0, textures,
                                    rhi::DescriptorType::CombinedImageSampler,
                                    rhi::castHandleToUint(handle));
  }

  return handle;
}

rhi::TextureViewHandle RenderStorage::createTextureView(
    const liquid::rhi::TextureViewDescription &description,
    bool addToDescriptor) {
  auto handle = mTextureViewCounter.create();

  mDevice->createTextureView(description, handle);

  if (addToDescriptor) {
    std::array<rhi::TextureViewHandle, 1> textures{handle};
    mGlobalTexturesDescriptor.write(0, textures,
                                    rhi::DescriptorType::CombinedImageSampler,
                                    rhi::castHandleToUint(handle));
  }

  return handle;
}

rhi::ShaderHandle
RenderStorage::createShader(const String &name,
                            const liquid::rhi::ShaderDescription &description) {
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

rhi::TextureHandle RenderStorage::createFramebufferRelativeTexture(
    const liquid::rhi::TextureDescription &description, bool addToDescriptor) {

  auto fixedSizeDesc = description;
  fixedSizeDesc.width =
      mDevice->getSwapchain().extent.x * description.width / Hundred;
  fixedSizeDesc.height =
      mDevice->getSwapchain().extent.y * description.height / Hundred;

  auto handle = createTexture(fixedSizeDesc, addToDescriptor);

  mFramebufferRelativeTextures.insert({handle, description});
  if (addToDescriptor) {
    mFramebufferRelativeTexturesInGlobalDescriptor.push_back(handle);
  }

  return handle;
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
RenderStorage::createBuffer(const liquid::rhi::BufferDescription &description) {
  return mDevice->createBuffer(description);
}

rhi::Descriptor RenderStorage::createMaterialDescriptor(rhi::Buffer buffer) {
  auto descriptor = mDevice->createDescriptor(mMaterialDescriptorLayout);
  std::array<rhi::BufferHandle, 1> buffers{buffer.getHandle()};
  descriptor.write(0, buffers, rhi::DescriptorType::UniformBuffer, 0);

  return descriptor;
}

bool RenderStorage::recreateFramebufferRelativeTextures() {
  if (!mNeedsSwapchainResize) {
    return false;
  }

  mDevice->recreateSwapchain();
  mNeedsSwapchainResize = false;
  for (auto &[handle, description] : mFramebufferRelativeTextures) {
    auto fixedSizeDesc = description;
    fixedSizeDesc.width = mWidth * description.width / Hundred;
    fixedSizeDesc.height = mHeight * description.height / Hundred;

    mDevice->createTexture(fixedSizeDesc, handle);
  }

  for (auto handle : mFramebufferRelativeTexturesInGlobalDescriptor) {
    std::array<rhi::TextureHandle, 1> textures{handle};
    mGlobalTexturesDescriptor.write(0, textures,
                                    rhi::DescriptorType::CombinedImageSampler,
                                    rhi::castHandleToUint(handle));
  }

  return true;
}

bool RenderStorage::isFramebufferRelative(rhi::TextureHandle handle) {
  return mFramebufferRelativeTextures.find(handle) !=
         mFramebufferRelativeTextures.end();
}

void RenderStorage::setFramebufferSize(uint32_t width, uint32_t height) {
  mWidth = width;
  mHeight = height;
  mNeedsSwapchainResize = true;
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
