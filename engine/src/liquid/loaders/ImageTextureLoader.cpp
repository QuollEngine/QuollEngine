#include "liquid/core/Base.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "ImageTextureLoader.h"
#include <vulkan/vulkan.hpp>

namespace liquid {

ImageTextureLoader::ImageTextureLoader(rhi::ResourceRegistry &registry)
    : mRegistry(registry) {}

rhi::TextureHandle ImageTextureLoader::loadFromFile(const String &filename) {
  liquid::rhi::TextureDescription description;
  int width = 0, height = 0, channels = 0;

  description.data =
      stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  LIQUID_ASSERT(description.data, "Failed to load image: " + filename);

  description.format = VK_FORMAT_R8G8B8A8_SRGB;
  description.width = width;
  description.height = height;
  description.usage = rhi::TextureUsage::Color |
                      rhi::TextureUsage::TransferDestination |
                      rhi::TextureUsage::Sampled;
  description.type = rhi::TextureType::Standard;
  description.size = width * height * channels;

  return mRegistry.setTexture(description);
}

} // namespace liquid
