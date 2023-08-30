#pragma once

#include "quoll/rhi/RenderHandle.h"
#include "quoll/rhi/Format.h"

namespace quoll {

enum class TextureAssetType { Standard, Cubemap };

/**
 * @brief Texture data for each mip level
 */
struct TextureAssetLevel {
  /**
   * Offset of mip level
   */
  size_t offset = 0;

  /**
   * Size of mip level
   */
  size_t size = 0;

  /**
   * Mip level width
   */
  uint32_t width = 0;

  /**
   * Mip level height
   */
  uint32_t height = 0;
};

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
  std::vector<uint8_t> data;

  /**
   * Mip levels
   *
   * Base texture data is stored in level 0
   */
  std::vector<TextureAssetLevel> levels;

  /**
   * Device handle
   */
  rhi::TextureHandle deviceHandle = rhi::TextureHandle::Null;
};

} // namespace quoll
