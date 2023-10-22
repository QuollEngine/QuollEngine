#pragma once

namespace quoll {

/**
 * @brief FPS counter
 */
class FPSCounter {
public:
  /**
   * @brief Collect frames per second
   *
   * @param fps Frames per second
   */
  void collectFPS(u32 fps);

  /**
   * @brief Get frames per second
   *
   * @return Frames per second
   */
  inline u32 getFPS() const { return mFps; }

private:
  u32 mFps = 0;
};

} // namespace quoll
