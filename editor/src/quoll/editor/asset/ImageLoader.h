#pragma once

#include "quoll/rhi/RenderDevice.h"
#include "quoll/asset/Result.h"

namespace quoll::editor {

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
   * @param renderStorage Render storage
   */
  ImageLoader(AssetCache &assetCache, RenderStorage &renderStorage);

  /**
   * @brief Load texture from path
   *
   * @param sourceAssetPath Source asset path
   * @param uuid Asset uuid
   * @param generateMipMaps Generate mip maps
   * @param format Texture format
   * @return Uuid of newly created texture
   */
  Result<Uuid> loadFromPath(const Path &sourceAssetPath, const Uuid &uuid,
                            bool generateMipMaps, rhi::Format format);

  /**
   * @brief Load texture from memory
   *
   * @param data Texture data
   * @param width Texture width
   * @param height Texture height
   * @param uuid Asset uuid
   * @param name Asset name
   * @param generateMipMaps Generate mip maps
   * @param format Texture format
   * @return Uuid of newly created texture
   */
  Result<Uuid> loadFromMemory(void *data, u32 width, u32 height,
                              const Uuid &uuid, const String &name,
                              bool generateMipMaps, rhi::Format format);

private:
  /**
   * @brief Generate mip maps from texture data
   *
   * @param data Texture data
   * @param levels Texture levels
   * @param format Texture format
   * @return Texture data with mip maps
   */
  std::vector<u8>
  generateMipMapsFromTextureData(void *data,
                                 const std::vector<TextureAssetLevel> &levels,
                                 rhi::Format format);

private:
  RenderStorage &mRenderStorage;
  AssetCache &mAssetCache;
};

} // namespace quoll::editor
