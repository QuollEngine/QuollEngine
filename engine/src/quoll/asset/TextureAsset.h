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
  usize offset = 0;

  /**
   * Size of mip level
   */
  usize size = 0;

  /**
   * Mip level width
   */
  u32 width = 0;

  /**
   * Mip level height
   */
  u32 height = 0;
};

/**
 * @brief Texture asset data
 */
struct TextureAsset {
  /**
   * Width
   */
  u32 width = 0;

  /**
   * Height
   */
  u32 height = 0;

  /**
   * Layers
   */
  u32 layers = 0;

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
  std::vector<u8> data;

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
