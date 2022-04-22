#pragma once

namespace liquid {

enum class KeyboardEvent { Pressed, Released, Repeat };

struct KeyboardEventObject {
  int key = -1;
  int scanCode = -1;
  int mods = 0;
};

} // namespace liquid
