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
 * @brief Fixed window
 *
 * Fixed windows are not dockable
 */
class FixedWindow {
public:
  /**
   * @brief Begin fixed windows
   *
   * @param title Window title
   * @retval true Window is expanded
   * @retval false Window is not expanded
   */
  static bool begin(const char *title);

  /**
   * @brief Begin fixed windows with close button
   *
   * @param title Window title
   * @param open Open state
   * @retval true Window is expanded
   * @retval false Window is not expanded
   */
  static bool begin(const char *title, bool &open);

  /**
   * @brief End fixed window
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
   * @retval true Context menu is visible
   * @retval false Context menu is not visible
   */
  static bool begin();

  /**
   * @brief End context menu
   */
  static void end();
};

/**
 * @brief Table widget
 */
class Table {
public:
  /**
   * @brief Begin table
   *
   * @param id Table id
   * @param numColumns Number of columns
   * @retval true Table is visible
   * @retval false Table is not visible
   */
  static bool begin(const char *id, uint32_t numColumns);

  /**
   * @brief End table
   */
  static void end();

  /**
   * @brief Render vector4 column
   *
   * @param value Vector4D value
   */
  static void column(const glm::vec4 &value);

  /**
   * @brief Render vector3 column
   *
   * @param value Vector3 value
   */
  static void column(const glm::vec3 &value);

  /**
   * @brief Render quaternion column
   *
   * @param value Quaternion value
   */
  static void column(const glm::quat &value);

  /**
   * @brief Render scalar column
   *
   * @param value Scalar value
   */
  static void column(float value);

  /**
   * @brief Render string column
   *
   * @param value String value
   */
  static void column(const liquid::String &value);

  /**
   * @brief Render integer column
   *
   * @param value Integer value
   */
  static void column(int32_t value);

  /**
   * @brief Render unsigned integer column
   *
   * @param value Unsigned integer value
   */
  static void column(uint32_t value);

  /**
   * @brief Render image column
   *
   * @param handle Texture handle
   * @param size Image size
   */
  static void column(liquid::rhi::TextureHandle handle, const glm::vec2 &size);

  /**
   * @brief Render row
   *
   * @tparam ...TColumns Column types
   * @param ...columns Columns
   */
  template <class... TColumns> static void row(const TColumns &...columns) {
    ImGui::TableNextRow();

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    (column(columns), ...);
  }
};

} // namespace liquidator::widgets
