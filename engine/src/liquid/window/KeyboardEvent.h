#pragma once

namespace quoll {

enum class KeyboardEvent { Pressed, Released, Repeat };

/**
 * @brief Keyboard event data
 */
struct KeyboardEventObject {
  /**
   * Key code
   */
  int key = -1;

  /**
   * Key scan code
   */
  int scanCode = -1;

  /**
   * Mods
   */
  int mods = 0;
};

} // namespace quoll
