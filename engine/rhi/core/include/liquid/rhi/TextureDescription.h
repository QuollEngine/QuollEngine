#pragma once

#include "liquid/rhi/Format.h"

namespace liquid::rhi {

enum class TextureType { Standard, Cubemap };

enum class TextureUsage : uint8_t {
  None = 0,
  Color = 1 << 0,
  Depth = 1 << 1,
  Sampled = 1 << 2,
  Storage = 1 << 3,
  TransferSource = 1 << 4,
  TransferDestination = 1 << 5
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
  uint32_t layers = 1;

  /**
   * @brief Number of mip levels
   */
  uint32_t levels = 1;
};

} // namespace liquid::rhi
