#pragma once

namespace liquid {

class StatsManager {
public:
  /**
   * @brief Add draw call
   *
   * @brief primitiveCount Number of primitives
   */
  void addDrawCall(size_t primitiveCount);

  /**
   * @brief Resets draw calls
   */
  void resetDrawCalls();

  /**
   * @brief Get number of draw calls
   *
   * @return Number of draw calls
   */
  inline uint32_t getDrawCallsCount() const { return mDrawCallsCount; }

  /**
   * @brief Get number of drawn primitives
   *
   * @return Number of drawn primitives
   */
  inline size_t getDrawnPrimitivesCount() const {
    return mDrawnPrimitivesCount;
  }

  /**
   * @brief Add texture stats
   *
   * @param size Size to add
   */
  void addTexture(size_t size);

  /**
   * @brief Remove texture stats
   *
   * @param size Size to remove
   */
  void removeTexture(size_t size);

  /**
   * @brief Get allocated textures count
   *
   * @return Number of textures
   */
  inline uint32_t getAllocatedTexturesCount() const {
    return mAllocatedTexturesCount;
  }

  /**
   * @brief Get total allocated texture size
   *
   * @return Total size of textures
   */
  inline size_t getAllocatedTexturesSize() const {
    return mAllocatedTexturesSize;
  }

  /**
   * @brief Add buffer stats
   *
   * @param size Size to add
   */
  void addBuffer(size_t size);

  /**
   * @brief Remove buffer stats
   *
   * @param size Size to remove
   */
  void removeBuffer(size_t size);

  /**
   * @brief Get allocated buffers count
   *
   * @return Number of buffers
   */
  inline uint32_t getAllocatedBuffersCount() const {
    return mAllocatedBuffersCount;
  }

  /**
   * @brief Get total allocated buffer size
   *
   * @return Total size of buffers
   */
  inline size_t getAllocatedBuffersSize() const {
    return mAllocatedBuffersSize;
  }

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
  uint32_t mDrawCallsCount = 0;
  size_t mDrawnPrimitivesCount = 0;

  uint32_t mAllocatedTexturesCount = 0;
  size_t mAllocatedTexturesSize = 0;

  uint32_t mAllocatedBuffersCount = 0;
  size_t mAllocatedBuffersSize = 0;

  uint32_t mFps = 0;
};

} // namespace liquid
