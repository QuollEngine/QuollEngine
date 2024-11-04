#pragma once

#include "quoll/qui/Qui.h"

namespace quoll {

namespace debug {

class DebugPanel;

} // namespace debug

class ImguiDebugLayer {
public:
  ImguiDebugLayer(std::vector<debug::DebugPanel *> panels = {});

  void renderMenu();

  void render();

private:
  void renderDemoWindow();

  void renderQui();

private:
  std::vector<debug::DebugPanel *> mPanels;

  bool mDemoWindowOpen = false;
  bool mQuiOpen = false;

  qui::Tree mDemoTree;
};

} // namespace quoll
