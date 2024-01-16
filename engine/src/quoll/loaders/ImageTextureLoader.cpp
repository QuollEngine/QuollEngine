#include "quoll/core/Base.h"
#include "quoll/renderer/TextureUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "ImageTextureLoader.h"

namespace quoll {

ImageTextureLoader::ImageTextureLoader(RenderStorage &renderStorage)
    : mRenderStorage(renderStorage) {}

rhi::TextureHandle ImageTextureLoader::loadFromFile(const Path &path) {
  quoll::rhi::TextureDescription description;
  int width = 0, height = 0, channels = 0;

  void *data = stbi_load(path.string().c_str(), &width, &height, &channels,
                         STBI_rgb_alpha);
  QuollAssert(data, "Failed to load image: " + path.string());

  description.format = rhi::Format::Rgba8Srgb;
  description.width = width;
  description.height = height;
  description.usage = rhi::TextureUsage::Color |
                      rhi::TextureUsage::TransferDestination |
                      rhi::TextureUsage::Sampled;
  description.type = rhi::TextureType::Standard;
  description.debugName = path.filename().string();

  auto texture = mRenderStorage.createTexture(description);

  TextureUtils::copyDataToTexture(
      mRenderStorage.getDevice(), data, texture,
      rhi::ImageLayout::ShaderReadOnlyOptimal, 1,
      {TextureAssetMipLevel{0, static_cast<usize>(width) * height * channels,
                            static_cast<u32>(width),
                            static_cast<u32>(height)}});

  return texture;
}

} // namespace quoll
