#pragma once

#include "quoll/asset/AssetHandle.h"
#include "quoll/asset/AssetType.h"
#include "quoll/renderer/TextureAsset.h"
#include "Yoga.h"


namespace quoll {

struct UIImage {
  AssetHandle<TextureAsset> texture;

  String id;
};

struct UIText {
  String content;

  String id;
};

struct UIViewStyle {
  f32 grow = 0.0f;

  f32 shrink = 1.0f;

  YGFlexDirection direction{YGFlexDirectionRow};

  YGJustify justifyContent{YGJustifyFlexStart};

  YGAlign alignItems{YGAlignStretch};

  YGAlign alignContent{YGAlignFlexStart};

  glm::vec4 backgroundColor{0.0f};
};

struct UIView {
  std::vector<std::variant<UIView, UIImage, UIText>> children;

  UIViewStyle style{};

  String id;
};

using UIComponent = decltype(UIView::children)::value_type;

} // namespace quoll
