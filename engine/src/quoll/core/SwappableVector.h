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
  class Iterator {
  public:
    Iterator(const SwappableVector<TItem> &container, usize index)
        : mContainer(container), mIndex(index) {}

    bool operator!=(const Iterator &rhs) const { return mIndex != rhs.mIndex; }

    const TItem &operator*() const { return mContainer.at(mIndex); }

    const Iterator &operator++() {
      mIndex++;
      return *this;
    }

  private:
    const SwappableVector<TItem> &mContainer;
    usize mIndex;
  };

public:
  void push_back(const TItem &item) {
    if (mBuffer.size() > mSize) {
      mBuffer.at(mSize++) = item;
    } else {
      mBuffer.push_back(item);
      mSize++;
    }
  }

  void erase(usize index) {
    QuollAssert(index < mSize, "Index out of bounds");
    usize lastItem = mSize - 1;
    mBuffer.at(index) = mBuffer.at(lastItem);
    mSize--;
  }

  inline TItem &at(usize index) {
    QuollAssert(index < mSize, "Index out of bounds");
    return mBuffer.at(index);
  }

  inline const TItem &at(usize index) const {
    QuollAssert(index < mSize, "Index out of bounds");
    return mBuffer.at(index);
  }

  inline usize size() const { return mSize; }

  inline bool empty() const { return size() == 0; }

  inline Iterator begin() const { return Iterator(*this, 0); }

  inline Iterator end() const { return Iterator(*this, mSize); }

private:
  usize mSize = 0;
  std::vector<TItem> mBuffer;
};

} // namespace quoll
