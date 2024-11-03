#pragma once

namespace quoll::debug {

class DebugPanel : public NoCopyMove {
public:
  virtual ~DebugPanel() = default;

  virtual void onRenderMenu() = 0;

  virtual void onRender() = 0;
};

} // namespace quoll::debug
