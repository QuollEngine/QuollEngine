#pragma once

namespace quoll::editor {

class MainMenuBar : NoCopyMove {
public:
  MainMenuBar();

  ~MainMenuBar();

  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

} // namespace quoll::editor
