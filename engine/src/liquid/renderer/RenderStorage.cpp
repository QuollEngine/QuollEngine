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

  {
    rhi::DescriptorLayoutBindingDescription binding0{};
    binding0.binding = 0;
    binding0.type = rhi::DescriptorLayoutBindingType::Dynamic;
    binding0.name = "uGlobalBuffers";
    binding0.descriptorCount = MaxBuffers;
    binding0.descriptorType = rhi::DescriptorType::StorageBuffer;
    binding0.shaderStage = rhi::ShaderStage::All;

    rhi::DescriptorLayoutBindingDescription binding1{};
    binding1.binding = 1;
    binding1.type = rhi::DescriptorLayoutBindingType::Dynamic;
    binding1.name = "uGlobalUniforms";
    binding1.descriptorCount = MaxBuffers;
    binding1.descriptorType = rhi::DescriptorType::UniformBuffer;
    binding1.shaderStage = rhi::ShaderStage::All;

    rhi::DescriptorLayoutDescription description{{binding0, binding1}};
    auto layout = mDevice->createDescriptorLayout(description);

    mGlobalBuffersDescriptor = mDevice->createDescriptor(layout);
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
  auto texture = mDevice->createTexture(description);

  if (addToDescriptor) {
    mGlobalTexturesDescriptor.write(0, {texture},
                                    rhi::DescriptorType::CombinedImageSampler,
                                    rhi::castHandleToUint(texture));
  }

  return texture;
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

  return handle;
}

rhi::Buffer
RenderStorage::createBuffer(const liquid::rhi::BufferDescription &description) {
  auto buffer = mDevice->createBuffer(description);

  if (description.usage == rhi::BufferUsage::Storage) {
    mGlobalBuffersDescriptor.write(0, {buffer.getHandle()},
                                   rhi::DescriptorType::StorageBuffer,
                                   rhi::castHandleToUint(buffer.getHandle()));
  } else if (description.usage == rhi::BufferUsage::Uniform) {
    mGlobalBuffersDescriptor.write(1, {buffer.getHandle()},
                                   rhi::DescriptorType::UniformBuffer,
                                   rhi::castHandleToUint(buffer.getHandle()));
  }

  return buffer;
}

rhi::Descriptor RenderStorage::createMaterialDescriptor(rhi::Buffer buffer) {
  auto descriptor = mDevice->createDescriptor(mMaterialDescriptorLayout);
  descriptor.write(0, {buffer.getHandle()}, rhi::DescriptorType::UniformBuffer,
                   0);

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

    mDevice->updateTexture(handle, fixedSizeDesc);

    mGlobalTexturesDescriptor.write(0, {handle},
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

} // namespace liquid
