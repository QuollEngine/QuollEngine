#pragma once

#include "quoll/asset/Result.h"

namespace quoll {

class RenderStorage;

} // namespace quoll

namespace quoll::editor {

class ImageLoader {
public:
  ImageLoader(AssetCache &assetCache, RenderStorage &renderStorage);

  Result<Uuid> loadFromPath(const Path &sourceAssetPath, const Uuid &uuid,
                            bool generateMipMaps, rhi::Format format);

  Result<Uuid> loadFromMemory(void *data, u32 width, u32 height,
                              const Uuid &uuid, const String &name,
                              bool generateMipMaps, rhi::Format format);

private:
  std::vector<u8> generateMipMapsFromTextureData(
      void *data, const std::vector<TextureAssetMipLevel> &levels,
      rhi::Format format);

private:
  RenderStorage &mRenderStorage;
  AssetCache &mAssetCache;
};

} // namespace quoll::editor
