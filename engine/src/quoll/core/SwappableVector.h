#pragma once

namespace quoll {

/**
 * @brief Swappable vector
 *
 * Dynamic array that does not
 * retain order of items but allows
 * for items to be removed from anywhere
 * in O(1) time.
 *
 * When erasing an item from the middle
 * of the array, the last item in the
 * array will take place of the erased item.
 *
 * @tparam TItem Item type
 */
template <class TItem> class SwappableVector {
public:
  /**
   * @brief Swappable vector iterator
   */
  class Iterator {
  public:
    /**
     * @brief Create iterator
     *
     * @param container Swappaple vector container
     * @param index Index
     */
    Iterator(const SwappableVector<TItem> &container, usize index)
        : mContainer(container), mIndex(index) {}

    /**
     * @brief Check if iterators are not equal
     *
     * @param rhs Other iterator
     * @retval true Iterators are not equal
     * @retval false Iterators are equal
     */
    bool operator!=(const Iterator &rhs) const { return mIndex != rhs.mIndex; }

    /**
     * @brief Get item
     *
     * @return Item
     */
    const TItem &operator*() const { return mContainer.at(mIndex); }

    /**
     * @brief Advance iterator
     *
     * @return This iterator
     */
    const Iterator &operator++() {
      mIndex++;
      return *this;
    }

  private:
    const SwappableVector<TItem> &mContainer;
    usize mIndex;
  };

public:
  /**
   * @brief Push item to the end of the vector
   *
   * @param item Item
   */
  void push_back(const TItem &item) {
    if (mBuffer.size() > mSize) {
      mBuffer.at(mSize++) = item;
    } else {
      mBuffer.push_back(item);
      mSize++;
    }
  }

  /**
   * @brief Erase item at any index
   *
   * @param index Index
   */
  void erase(usize index) {
    QuollAssert(index < mSize, "Index out of bounds");
    usize lastItem = mSize - 1;
    mBuffer.at(index) = mBuffer.at(lastItem);
    mSize--;
  }

  /**
   * @brief Get value at index
   *
   * @param index Index
   * @return Item
   */
  inline TItem &at(usize index) {
    QuollAssert(index < mSize, "Index out of bounds");
    return mBuffer.at(index);
  }

  /**
   * @brief Get value at index
   *
   * @param index Index
   * @return Item
   */
  inline const TItem &at(usize index) const {
    QuollAssert(index < mSize, "Index out of bounds");
    return mBuffer.at(index);
  }

  /**
   * @brief Get size of vector
   * @return Vector size
   */
  inline usize size() const { return mSize; }

  /**
   * @brief Check if vector is empty
   *
   * @retval true Vector is empty
   * @retval false Vector is not empty
   */
  inline bool empty() const { return size() == 0; }

  /**
   * @brief Get iterator that points to first item
   *
   * @return Iterator that points to first item
   */
  inline Iterator begin() const { return Iterator(*this, 0); }

  /**
   * @brief Get iterator that points to end
   *
   * @return Iterator that points to end
   */
  inline Iterator end() const { return Iterator(*this, mSize); }

private:
  usize mSize = 0;
  std::vector<TItem> mBuffer;
};

} // namespace quoll
