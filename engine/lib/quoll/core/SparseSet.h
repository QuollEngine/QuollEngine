#pragma once

namespace quoll {

/**
 * @brief Sparse set data structure
 *
 * Stores unordered list of items
 * with both fast access and fast
 * iteration.
 *
 * This structure consists of two sets:
 *
 * 1. A dense array that stores the actual
 *    data in a cache friendly way. This
 *    array is automatically shuffled and
 *    reordered on inserts and deletes.
 *    All the range iterators advance
 *    using this array.
 * 2. A sparse array with holes that point
 *    to dense array indices. Any single
 *    item access (getter and delete)
 *    is determined through this array.
 *
 * @tparam TData Data type
 */
template <class TData> class SparseSet {
  static constexpr usize SparseDataSize = 100;

public:
  static constexpr usize Empty = std::numeric_limits<usize>::max();
  using Iterator = typename std::vector<TData>::iterator;

public:
  SparseSet() { mSparseData.resize(SparseDataSize, Empty); }

  usize insert(const TData &item) {
    auto newKey = size();
    if (!mEmptyData.empty()) {
      newKey = mEmptyData.back();
      mEmptyData.pop_back();
    }

    mDenseData.push_back(newKey);
    mRealData.push_back(item);
    mSparseData[newKey] = size() - 1;

    return newKey;
  }

  void erase(usize key) {
    QuollAssert(key < mSparseData.size(),
                "Key is out of bounds: " + std::to_string(key));

    auto lastKey = size() - 1;

    // Copy last item to the deleted item
    mDenseData[mSparseData[key]] = mDenseData[lastKey];
    mRealData[mSparseData[key]] = mRealData[lastKey];

    // Update item's dense index
    // in the sparse array
    mSparseData[mDenseData[lastKey]] = mSparseData[key];

    // Remove last item from dense arrays
    mDenseData.pop_back();
    mRealData.pop_back();

    // Set deleted item key to empty in sparse array
    mSparseData[key] = Empty;

    mEmptyData.push_back(key);
  }

  inline TData &at(usize key) {
    QuollAssert(key < mSparseData.size(), "Index out of bounds");
    usize denseIndex = mSparseData.at(key);

    QuollAssert(denseIndex != Empty, "No data at key");

    return mRealData.at(denseIndex);
  }

  inline const TData &at(usize key) const {
    QuollAssert(key < mSparseData.size(), "Index out of bounds");
    usize denseIndex = mSparseData.at(key);

    QuollAssert(denseIndex != Empty, "No data at key");

    return mRealData.at(denseIndex);
  }

  bool contains(usize key) {
    return key < mSparseData.size() && mSparseData.at(key) != Empty;
  }

  inline bool empty() const { return mDenseData.empty(); }

  inline usize size() const { return mDenseData.size(); }

  inline Iterator begin() { return mRealData.begin(); }

  inline Iterator end() { return mRealData.end(); }

private:
  std::vector<usize> mDenseData;
  std::vector<usize> mSparseData;
  std::vector<TData> mRealData;
  std::vector<usize> mEmptyData;
};

} // namespace quoll
