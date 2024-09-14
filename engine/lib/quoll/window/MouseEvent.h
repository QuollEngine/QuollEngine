#pragma once

namespace quoll {

struct MouseButtonEvent {
  i32 button = -1;

  i32 mods = 0;
};

struct MouseCursorEvent {
  f32 xpos = 0.0f;

  f32 ypos = 0.0f;
};

struct MouseScrollEvent {
  f32 xoffset = 0.0f;

  f32 yoffset = 0.0f;
};

} // namespace quoll
