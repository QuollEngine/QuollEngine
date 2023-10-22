#pragma once

#include "Filter.h"
#include "WrapMode.h"

namespace quoll::rhi {

/**
 * @brief Sampler description
 */
struct SamplerDescription {
  /**
   * Minification filter
   */
  Filter minFilter = Filter::Linear;

  /**
   * Magnification filter
   */
  Filter magFilter = Filter::Linear;

  /**
   * Wrap mode for U coordinates
   */
  WrapMode wrapModeU = WrapMode::Repeat;

  /**
   * Wrapping mode for V coordinates
   */
  WrapMode wrapModeV = WrapMode::Repeat;

  /**
   * Wrapping mode for W coordinates
   */
  WrapMode wrapModeW = WrapMode::Repeat;

  /**
   * Clamp Lod to minimum value
   */
  f32 minLod = 0.0f;

  /**
   * Maximum clamped Lod value
   *
   * If value is 0.0, maximum value
   * clamping is disabled
   */
  f32 maxLod = 0.0f;

  /**
   * Debug name
   */
  String debugName;
};

} // namespace quoll::rhi
