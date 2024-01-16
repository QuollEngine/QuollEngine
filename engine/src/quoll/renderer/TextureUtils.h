#pragma once

#include "quoll/asset/TextureAsset.h"
#include "quoll/rhi/RenderDevice.h"

namespace quoll {

class TextureUtils {
public:
  static void
  copyDataToTexture(rhi::RenderDevice *device, void *source,
                    rhi::TextureHandle destination,
                    rhi::ImageLayout destinationLayout, u32 destinationLayers,
                    const std::vector<TextureAssetMipLevel> &destinationLevels);

  static void
  copyTextureToData(rhi::RenderDevice *device, rhi::TextureHandle source,
                    rhi::ImageLayout sourceLayout, u32 sourceLayers,
                    const std::vector<TextureAssetMipLevel> &sourceLevels,
                    void *destination);

  /**
   * Generate mip maps for texture
   *
   * Texture must have default base image
   * data already stored
   */
  static void generateMipMapsForTexture(rhi::RenderDevice *device,
                                        rhi::TextureHandle texture,
                                        rhi::ImageLayout layout, u32 layers,
                                        u32 levels, u32 width, u32 height);

  static usize
  getBufferSizeFromLevels(const std::vector<TextureAssetMipLevel> &levels);
};

} // namespace quoll
