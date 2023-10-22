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
  static void
  copyDataToTexture(rhi::RenderDevice *device, void *source,
                    rhi::TextureHandle destination,
                    rhi::ImageLayout destinationLayout, u32 destinationLayers,
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
                    rhi::ImageLayout sourceLayout, u32 sourceLayers,
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
                                        rhi::ImageLayout layout, u32 layers,
                                        u32 levels, u32 width, u32 height);

  /**
   * @brief Get buffer size from texture level data
   *
   * @param levels Levels
   * @return Buffer size
   */
  static usize
  getBufferSizeFromLevels(const std::vector<TextureAssetLevel> &levels);
};

} // namespace quoll
