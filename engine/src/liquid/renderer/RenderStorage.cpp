#include "liquid/core/Base.h"
#include "RenderStorage.h"

namespace liquid {

RenderStorage::RenderStorage(rhi::RenderDevice *device) : mDevice(device) {
  static constexpr uint32_t NumSamplers = 1000;

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

  mResizeListener =
      device->addTextureUpdateListener([this](const auto &textures) {
        for (auto texture : textures) {
          mGlobalTexturesDescriptor.write(
              0, {texture}, rhi::DescriptorType::CombinedImageSampler,
              rhi::castHandleToUint(texture));
        }
      });
}

RenderStorage::~RenderStorage() {
  mDevice->removeTextureUpdateListener(mResizeListener);
}

rhi::TextureHandle RenderStorage::createTexture(
    const liquid::rhi::TextureDescription &description) {
  auto texture = mDevice->createTexture(description);

  mGlobalTexturesDescriptor.write(0, {texture},
                                  rhi::DescriptorType::CombinedImageSampler,
                                  rhi::castHandleToUint(texture));

  return texture;
}

rhi::Buffer
RenderStorage::createBuffer(const liquid::rhi::BufferDescription &description) {
  auto buffer = mDevice->createBuffer(description);

  return buffer;
}

} // namespace liquid
