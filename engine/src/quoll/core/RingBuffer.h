#pragma once

namespace quoll {

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
  RingBuffer(usize maxSize) : mBuffer(maxSize) {}

  /**
   * @brief Push item to the end of buffer
   *
   * @param item Item
   */
  void push(const TItem &item) {
    QuollAssert(mSize < mBuffer.size(), "Queue is full");
    mBuffer.at(mEnd) = item;
    mEnd = (mEnd + 1) % mBuffer.size();
    mSize++;
  }

  /**
   * @brief Pop item from the front
   */
  void pop() {
    QuollAssert(!empty(), "Cannot pop from empty queue");
    mStart = (mStart + 1) % mBuffer.size();
    mSize--;
  }

  /**
   * @brief Get item at the front
   *
   * @return Item
   */
  inline TItem &front() {
    QuollAssert(!empty(), "Queue is empty");
    return mBuffer.at(mStart);
  }

  /**
   * @brief Get item at the front
   *
   * @return Item
   */
  inline const TItem &front() const {
    QuollAssert(!empty(), "Queue is empty");
    return mBuffer.at(mStart);
  }

  /**
   * @brief Get buffer size
   *
   * @return Buffer size
   */
  inline usize size() const { return mSize; }

  /**
   * @brief Check if ring buffer is empty
   *
   * @retval true Buffer is empty
   * @retval false Buffer is not empty
   */
  inline bool empty() const { return mSize == 0; }

private:
  usize mStart = 0;
  usize mEnd = 0;
  usize mSize = 0;
  std::vector<TItem> mBuffer;
};

} // namespace quoll
