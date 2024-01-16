#pragma once

#include "Filter.h"
#include "WrapMode.h"

namespace quoll::rhi {

struct SamplerDescription {
  Filter minFilter = Filter::Linear;

  Filter magFilter = Filter::Linear;

  WrapMode wrapModeU = WrapMode::Repeat;

  WrapMode wrapModeV = WrapMode::Repeat;

  WrapMode wrapModeW = WrapMode::Repeat;

  f32 minLod = 0.0f;

  /**
   * Maximum clamped Lod value
   *
   * If value is 0.0, maximum value
   * clamping is disabled
   */
  f32 maxLod = 0.0f;

  String debugName;
};

} // namespace quoll::rhi
