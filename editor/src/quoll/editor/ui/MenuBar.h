#pragma once

namespace quoll::editor {

class MenuBar {
public:
  MenuBar();

  ~MenuBar();

  MenuBar(const MenuBar &) = delete;
  MenuBar(MenuBar &&) = delete;
  MenuBar &operator=(const MenuBar &) = delete;
  MenuBar &operator=(MenuBar &&) = delete;

  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

} // namespace quoll::editor
