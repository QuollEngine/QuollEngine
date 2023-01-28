#pragma once

namespace liquid::editor {

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

public:
  /**
   * @brief Begin toolbar
   */
  Toolbar();

  /**
   * @brief End toolbar
   */
  ~Toolbar();

  Toolbar(const Toolbar &) = delete;
  Toolbar(Toolbar &&) = delete;
  Toolbar &operator=(const Toolbar &) = delete;
  Toolbar &operator=(Toolbar &&) = delete;

  /**
   * @brief Check if toolbar is expanded
   *
   * @retval true Toolbar is expanded
   * @retval false Toolbar is not expanded
   */
  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

} // namespace liquid::editor
