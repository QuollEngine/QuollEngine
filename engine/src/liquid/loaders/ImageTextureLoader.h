#pragma once

#include "liquid/renderer/RenderStorage.h"

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
   * @param renderStorage Render storage
   */
  ImageTextureLoader(RenderStorage &renderStorage);

  /**
   * @brief Load image from filename
   *
   * @param filename Filename
   * @return 2D Texture
   */
  rhi::TextureHandle loadFromFile(const Path &filename);

private:
  RenderStorage &mRenderStorage;
};

} // namespace liquid
