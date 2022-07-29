#pragma once

namespace liquidator {

/**
 * @brief Toolbar widget
 *
 * Actions such as "Play"
 * is stored in this widget
 */
class Toolbar {
public:
  /**
   * @brief Toolbar height
   */
  static constexpr float Height = 60.0f;

  /**
   * @brief Begin toolbar
   *
   * @retval true Toolbar is visible
   * @retval false Toolbar is not visible
   */
  static bool begin();

  /**
   * @brief End toolbar
   */
  static void end();
};

} // namespace liquidator
