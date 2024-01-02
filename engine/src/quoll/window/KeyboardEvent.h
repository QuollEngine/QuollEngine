#pragma once

namespace quoll {

/**
 * @brief Keyboard event data
 */
struct KeyboardEvent {
  /**
   * Key code
   */
  i32 key = -1;

  /**
   * Key scan code
   */
  i32 scanCode = -1;

  /**
   * Mods
   */
  i32 mods = 0;
};

} // namespace quoll
