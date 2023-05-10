#pragma once

#include "liquid/asset/AssetCache.h"

namespace liquid::editor {

/**
 * @brief HDRI Importer
 *
 * Imports HDR files and generates
 * environment assets
 */
class HDRIImporter {
  /**
   * @brief Cubemap data
   */
  struct CubemapData {
    /**
     * Cubemap texture
     */
    rhi::TextureHandle texture;

    /**
     * Cubemap levels
     */
    std::vector<TextureAssetLevel> levels;
  };

public:
  /**
   * @brief Create HDRI importer
   *
   * @param assetCache Asset cache
   * @param renderStorage Render storage
   */
  HDRIImporter(AssetCache &assetCache, RenderStorage &renderStorage);

  /**
   * @brief Load HDRI from file
   *
   * @param originalAssetPath Original asset path
   * @param engineAssetPath Engine asset path
   * @return Path to newly created environment asset
   */
  Result<Path> loadFromPath(const Path &originalAssetPath,
                            const Path &engineAssetPath);

  /**
   * @brief Load HDRI from file to a device
   *
   * @param originalAssetPath Original asset path
   * @param renderStorage Render storage
   * @return Texture handle
   */
  rhi::TextureHandle loadFromPathToDevice(const Path &originalAssetPath,
                                          RenderStorage &renderStorage);

private:
  /**
   * @brief Convert equirectangular image to cubemap
   *
   * @param data Equirectangular image data
   * @param width Equirectangular image width
   * @param height Equirectangular image height
   * @return Cubemap
   */
  CubemapData convertEquirectangularToCubemap(float *data, uint32_t width,
                                              uint32_t height);

  /**
   * @brief Generate irradiance map
   *
   * @param unfilteredCubemap Unfiltered cubemap
   * @param path Path to irradiance map
   * @return Irradiance map texture asset
   */
  Result<TextureAssetHandle>
  generateIrradianceMap(const CubemapData &unfilteredCubemap, const Path &path);

  /**
   * @brief Generate specular map
   *
   * @param unfilteredCubemap Unfiltered cubemap
   * @param path Path to specular map
   * @return Specular map texture asset
   */
  Result<TextureAssetHandle>
  generateSpecularMap(const CubemapData &unfilteredCubemap, const Path &path);

private:
  AssetCache &mAssetCache;
  RenderStorage &mRenderStorage;
  rhi::Descriptor mDescriptorGenerateCubemap;
  rhi::PipelineHandle mPipelineGenerateCubemap;
  rhi::PipelineHandle mPipelineGenerateIrradianceMap;
  rhi::PipelineHandle mPipelineGenerateSpecularMap;
};

} // namespace liquid::editor
