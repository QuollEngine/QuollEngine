#pragma once

#include "liquid/rhi/RenderDevice.h"

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
   * @param device Render device
   */
  ImageTextureLoader(rhi::RenderDevice *device);

  /**
   * @brief Load image from filename
   *
   * @param filename Filename
   * @return 2D Texture
   */
  rhi::TextureHandle loadFromFile(const String &filename);

private:
  rhi::RenderDevice *mDevice;
};

} // namespace liquid
