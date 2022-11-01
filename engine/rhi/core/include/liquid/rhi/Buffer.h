#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid::rhi {

class NativeBuffer;

/**
 * @brief Hardware buffer
 */
class Buffer {
public:
  /**
   * @brief Default constructor
   */
  Buffer() = default;

  /**
   * @brief Create buffer
   *
   * @param handle Buffer handle
   * @param nativeBuffer Native buffer data
   */
  Buffer(BufferHandle handle, NativeBuffer *nativeBuffer);

  /**
   * @brief Map buffer
   *
   * @return Mapped data
   */
  void *map();

  /**
   * @brief Unmap buffer
   */
  void unmap();

  /**
   * @brief Update buffer
   *
   * Maps the buffer, copies data into it,
   * and unmaps it
   *
   * @param data New data
   * @param size Size to copy
   */
  void update(const void *data, size_t size);

  /**
   * @brief Resize buffer
   *
   * Recreates the buffer with
   * new size. Does not retain
   * the previous data in it
   *
   * @param size New size
   */
  void resize(size_t size);

  /**
   * @brief Get buffer handle
   *
   * @return Buffer handle
   */
  inline BufferHandle getHandle() const { return mHandle; }

private:
  BufferHandle mHandle = BufferHandle::Invalid;
  NativeBuffer *mNativeBuffer = nullptr;
};

} // namespace liquid::rhi
