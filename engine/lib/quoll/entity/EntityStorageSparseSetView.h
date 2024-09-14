#pragma once

#include "EntityStorageSparseSetComponentPool.h"

namespace quoll {

template <class... TComponentTypes> class EntityStorageSparseSetView {
  using PickedPools = std::array<EntityStorageSparseSetComponentPool *,
                                 sizeof...(TComponentTypes)>;

  static constexpr usize DeadIndex = std::numeric_limits<usize>::max();

public:
  class Iterator {
  public:
    Iterator(usize index, PickedPools &pools,
             EntityStorageSparseSetComponentPool *smallestPool)
        : mIndex(index), mPools(pools), mSmallestPool(smallestPool) {}

    Iterator &operator++() {
      do {
        mIndex++;
      } while (mIndex < mSmallestPool->entities.size() &&
               !isValidIndex(mIndex, mPools, mSmallestPool));

      return *this;
    }

    bool operator==(Iterator &rhs) { return mIndex == rhs.mIndex; }

    bool operator!=(Iterator &rhs) { return mIndex != rhs.mIndex; }

    std::tuple<Entity, TComponentTypes &...> operator*() {
      return get(std::index_sequence_for<TComponentTypes...>{});
    }

  private:
    template <usize... TComponentIndices>
    std::tuple<Entity, TComponentTypes &...>
    get(std::index_sequence<TComponentIndices...> sequence) {
      auto entity = mSmallestPool->entities.at(mIndex);

      const auto &indices =
          std::array{std::get<TComponentIndices>(mPools)
                         ->entityIndices[static_cast<usize>(entity)]...};

      return {mSmallestPool->entities.at(mIndex),
              std::any_cast<TComponentTypes &>(
                  std::get<TComponentIndices>(mPools)
                      ->components[std::get<TComponentIndices>(indices)])...};
    }

  private:
    usize mIndex = 0;
    PickedPools &mPools;
    EntityStorageSparseSetComponentPool *mSmallestPool = nullptr;
  };

public:
  EntityStorageSparseSetView(PickedPools pools) : mPools(pools) {}

  Iterator begin() {
    mSmallestPool = mPools.at(0);
    for (auto *pool : mPools) {
      if (pool->entities.size() < mSmallestPool->entities.size()) {
        mSmallestPool = pool;
      }
    }

    usize index = 0;
    while (index < mSmallestPool->entities.size() &&
           !isValidIndex(index, mPools, mSmallestPool)) {
      index++;
    }

    return Iterator(index, mPools, mSmallestPool);
  }

  Iterator end() {
    QuollAssert(mSmallestPool != nullptr, "Begin is not called");

    return Iterator(mSmallestPool->entities.size(), mPools, mSmallestPool);
  }

private:
  static bool isValidIndex(usize index, PickedPools &pools,
                           EntityStorageSparseSetComponentPool *smallestPool) {
    bool isValid = true;
    auto entity = static_cast<usize>(smallestPool->entities.at(index));
    for (usize i = 0; i < pools.size() && isValid; ++i) {
      auto *pool = pools.at(i);
      isValid = entity < pool->entityIndices.size() &&
                pool->entityIndices[entity] != DeadIndex;
    }

    return isValid;
  }

private:
  PickedPools mPools;

  EntityStorageSparseSetComponentPool *mSmallestPool = nullptr;
};

} // namespace quoll
