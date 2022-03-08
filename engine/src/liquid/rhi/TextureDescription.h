#pragma once

namespace liquid {

enum class TextureType { Standard, Cubemap };

struct TextureDescription {
  TextureType type = TextureType::Standard;
  uint32_t format = 0;
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t depth = 1;
  uint32_t layers = 1;
  uint32_t usageFlags = 0;
  uint32_t aspectFlags = 0;

  size_t size = 0;
  void *data = nullptr;
};

} // namespace liquid
