#pragma once

namespace liquidator {

enum class ThemeColor { BackgroundColor, SceneBackgroundColor };

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
  static glm::vec4 getColor(ThemeColor color);

  /**
   * @brief Get theme style
   *
   * @param style Theme style
   * @return Style value
   */
  static glm::vec2 getStyle(ThemeStyle style);
};

} // namespace liquidator
