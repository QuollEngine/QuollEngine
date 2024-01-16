#pragma once

#include "Shortcut.h"

namespace quoll::editor {

class Menu {
public:
  Menu(String label);

  ~Menu();

  bool item(String label, Shortcut shortcut = Shortcut{});

  inline operator bool() { return mOpen; }

  Menu(const Menu &) = delete;
  Menu(Menu &&) = delete;
  Menu &operator=(const Menu &) = delete;
  Menu &operator=(Menu &&) = delete;

private:
  bool mOpen = false;
};

} // namespace quoll::editor
