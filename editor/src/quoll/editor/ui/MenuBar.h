#pragma once

namespace quoll::editor {

/**
 * @brief Menu bar component
 */
class MenuBar {
public:
  /**
   * @brief Begin menu bar
   */
  MenuBar();

  /**
   * @brief End menu bar
   */
  ~MenuBar();

  MenuBar(const MenuBar &) = delete;
  MenuBar(MenuBar &&) = delete;
  MenuBar &operator=(const MenuBar &) = delete;
  MenuBar &operator=(MenuBar &&) = delete;

  /**
   * @brief Check if menu bar is expanded
   *
   * @retval true Menu bar is expanded
   * @retval false Menu bar is not expanded
   */
  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

} // namespace quoll::editor
