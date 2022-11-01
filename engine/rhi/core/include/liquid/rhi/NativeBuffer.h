#pragma once

namespace liquid::rhi {

/**
 * @brief Native device interface
 */
class NativeBuffer {
public:
  /**
   * @brief Default destructor
   */
  virtual ~NativeBuffer() = default;

  /**
   * @brief Map buffer
   *
   * @return Mapped data
   */
  virtual void *map() = 0;

  /**
   * @brief Unmap buffer
   */
  virtual void unmap() = 0;

  /**
   * @brief Resize buffer
   *
   * Recreates the buffer with
   * new size. Does not retain
   * the previous data in it
   *
   * @param size New size
   */
  virtual void resize(size_t size) = 0;
};

} // namespace liquid::rhi
