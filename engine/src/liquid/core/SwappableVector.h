#pragma once

namespace liquid {

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
  class Iterator {
  public:
    Iterator(const SwappableVector<TItem> &container, size_t index)
        : mContainer(container), mIndex(index) {}

    bool operator!=(const Iterator &rhs) const { return mIndex != rhs.mIndex; }

    const TItem &operator*() const { return mContainer.at(mIndex); }

    const Iterator &operator++() {
      mIndex++;
      return *this;
    }

  private:
    const SwappableVector<TItem> &mContainer;
    size_t mIndex;
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
  void erase(size_t index) {
    LIQUID_ASSERT(index < mSize, "Index out of bounds");
    size_t lastItem = mSize - 1;
    mBuffer.at(index) = mBuffer.at(lastItem);
    mSize--;
  }

  /**
   * @brief Get value at index
   *
   * @param index Index
   * @return Item
   */
  inline TItem &at(size_t index) {
    LIQUID_ASSERT(index < mSize, "Index out of bounds");
    return mBuffer.at(index);
  }

  /**
   * @brief Get value at index
   *
   * @param index Index
   * @return Item
   */
  inline const TItem &at(size_t index) const {
    LIQUID_ASSERT(index < mSize, "Index out of bounds");
    return mBuffer.at(index);
  }

  /**
   * @brief Get size of vector
   * @return Vector size
   */
  inline size_t size() const { return mSize; }

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
  size_t mSize = 0;
  std::vector<TItem> mBuffer;
};

} // namespace liquid
