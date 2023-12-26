#pragma once

#include "Shortcut.h"

namespace quoll::editor {

/**
 * @brief Menu component
 */
class Menu {
public:
  /**
   * @brief Begin menu
   *
   * @param label Menu label
   */
  Menu(String label);

  /**
   * @brief End menu
   */
  ~Menu();

  /**
   * @brief Render menu item
   *
   * @param label Menu item label
   * @param shortcut Shortcut
   *
   * @retval true Item is clicked
   * @retval false Item is not clicked
   */
  bool item(String label, Shortcut shortcut = Shortcut{});

  /**
   * @brief Check if menu is open
   *
   * @retval true Menu is open
   * @retval false Menu is not open
   */
  inline operator bool() { return mOpen; }

  Menu(const Menu &) = delete;
  Menu(Menu &&) = delete;
  Menu &operator=(const Menu &) = delete;
  Menu &operator=(Menu &&) = delete;

private:
  bool mOpen = false;
};

} // namespace quoll::editor
