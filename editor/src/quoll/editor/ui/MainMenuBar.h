#pragma once

namespace quoll::editor {

/**
 * @brief Main menu bar component
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

} // namespace quoll::editor
