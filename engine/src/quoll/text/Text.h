#pragma once

#include "quoll/asset/Asset.h"

namespace quoll {

/**
 * @brief Text component
 */
struct Text {
  /**
   * Text contents
   */
  String text;

  /**
   * Line height
   */
  f32 lineHeight = 1.0f;

  /**
   * Font used for rendering
   */
  FontAssetHandle font = FontAssetHandle::Null;
};

} // namespace quoll
