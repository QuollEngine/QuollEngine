#pragma once

#include "EntityStorageSparseSetComponentPool.h"

namespace liquid {

/**
 * @brief Observer for sparse set based entity storage
 *
 * @tparam TComponentTypes Component types
 */
template <class TComponent> class EntityStorageSparseSetObserver {
public:
  /**
   * @brief Observer iterator
   */
  class Iterator {
  public:
    /**
     * @brief Create iterator
     *
     * @param index Index
     * @param pool Picked pools
     */
    Iterator(size_t index, EntityStorageSparseSetComponentPool *pool)
        : mIndex(index), mPool(pool) {}

    /**
     * @brief Increment iterator
     *
     * @return This iterator
     */
    Iterator &operator++() {
      mIndex++;
      return *this;
    }

    /**
     * @brief Check if iterators are equal
     *
     * @param rhs Other iterator
     * @retval true Iterators are equal
     * @retval false Iterators are not equal
     */
    bool operator==(Iterator &rhs) { return mIndex == rhs.mIndex; }

    /**
     * @brief Check if iterators are not equal
     *
     * @param rhs Other iterator
     * @retval true Iterators are not equal
     * @retval false Iterators are equal
     */
    bool operator!=(Iterator &rhs) { return mIndex != rhs.mIndex; }

    /**
     * @brief Get value
     *
     * @return Tuple with first item as entity and rest as components
     */
    std::tuple<Entity, TComponent> operator*() {
      return {mPool->entities.at(mIndex),
              std::any_cast<TComponent>(mPool->components.at(mIndex))};
    }

  private:
    size_t mIndex = 0;
    EntityStorageSparseSetComponentPool *mPool;
  };

public:
  /**
   * @brief Create observer
   */
  EntityStorageSparseSetObserver() = default;

  /**
   * @brief Create observer
   *
   * @param pool Component pool
   */
  EntityStorageSparseSetObserver(EntityStorageSparseSetComponentPool *pool)
      : mPool(pool) {}

  /**
   * @brief Get begin iterator
   *
   * @return Begin iterator
   */
  Iterator begin() {
    LIQUID_ASSERT(mPool != nullptr, "Observer is not initialized");

    return Iterator(0, mPool);
  }

  /**
   * @brief Get end iterator
   *
   * @return End iterator
   */
  Iterator end() { return Iterator(mPool->entities.size(), mPool); }

  /**
   * @brief Get number of observed components
   *
   * @return Number of observed components
   */
  inline size_t size() { return mPool->entities.size(); }

  /**
   * @brief Clear observed items
   */
  void clear() {
    mPool->entities.clear();
    mPool->components.clear();
  }

private:
  EntityStorageSparseSetComponentPool *mPool = nullptr;
};

} // namespace liquid
