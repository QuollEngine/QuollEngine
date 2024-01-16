#pragma once

namespace quoll::editor {

class MainMenuBar {
public:
  MainMenuBar();

  ~MainMenuBar();

  MainMenuBar(const MainMenuBar &) = delete;
  MainMenuBar(MainMenuBar &&) = delete;
  MainMenuBar &operator=(const MainMenuBar &) = delete;
  MainMenuBar &operator=(MainMenuBar &&) = delete;

  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

} // namespace quoll::editor
