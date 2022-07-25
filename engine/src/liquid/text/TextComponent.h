#pragma once

#include "liquid/asset/Asset.h"

namespace liquid {

/**
 * @brief Text component
 */
struct TextComponent {
  /**
   * @brief Text contents
   */
  String text;

  /**
   * @brief Line height
   */
  float lineHeight = 1.0f;

  /**
   * @brief Font used for rendering
   */
  FontAssetHandle font = FontAssetHandle::Invalid;
};

} // namespace liquid
