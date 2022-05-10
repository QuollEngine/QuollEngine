#pragma once

namespace liquid {

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
  void collectFPS(uint32_t fps);

  /**
   * @brief Get frames per second
   *
   * @return Frames per second
   */
  inline uint32_t getFPS() const { return mFps; }

private:
  uint32_t mFps = 0;
};

} // namespace liquid
