#pragma once

#include "EntityStorageSparseSetComponentPool.h"

namespace quoll {

template <class TComponent> class EntityStorageSparseSetObserver {
public:
  class Iterator {
  public:
    Iterator(usize index, EntityStorageSparseSetComponentPool *pool)
        : mIndex(index), mPool(pool) {}

    Iterator &operator++() {
      mIndex++;
      return *this;
    }

    bool operator==(Iterator &rhs) { return mIndex == rhs.mIndex; }

    bool operator!=(Iterator &rhs) { return mIndex != rhs.mIndex; }

    std::tuple<Entity, TComponent> operator*() {
      return {mPool->entities.at(mIndex),
              std::any_cast<TComponent>(mPool->components.at(mIndex))};
    }

  private:
    usize mIndex = 0;
    EntityStorageSparseSetComponentPool *mPool;
  };

public:
  EntityStorageSparseSetObserver() = default;

  EntityStorageSparseSetObserver(EntityStorageSparseSetComponentPool *pool)
      : mPool(pool) {}

  Iterator begin() {
    QuollAssert(mPool != nullptr, "Observer is not initialized");

    return Iterator(0, mPool);
  }

  Iterator end() { return Iterator(mPool->entities.size(), mPool); }

  inline usize size() { return mPool->entities.size(); }

  void clear() {
    mPool->entities.clear();
    mPool->components.clear();
  }

private:
  EntityStorageSparseSetComponentPool *mPool = nullptr;
};

} // namespace quoll
