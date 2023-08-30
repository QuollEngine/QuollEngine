#pragma once

#include "quoll/asset/TextureAsset.h"
#include "quoll/rhi/RenderDevice.h"

namespace quoll {

/**
 * @brief Texture utilities
 *
 * General texture utilities that
 * can be used to work with textures
 */
class TextureUtils {
public:
  /**
   * @brief Copy data to texture
   *
   * @param device Render device
   * @param source Source data
   * @param destination Destination texture
   * @param destinationLayout Destination texture layout
   * @param destinationLayers Destination texture layers
   * @param destinationLevels Destination texture levels
   */
  static void copyDataToTexture(
      rhi::RenderDevice *device, void *source, rhi::TextureHandle destination,
      rhi::ImageLayout destinationLayout, uint32_t destinationLayers,
      const std::vector<TextureAssetLevel> &destinationLevels);

  /**
   * @brief Copy texture to data
   *
   * @param device Render device
   * @param source Source texture
   * @param sourceLayout Source texture layout
   * @param sourceLayers Source texture layers
   * @param sourceLevels Texture levels
   * @param destination Destination data
   */
  static void
  copyTextureToData(rhi::RenderDevice *device, rhi::TextureHandle source,
                    rhi::ImageLayout sourceLayout, uint32_t sourceLayers,
                    const std::vector<TextureAssetLevel> &sourceLevels,
                    void *destination);

  /**
   * @brief Generate mip maps for texture
   *
   * Texture must have default base image
   * data already stored
   *
   * @param device Render device
   * @param texture Texture
   * @param layout Texture layout
   * @param layers Texture layers count
   * @param levels Texture levels count
   * @param width Texture width
   * @param height Texture height
   */
  static void generateMipMapsForTexture(rhi::RenderDevice *device,
                                        rhi::TextureHandle texture,
                                        rhi::ImageLayout layout,
                                        uint32_t layers, uint32_t levels,
                                        uint32_t width, uint32_t height);

  /**
   * @brief Get buffer size from texture level data
   *
   * @param levels Levels
   * @return Buffer size
   */
  static size_t
  getBufferSizeFromLevels(const std::vector<TextureAssetLevel> &levels);
};

} // namespace quoll
