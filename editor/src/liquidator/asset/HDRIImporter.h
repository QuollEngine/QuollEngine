#pragma once

#include "liquid/asset/AssetCache.h"
#include "liquid/renderer/ShaderLibrary.h"

namespace liquid::editor {

class HDRIImporter {
  struct CubemapData {
    rhi::TextureHandle texture;
    std::vector<TextureAssetLevel> levels;
  };

public:
  /**
   * @brief Create HDRI importer
   *
   * @param assetCache Asset cache
   * @param device Render device
   */
  HDRIImporter(AssetCache &assetCache, rhi::RenderDevice *device);

  /**
   * @brief Load HDRI from file
   *
   * @param originalAssetPath Original asset path
   * @param engineAssetPath Engine asset path
   * @return Path to newly created environment asset
   */
  Result<Path> loadFromPath(const Path &originalAssetPath,
                            const Path &engineAssetPath);

private:
  CubemapData convertEquirectangularToCubemap(float *data, uint32_t width,
                                              uint32_t height);

  CubemapData generateIrradianceMap(const CubemapData &unfilteredCubemap);

  CubemapData generateSpecularMap(const CubemapData &unfilteredCubemap);

  CubemapData generateBrdfLut(const CubemapData &unfilteredCubemap);

private:
  AssetCache &mAssetCache;
  ShaderLibrary mShaderLibrary;
  rhi::RenderDevice *mDevice;
  rhi::Descriptor mDescriptorGenerateCubemap;
  rhi::PipelineHandle mPipelineGenerateCubemap;
  rhi::PipelineHandle mPipelineGenerateIrradianceMap;
  rhi::PipelineHandle mPipelineGenerateSpecularMap;
  rhi::PipelineHandle mPipelineGenerateBrdfLut;
};

} // namespace liquid::editor
