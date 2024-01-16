#pragma once

namespace quoll {

class FPSCounter {
public:
  void collectFPS(u32 fps);

  inline u32 getFPS() const { return mFps; }

private:
  u32 mFps = 0;
};

} // namespace quoll
