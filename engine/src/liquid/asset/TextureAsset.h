#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid {

enum class TextureAssetType { Standard, Cubemap };

/**
 * @brief Texture asset data
 */
struct TextureAsset {
  /**
   * Width
   */
  uint32_t width = 0;

  /**
   * Height
   */
  uint32_t height = 0;

  /**
   * Layers
   */
  uint32_t layers = 0;

  /**
   * Texture type
   */
  TextureAssetType type = TextureAssetType::Standard;

  /**
   * Texture format
   */
  rhi::Format format = rhi::Format::Rgba8Srgb;

  /**
   * Raw texture data
   */
  void *data = nullptr;

  /**
   * Device handle
   */
  rhi::TextureHandle deviceHandle = rhi::TextureHandle::Invalid;
};

} // namespace liquid
