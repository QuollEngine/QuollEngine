#pragma once

#include "liquid/rhi/RenderDevice.h"
#include "liquid/asset/Result.h"

namespace liquid::editor {

/**
 * @brief Image loader
 *
 * Loads static images and generates mip levels
 */
class ImageLoader {
public:
  /**
   * @brief Create image loader
   *
   * @param assetCache Asset cache
   * @param device Render device for processing
   */
  ImageLoader(AssetCache &assetCache, rhi::RenderDevice *device);

  /**
   * @brief Load texture from path
   *
   * @param originalAssetPath Original asset path
   * @param engineAssetPath Engine asset path
   * @param generateMipMaps Generate mip maps
   * @return Path to newly created texture
   */
  Result<Path> loadFromPath(const Path &originalAssetPath,
                            const Path &engineAssetPath, bool generateMipMaps);

  /**
   * @brief Load texture from memory
   *
   * @param data Texture data
   * @param width Texture width
   * @param height Texture height
   * @param engineAssetPath Engine asset path
   * @param generateMipMaps Generate mip maps
   * @return Path to newly created texture
   */
  Result<Path> loadFromMemory(void *data, uint32_t width, uint32_t height,
                              const Path &engineAssetPath,
                              bool generateMipMaps);

private:
  /**
   * @brief Generate mip maps from texture data
   *
   * @param data Texture data
   * @param levels Texture levels
   * @return Texture data with mip maps
   */
  std::vector<uint8_t>
  generateMipMapsFromTextureData(void *data,
                                 const std::vector<TextureAssetLevel> &levels);

private:
  rhi::RenderDevice *mDevice;
  AssetCache &mAssetCache;
};

} // namespace liquid::editor
