#pragma once

namespace quoll {

/**
 * @brief Mouse button event data
 */
struct MouseButtonEvent {
  /**
   * Button
   */
  i32 button = -1;

  /**
   * Mods
   */
  i32 mods = 0;
};

/**
 * @brief Mouse cursor event data
 */
struct MouseCursorEvent {
  /**
   * Cursor x position
   */
  f32 xpos = 0.0f;

  /**
   * Cursor y position
   */
  f32 ypos = 0.0f;
};

/**
 * @brief Mouse scroll event data
 */
struct MouseScrollEvent {
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
