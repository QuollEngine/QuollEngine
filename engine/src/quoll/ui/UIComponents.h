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

  /**
   * ID
   */
  String id;
};

/**
 * @brief UI text
 */
struct UIText {
  /**
   * Text content
   */
  String content;

  /**
   * ID
   */
  String id;
};

/**
 * @brief UI view style
 */
struct UIViewStyle {
  /**
   * Flex grow
   */
  f32 grow = 0.0f;

  /**
   * Flex shrink
   */
  f32 shrink = 1.0f;

  /**
   * Flex direction
   */
  YGFlexDirection direction{YGFlexDirectionRow};

  /**
   * Justify content
   */
  YGJustify justifyContent{YGJustifyFlexStart};

  /**
   * Align items
   */
  YGAlign alignItems{YGAlignStretch};

  /**
   * Align items
   */
  YGAlign alignContent{YGAlignFlexStart};

  /**
   * Background color
   */
  glm::vec4 backgroundColor{0.0f};
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
   * Style
   */
  UIViewStyle style{};

  /**
   * ID
   */
  String id;
};

/**
 * @brief UI component
 */
using UIComponent = decltype(UIView::children)::value_type;

} // namespace quoll
