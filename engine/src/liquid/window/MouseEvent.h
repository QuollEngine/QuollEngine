#pragma once

namespace liquid {

enum class MouseButtonEvent { Pressed, Released, Moved };

struct MouseButtonEventObject {
  int button = -1;
  int mods = 0;
};

enum class MouseCursorEvent { Moved };

struct MouseCursorEventObject {
  float xpos = 0.0f, ypos = 0.0f;
};

enum class MouseScrollEvent { Scroll };

struct MouseScrollEventObject {
  float xoffset = 0.0f, yoffset = 0.0f;
};

} // namespace liquid
