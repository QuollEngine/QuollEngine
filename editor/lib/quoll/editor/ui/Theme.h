#pragma once

#include <imgui.h>

namespace quoll::editor {

enum class ThemeColor {
  // Base
  White,
  Black,
  Transparent,

  // Charcoal
  Charcoal300,
  Charcoal400,
  Charcoal500,
  Charcoal600,
  Charcoal700,
  Charcoal800,

  // Sienna
  Sienna500,
  Sienna600,

  // Misc
  ModalBackdrop,
};

struct ThemeStyles {
  ImVec2 itemSpacing;

  ImVec2 sectionRounding;

  ImVec2 windowPadding;

  ImVec2 framePadding;

  f32 childRounding = 0.0f;
};

class Theme {
public:
  static void apply();

  static glm::vec4 getEngineColor(ThemeColor color);

  static glm::vec4 getClearColor();

  static ImVec4 getColor(ThemeColor color);

  static ImFont *getBoldFont();

  static const ThemeStyles &getStyles();
};

} // namespace quoll::editor
