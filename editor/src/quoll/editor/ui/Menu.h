#pragma once

#include "Shortcut.h"

namespace quoll::editor {

class Menu : NoCopyMove {
public:
  Menu(String label);

  ~Menu();

  bool item(String label, Shortcut shortcut = Shortcut{});

  inline operator bool() { return mOpen; }

private:
  bool mOpen = false;
};

} // namespace quoll::editor
