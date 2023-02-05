#include "liquid/core/Base.h"
#include "liquid/renderer/TextureUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "ImageTextureLoader.h"

namespace liquid {

ImageTextureLoader::ImageTextureLoader(RenderStorage &renderStorage)
    : mRenderStorage(renderStorage) {}

rhi::TextureHandle ImageTextureLoader::loadFromFile(const String &filename) {
  liquid::rhi::TextureDescription description;
  int width = 0, height = 0, channels = 0;

  void *data =
      stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  LIQUID_ASSERT(data, "Failed to load image: " + filename);

  description.format = rhi::Format::Rgba8Srgb;
  description.width = width;
  description.height = height;
  description.usage = rhi::TextureUsage::Color |
                      rhi::TextureUsage::TransferDestination |
                      rhi::TextureUsage::Sampled;
  description.type = rhi::TextureType::Standard;

  auto texture = mRenderStorage.createTexture(description);

  TextureUtils::copyDataToTexture(
      mRenderStorage.getDevice(), data, texture,
      rhi::ImageLayout::ShaderReadOnlyOptimal, 1,
      {TextureAssetLevel{0, static_cast<size_t>(width) * height * channels,
                         static_cast<uint32_t>(width),
                         static_cast<uint32_t>(height)}});

  return texture;
}

} // namespace liquid
