#pragma once

namespace quoll::editor {

class MenuBar : NoCopyMove {
public:
  MenuBar();

  ~MenuBar();

  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

} // namespace quoll::editor
