#include "liquid/core/Base.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "ImageTextureLoader.h"
#include <vulkan/vulkan.hpp>

namespace liquid {

ImageTextureLoader::ImageTextureLoader(rhi::ResourceRegistry &registry_)
    : registry(registry_) {}

rhi::TextureHandle ImageTextureLoader::loadFromFile(const String &filename) {
  liquid::rhi::TextureDescription description;
  int width = 0, height = 0, channels = 0;

  description.data =
      stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  LIQUID_ASSERT(description.data, "Failed to load image: " + filename);

  description.format = VK_FORMAT_R8G8B8A8_SRGB;
  description.width = width;
  description.height = height;
  description.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
  description.usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  description.type = rhi::TextureType::Standard;
  description.size = width * height * channels;

  return registry.addTexture(description);
}

} // namespace liquid
