#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "ImageTextureLoader.h"

namespace liquid {

ImageTextureLoader::ImageTextureLoader(ResourceAllocator *resourceAllocator_)
    : resourceAllocator(resourceAllocator_) {}

SharedPtr<Texture> ImageTextureLoader::loadFromFile(const String &filename) {
  TextureData textureData{};

  int width = 0, height = 0, channels = 0;

  textureData.data =
      stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);

  if (!textureData.data) {
    throw std::runtime_error("Failed to load image: " + filename);
  }

  textureData.width = width;
  textureData.height = height;
  textureData.channels = channels;

  auto texture = resourceAllocator->createTexture2D(textureData);

  delete textureData.data;

  return texture;
}

} // namespace liquid
