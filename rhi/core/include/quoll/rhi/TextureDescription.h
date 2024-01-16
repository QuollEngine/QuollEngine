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

struct TextureDescription {
  TextureType type = TextureType::Standard;

  TextureUsage usage = TextureUsage::None;

  Format format = Format::Undefined;

  u32 width = 0;

  u32 height = 0;

  u32 depth = 1;

  u32 layerCount = 1;

  u32 mipLevelCount = 1;

  u32 samples = 1;

  String debugName;
};

} // namespace quoll::rhi
