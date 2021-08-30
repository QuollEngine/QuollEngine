#pragma once

#include "renderer/ResourceAllocator.h"

namespace liquid {

/**
 * @brief Loads images in various formats as textures
 *
 * Uses STB to load images in various formats
 */
class ImageTextureLoader {
public:
  /**
   * @brief Create image texture loader
   *
   * @param resourceAllocator Resource allocator
   */
  ImageTextureLoader(ResourceAllocator *resourceAllocator);

  /**
   * @brief Load image from filename
   *
   * @param filename Filename
   * @return 2D Texture
   */
  SharedPtr<Texture> loadFromFile(const String &filename);

private:
  ResourceAllocator *resourceAllocator;
};

} // namespace liquid
