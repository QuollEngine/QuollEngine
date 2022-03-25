#pragma once

namespace liquid {

/**
 * @brief Ring buffer
 *
 * Queue implementation that uses
 * a fixed array instead of individual
 * nodes connected via pointers.
 *
 * Ring buffers must have a fixed size.
 *
 * @tparam TItem Item type
 */
template <class TItem> class RingBuffer {
public:
  /**
   * @brief Create ring buffer
   *
   * @param maxSize Maximum size
   */
  RingBuffer(size_t maxSize) : mBuffer(maxSize) {}

  /**
   * @brief Push item to the end of buffer
   *
   * @param item Item
   */
  void push(const TItem &item) {
    LIQUID_ASSERT(mSize < mBuffer.size(), "Queue is full");
    mBuffer.at(mEnd) = item;
    mEnd = (mEnd + 1) % mBuffer.size();
    mSize++;
  }

  /**
   * @brief Pop item from the front
   */
  void pop() {
    LIQUID_ASSERT(!empty(), "Cannot pop from empty queue");
    mStart = (mStart + 1) % mBuffer.size();
    mSize--;
  }

  /**
   * @brief Get item at the front
   *
   * @return Item
   */
  inline TItem &front() {
    LIQUID_ASSERT(!empty(), "Queue is empty");
    return mBuffer.at(mStart);
  }

  /**
   * @brief Get item at the front
   *
   * @return Item
   */
  inline const TItem &front() const {
    LIQUID_ASSERT(!empty(), "Queue is empty");
    return mBuffer.at(mStart);
  }

  /**
   * @brief Get buffer size
   *
   * @return Buffer size
   */
  inline size_t size() const { return mSize; }

  /**
   * @brief Check if ring buffer is empty
   *
   * @retval true Buffer is empty
   * @retval false Buffer is not empty
   */
  inline bool empty() const { return mSize == 0; }

private:
  size_t mStart = 0;
  size_t mEnd = 0;
  size_t mSize = 0;
  std::vector<TItem> mBuffer;
};

} // namespace liquid
