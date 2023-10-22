#pragma once

namespace quoll {

enum class MouseButtonEvent { Pressed, Released, Moved };

/**
 * @brief Mouse button event data
 */
struct MouseButtonEventObject {
  /**
   * Button
   */
  int button = -1;

  /**
   * Mods
   */
  int mods = 0;
};

enum class MouseCursorEvent { Moved };

/**
 * @brief Mouse cursor event data
 */
struct MouseCursorEventObject {
  /**
   * Cursor x position
   */
  f32 xpos = 0.0f;

  /**
   * Cursor y position
   */
  f32 ypos = 0.0f;
};

enum class MouseScrollEvent { Scroll };

/**
 * @brief Mouse scroll event data
 */
struct MouseScrollEventObject {
  /**
   * Scroll x offset
   */
  f32 xoffset = 0.0f;

  /**
   * Scroll y offset
   */
  f32 yoffset = 0.0f;
};

} // namespace quoll
