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

enum class ThemeStyle { SectionRounding };

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
   * @brief Get theme style
   *
   * @param style Theme style
   * @return Style value
   */
  static glm::vec2 getStyle(ThemeStyle style);

  /**
   * @brief Get bold font
   *
   * @return Bold font
   */
  static ImFont *getBoldFont();
};

} // namespace liquid::editor
