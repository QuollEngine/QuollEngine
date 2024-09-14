#pragma once

namespace quoll {

struct KeyboardEvent {
  i32 key = -1;

  i32 scanCode = -1;

  i32 mods = 0;
};

} // namespace quoll
