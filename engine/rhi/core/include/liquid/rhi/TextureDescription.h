#pragma once

#include "liquid/rhi/Format.h"

namespace liquid::rhi {

enum class TextureType { Standard, Cubemap };

enum class TextureUsage : uint8_t {
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
  uint32_t width = 0;

  /**
   * Texture height
   */
  uint32_t height = 0;

  /**
   * Texture depth
   */
  uint32_t depth = 1;

  /**
   * Number of layers in texture
   */
  uint32_t layerCount = 1;

  /**
   * Number of mip levels
   */
  uint32_t mipLevelCount = 1;

  /**
   * Number of samples
   */
  uint32_t samples = 1;

  /**
   * Debug name
   */
  String debugName;
};

} // namespace liquid::rhi
