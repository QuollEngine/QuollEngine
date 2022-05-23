#pragma once

namespace liquid::rhi {

enum class TextureType { Standard, Cubemap };

enum class TextureSizeMethod { Fixed, FramebufferRatio };

enum class TextureUsage : uint8_t {
  None = 0,
  Color = 1 << 0,
  Depth = 1 << 1,
  Sampled = 1 << 2,
  TransferDestination = 1 << 3
};

constexpr TextureUsage operator|(TextureUsage a, TextureUsage b) {
  return TextureUsage(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

constexpr TextureUsage operator&(TextureUsage a, TextureUsage b) {
  return TextureUsage(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

/**
 * @brief Texture description
 */
struct TextureDescription {
  /**
   * Texture type
   */
  TextureType type = TextureType::Standard;

  /**
   * Texture size method
   */
  TextureSizeMethod sizeMethod = TextureSizeMethod::Fixed;

  /**
   * Texture usage
   */
  TextureUsage usage = TextureUsage::None;

  /**
   * Texture format
   */
  uint32_t format = 0;

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
   * Texture size
   */
  size_t size = 0;

  /**
   * Texture raw data
   */
  void *data = nullptr;
};

} // namespace liquid::rhi
