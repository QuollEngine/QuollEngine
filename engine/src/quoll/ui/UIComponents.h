#pragma once

#include "quoll/asset/Asset.h"
#include "Yoga.h"

namespace quoll {

/**
 * @brief UI image
 */
struct UIImage {
  /**
   * Texture asset
   */
  TextureAssetHandle texture;
};

/**
 * @brief UI text
 */
struct UIText {
  /**
   * Text content
   */
  String content;
};

/**
 * @brief UI view
 */
struct UIView {
  /**
   * Children
   */
  std::vector<std::variant<UIView, UIImage, UIText>> children;

  /**
   * Flex direction
   */
  YGFlexDirection flexDirection{YGFlexDirectionColumn};
};

/**
 * @brief UI component
 */
using UIComponent = decltype(UIView::children)::value_type;

} // namespace quoll
