#pragma once

namespace liquid::rhi {

enum class TextureType { Standard, Cubemap };

enum class TextureSizeMethod { Fixed, SwapchainRatio };

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

struct TextureDescription {
  TextureType type = TextureType::Standard;
  TextureSizeMethod sizeMethod = TextureSizeMethod::Fixed;
  TextureUsage usage = TextureUsage::None;
  uint32_t format = 0;
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t depth = 1;
  uint32_t layers = 1;
  size_t size = 0;
  void *data = nullptr;
};

} // namespace liquid::rhi
