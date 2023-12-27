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

/**
 * @brief Theme styles
 */
struct ThemeStyles {
  /**
   * Item spacing
   */
  ImVec2 itemSpacing;

  /**
   * Section rounding
   */
  ImVec2 sectionRounding;

  /**
   * Window padding
   */
  ImVec2 windowPadding;

  /**
   * Window padding
   */
  ImVec2 framePadding;

  /**
   * Child rounding
   */
  f32 childRounding = 0.0f;
};

/**
 * @brief UI theme
 */
class Theme {
public:
  /**
   * @brief Apply theme
   *
   * Applies theme fonts, colors,
   * and other styles
   */
  static void apply();

  /**
   * @brief Get theme color
   *
   * @param color Theme color
   * @return Color value
   */
  static glm::vec4 getEngineColor(ThemeColor color);

  /**
   * @brief Get clear color
   *
   * @return Color value
   */
  static glm::vec4 getClearColor();

  /**
   * @brief Get theme color
   *
   * @param color Theme color
   * @return Color value
   */
  static ImVec4 getColor(ThemeColor color);

  /**
   * @brief Get bold font
   *
   * @return Bold font
   */
  static ImFont *getBoldFont();

  /**
   * @brief Get default theme styles
   *
   * @return Theme styles
   */
  static const ThemeStyles &getStyles();
};

} // namespace quoll::editor
