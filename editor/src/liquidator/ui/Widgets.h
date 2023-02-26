#pragma once

#include "liquid/imgui/ImguiUtils.h"

namespace liquid::editor::widgets {

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
   */
  Section(const char *title);

  /**
   * @brief End section
   */
  ~Section();

  Section(const Section &) = delete;
  Section(Section &&) = delete;
  Section &operator=(const Section &) = delete;
  Section &operator=(Section &&) = delete;

  /**
   * @brief Check if section is expanded
   *
   * @retval true Section is expanded
   * @retval false Section is not expanded
   */
  inline operator bool() const { return mExpanded; }

  /**
   * @brief Get clip rect
   *
   * @return Clip rect
   */
  inline const ImRect &getClipRect() const { return mClipRect; }

  /**
   * @brief Get padding
   *
   * @return Padding
   */
  inline const ImVec2 &getPadding() const { return mPadding; }

private:
  bool mExpanded = false;
  ImRect mClipRect;
  ImVec2 mPadding;
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
   */
  Window(const char *title);

  /**
   * @brief End window
   */
  ~Window();

  Window(const Window &) = delete;
  Window(Window &&) = delete;
  Window &operator=(const Window &) = delete;
  Window &operator=(Window &&) = delete;

  /**
   * @brief Check if window is expanded
   *
   * @retval true Window is expanded
   * @retval false Window is not expanded
   */
  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

/**
 * @brief Fixed window
 *
 * Fixed windows are not dockable
 */
class FixedWindow {
public:
  /**
   * @brief Begin fixed window with close button
   *
   * @param title Window title
   * @param open Open flag
   */
  FixedWindow(const char *title, bool &open);

  /**
   * @brief End fixed window
   */
  ~FixedWindow();

  FixedWindow(const FixedWindow &) = delete;
  FixedWindow(FixedWindow &&) = delete;
  FixedWindow &operator=(const FixedWindow &) = delete;
  FixedWindow &operator=(FixedWindow &&) = delete;

  /**
   * @brief Check if fixed window is expanded
   *
   * @retval true Window is expanded
   * @retval false Window is not expanded
   */
  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

/**
 * @brief Main menu bar width
 */
class MainMenuBar {
public:
  /**
   * @brief Begin main menu bar
   */
  MainMenuBar();

  /**
   * @brief End main menu bar
   */
  ~MainMenuBar();

  MainMenuBar(const MainMenuBar &) = delete;
  MainMenuBar(MainMenuBar &&) = delete;
  MainMenuBar &operator=(const MainMenuBar &) = delete;
  MainMenuBar &operator=(MainMenuBar &&) = delete;

  /**
   * @brief Check if main menu bar is expanded
   *
   * @retval true Main menu bar is expanded
   * @retval false Main menu bar is not expanded
   */
  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

/**
 * @brief Context menu widget
 *
 * Opens right click context menu
 */
class ContextMenu {
public:
  /**
   * @brief Begin main menu bar
   */
  ContextMenu();

  /**
   * @brief End main menu bar
   */
  ~ContextMenu();

  ContextMenu(const ContextMenu &) = delete;
  ContextMenu(ContextMenu &&) = delete;
  ContextMenu &operator=(const ContextMenu &) = delete;
  ContextMenu &operator=(ContextMenu &&) = delete;

  /**
   * @brief Check if main menu bar is expanded
   *
   * @retval true Context menu is expanded
   * @retval false Context menu is not expanded
   */
  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

/**
 * @brief Table widget
 */
class Table {
public:
  /**
   * @brief Begin table
   *
   * @param id Table ID
   * @param numColumns Number of columns
   */
  Table(const char *id, uint32_t numColumns);

  /**
   * @brief End table
   */
  ~Table();

  Table(const Table &) = delete;
  Table(Table &&) = delete;
  Table &operator=(const Table &) = delete;
  Table &operator=(Table &&) = delete;

  /**
   * @brief Check if table is expanded
   *
   * @retval true Table is expanded
   * @retval false Table is not expanded
   */
  inline operator bool() const { return mExpanded; }

  /**
   * @brief Render vector4 column
   *
   * @param value Vector4D value
   */
  void column(const glm::vec4 &value);

  /**
   * @brief Render vector3 column
   *
   * @param value Vector3 value
   */
  void column(const glm::vec3 &value);

  /**
   * @brief Render quaternion column
   *
   * @param value Quaternion value
   */
  void column(const glm::quat &value);

  /**
   * @brief Render scalar column
   *
   * @param value Scalar value
   */
  void column(float value);

  /**
   * @brief Render string column
   *
   * @param value String value
   */
  void column(const String &value);

  /**
   * @brief Render integer column
   *
   * @param value Integer value
   */
  void column(int32_t value);

  /**
   * @brief Render unsigned integer column
   *
   * @param value Unsigned integer value
   */
  void column(uint32_t value);

  /**
   * @brief Render image column
   *
   * @param handle Texture handle
   * @param size Image size
   */
  void column(rhi::TextureHandle handle, const glm::vec2 &size);

  /**
   * @brief Render row
   *
   * @tparam ...TColumns Column types
   * @param ...columns Columns
   */
  template <class... TColumns> void row(const TColumns &...columns) {
    ImGui::TableNextRow();

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    (column(columns), ...);
  }

private:
  bool mExpanded = false;
};

/**
 * @brief Input widget
 */
class Input {
public:
  /**
   * @brief Render vec3 input
   *
   * @param label Input label
   * @param value Input value
   */
  Input(String label, glm::vec3 &value);

  /**
   * @brief Render decimal scalar input
   *
   * @param label Input label
   * @param value Input value
   */
  Input(String label, float &value);

  /**
   * @brief Render uint scalar input
   *
   * @param label Input label
   * @param value Input value
   */
  Input(String label, uint32_t &value);

  /**
   * @brief Render text input
   *
   * @param label Input label
   * @param value Input value
   */
  Input(String label, String &value);

  /**
   * @brief Check if input is changed
   *
   * @retval true Input is changed
   * @retval false Input is not changed
   */
  inline operator bool() const { return mChanged; }

private:
  /**
   * @brief Render scalar input
   *
   * @param label Input label
   * @param data Scalar data
   * @param size Number of data items
   * @param dataType Input data type
   */
  void renderScalarInput(String label, void *data, size_t size,
                         ImGuiDataType dataType);

  /**
   * @brief Render text input
   *
   * @param label Input label
   * @param data String data
   */
  void renderTextInput(String label, String &data);

private:
  bool mChanged = false;
};

/**
 * @brief Color input widget
 */
class InputColor {
public:
  /**
   * @brief Render color input
   *
   * @param label Input label
   * @param value Input value
   */
  InputColor(String label, glm::vec4 &value);

  /**
   * @brief Check if input is changed
   *
   * @retval true Input is changed
   * @retval false Input is not changed
   */
  inline operator bool() const { return mChanged; }

private:
  bool mChanged = false;
};

} // namespace liquid::editor::widgets
