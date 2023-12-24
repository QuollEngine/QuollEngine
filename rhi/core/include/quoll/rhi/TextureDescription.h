#pragma once

#include "quoll/rhi/Format.h"
#include "quoll/rhi/RenderHandle.h"

namespace quoll::rhi {

enum class TextureType { Standard, Cubemap };

enum class TextureUsage : u8 {
  None = 0,
  Color = 1 << 0,
  Depth = 1 << 1,
  Stencil = 1 << 2,
  Sampled = 1 << 3,
  Storage = 1 << 4,
  TransferSource = 1 << 5,
  TransferDestination = 1 << 6
};

EnableBitwiseEnum(TextureUsage);

/**
 * @brief Texture description
 */
struct TextureDescription {
  /**
   * Texture type
   */
  TextureType type = TextureType::Standard;

  /**
   * Texture usage
   */
  TextureUsage usage = TextureUsage::None;

  /**
   * Texture format
   */
  Format format = Format::Undefined;

  /**
   * Texture width
   */
  u32 width = 0;

  /**
   * Texture height
   */
  u32 height = 0;

  /**
   * Texture depth
   */
  u32 depth = 1;

  /**
   * Number of layers in texture
   */
  u32 layerCount = 1;

  /**
   * Number of mip levels
   */
  u32 mipLevelCount = 1;

  /**
   * Number of samples
   */
  u32 samples = 1;

  /**
   * Debug name
   */
  String debugName;
};

} // namespace quoll::rhi
