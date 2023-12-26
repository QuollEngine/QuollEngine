#pragma once

namespace quoll::editor {

/**
 * @brief Toolbar
 */
class Toolbar {
public:
  /**
   * @brief Toolbar height
   */
  static constexpr f32 Height = 60.0f;

public:
  /**
   * @brief Begin toolbar
   */
  Toolbar();

  /**
   * @brief End toolbar
   */
  ~Toolbar();

  /**
   * @brief Render toolbar item
   *
   * @param label Toolbar item label
   * @param icon Toolbar item icon
   * @param active Item is active
   *
   * @retval true Item is clicked
   * @retval false Item is not clicked
   */
  bool item(String label, String icon, bool active);

  /**
   * @brief Check if toolbar is open
   *
   * @retval true Toolbar is open
   * @retval false Toolbar is not open
   */
  inline operator bool() { return mOpen; }

  Toolbar(const Toolbar &) = delete;
  Toolbar(Toolbar &&) = delete;
  Toolbar &operator=(const Toolbar &) = delete;
  Toolbar &operator=(Toolbar &&) = delete;

private:
  bool mOpen = false;
};

} // namespace quoll::editor
