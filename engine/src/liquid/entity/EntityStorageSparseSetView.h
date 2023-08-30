#pragma once

#include "EntityStorageSparseSetComponentPool.h"

namespace quoll {

/**
 * @brief View for sparse set based entity storage
 *
 * @tparam ...TComponentTypes Component types
 */
template <class... TComponentTypes> class EntityStorageSparseSetView {
  using PickedPools = std::array<EntityStorageSparseSetComponentPool *,
                                 sizeof...(TComponentTypes)>;

  static constexpr size_t DeadIndex = std::numeric_limits<size_t>::max();

public:
  /**
   * @brief View iterator
   */
  class Iterator {
  public:
    /**
     * @brief Create iterator
     *
     * @param index Index
     * @param pools Picked pools
     * @param smallestPool Smallest pool
     */
    Iterator(size_t index, PickedPools &pools,
             EntityStorageSparseSetComponentPool *smallestPool)
        : mIndex(index), mPools(pools), mSmallestPool(smallestPool) {}

    /**
     * @brief Increment iterator
     *
     * Increment over invalid indices
     *
     * @return This iterator
     */
    Iterator &operator++() {
      do {
        mIndex++;
      } while (mIndex < mSmallestPool->entities.size() &&
               !isValidIndex(mIndex, mPools, mSmallestPool));

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
    std::tuple<Entity, TComponentTypes &...> operator*() {
      return get(std::index_sequence_for<TComponentTypes...>{});
    }

  private:
    /**
     * @brief Get value
     *
     * @tparam ...TComponentIndices Component indices
     * @param sequence Index sequence
     * @return Tuple with first item as entity and rest as components
     */
    template <size_t... TComponentIndices>
    std::tuple<Entity, TComponentTypes &...>
    get(std::index_sequence<TComponentIndices...> sequence) {
      auto entity = mSmallestPool->entities.at(mIndex);

      const auto &indices =
          std::array{std::get<TComponentIndices>(mPools)
                         ->entityIndices[static_cast<size_t>(entity)]...};

      return {mSmallestPool->entities.at(mIndex),
              std::any_cast<TComponentTypes &>(
                  std::get<TComponentIndices>(mPools)
                      ->components[std::get<TComponentIndices>(indices)])...};
    }

  private:
    size_t mIndex = 0;
    PickedPools &mPools;
    EntityStorageSparseSetComponentPool *mSmallestPool = nullptr;
  };

public:
  /**
   * @brief Create view for sparse set entity storage
   *
   * @param pools Picked pools
   */
  EntityStorageSparseSetView(PickedPools pools) : mPools(pools) {}

  /**
   * @brief Get begin iterator
   *
   * Get smallest pool and find first valid
   * index
   *
   * @return Begin iterator
   */
  Iterator begin() {
    mSmallestPool = mPools.at(0);
    for (auto *pool : mPools) {
      if (pool->entities.size() < mSmallestPool->entities.size()) {
        mSmallestPool = pool;
      }
    }

    size_t index = 0;
    while (index < mSmallestPool->entities.size() &&
           !isValidIndex(index, mPools, mSmallestPool)) {
      index++;
    }

    return Iterator(index, mPools, mSmallestPool);
  }

  /**
   * @brief Get end iterator
   *
   * Use smallest pool size as iterator end
   *
   * @return End iterator
   */
  Iterator end() {
    LIQUID_ASSERT(mSmallestPool != nullptr, "Begin is not called");

    return Iterator(mSmallestPool->entities.size(), mPools, mSmallestPool);
  }

private:
  /**
   * @brief Check if index is valid
   *
   * @param index Index
   * @param pools Component pools
   * @param smallestPool Smallest pools
   * @retval true Index is valid
   * @retval false Index is not valid
   */
  static bool isValidIndex(size_t index, PickedPools &pools,
                           EntityStorageSparseSetComponentPool *smallestPool) {
    bool isValid = true;
    auto entity = static_cast<size_t>(smallestPool->entities.at(index));
    for (size_t i = 0; i < pools.size() && isValid; ++i) {
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
