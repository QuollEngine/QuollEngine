#pragma once

#include "quoll/asset/AssetCache.h"
#include "quoll/rhi/Descriptor.h"
#include "UUIDMap.h"

namespace quoll::editor {

class HDRIImporter {
  struct CubemapData {
    rhi::TextureHandle texture;

    std::vector<TextureAssetMipLevel> levels;
  };

public:
  HDRIImporter(AssetCache &assetCache, RenderStorage &renderStorage);

  Result<UUIDMap> loadFromPath(const Path &sourceAssetPath,
                               const UUIDMap &uuids);

  rhi::TextureHandle loadFromPathToDevice(const Path &sourceAssetPath,
                                          RenderStorage &renderStorage);

private:
  CubemapData convertEquirectangularToCubemap(f32 *data, u32 width, u32 height);

  Result<TextureAssetHandle>
  generateIrradianceMap(const CubemapData &unfilteredCubemap, const Uuid &uuid,
                        const String &name);

  Result<TextureAssetHandle>
  generateSpecularMap(const CubemapData &unfilteredCubemap, const Uuid &uuid,
                      const String &name);

private:
  AssetCache &mAssetCache;
  RenderStorage &mRenderStorage;
  rhi::Descriptor mDescriptorGenerateCubemap;
  rhi::PipelineHandle mPipelineGenerateCubemap;
  rhi::PipelineHandle mPipelineGenerateIrradianceMap;
  rhi::PipelineHandle mPipelineGenerateSpecularMap;
};

} // namespace quoll::editor
