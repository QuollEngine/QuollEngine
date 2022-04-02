#pragma once

namespace liquid {

struct TextureAsset {
  uint32_t width = 0;
  uint32_t height = 0;
  void *data = nullptr;
};

} // namespace liquid
