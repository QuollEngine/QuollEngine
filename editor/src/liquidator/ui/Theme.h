#pragma once

namespace liquid::editor {

enum class ThemeColor {
  White,
  Black,
  Transparent,

  // Neutral colors
  Neutral100,
  Neutral200,
  Neutral300,
  Neutral400,
  Neutral500,
  Neutral600,
  Neutral700,
  Neutral800,
  Neutral900,

  // Primary colors
  Primary100,
  Primary200,

  // Misc
  ModalBackdrop
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
   * Child rounding
   */
  float childRounding = 0.0f;
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

} // namespace liquid::editor
