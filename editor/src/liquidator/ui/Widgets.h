#pragma once

namespace liquidator::widgets {

/**
 * @brief Section widget
 *
 * Represents a section within a window
 * with title and contents
 *
 * Automatically applies styles based on
 * the theme values
 */
class Section {
public:
  /**
   * @brief Begin section
   *
   * @param title Section title
   * @retval true Section is expanded
   * @retval false Section is not expanded
   */
  static bool begin(const char *title);

  /**
   * End section
   */
  static void end();
};

/**
 * @brief Window widget
 */
class Window {
public:
  /**
   * @brief Begin window
   *
   * @param title Window title
   * @retval true Window is expanded
   * @retval false Window is not expanded
   */
  static bool begin(const char *title);

  /**
   * @brief End window
   */
  static void end();
};

/**
 * @brief Main menu bar width
 */
class MainMenuBar {
public:
  /**
   * @brief Begin main menu bar
   *
   * @retval true Main menu bar is visible
   * @retval false Main menu bar is not visible
   */
  static bool begin();

  /**
   * @brief End main menu bar
   */
  static void end();
};

/**
 * @brief Context menu widget
 *
 * Opens right click context menu
 */
class ContextMenu {
public:
  /**
   * @brief Begin context menu
   *
   * @param Window title
   * @retval true Context menu is visible
   * @retval false Context menu is not visible
   */
  static bool begin();

  /**
   * @brief End context menu
   */
  static void end();
};

} // namespace liquidator::widgets
