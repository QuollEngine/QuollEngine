#pragma once

namespace liquid::rhi {

enum class ImageLayout {
  Undefined = 0,
  General = 1,
  ColorAttachmentOptimal = 2,
  DepthStencilAttachmentOptimal = 3,
  DepthStencilReadOnlyOptimal = 4,
  ShaderReadOnlyOptimal = 5,
  TransferSourceOptimal = 6,
  TransferDestinationOptimal = 7,
  Preinitialized = 8,
  PresentSource = 1000001002
};

} // namespace liquid::rhi
