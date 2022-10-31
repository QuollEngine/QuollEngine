#pragma once

namespace liquid::rhi {

enum Format {
  Undefined = 0,
  Rgba8Unorm,
  Rgba8Srgb,
  Bgra8Srgb,
  Rgba16Float,
  Rg32Float,
  Rgb32Float,
  Rgba32Float,
  Rgba32Uint,
  Depth16Unorm,
  Depth32Float
};

} // namespace liquid::rhi
