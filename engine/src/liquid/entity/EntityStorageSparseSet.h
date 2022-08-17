#pragma once

#include "Entity.h"
#include "EntityUtils.h"

namespace liquid {

/**
 * @brief Sparse set pool for entity storage
 *
 * @tparam TComponentType Component type
 */
template <class TComponentType> struct EntityStorageSparseSetComponentPool {
  /**
   * List of entity indices
   */
  std::vector<size_t> entityIndices;

  /**
   * List of Entities
   */
  std::vector<Entity> entities;

  /**
   * List of components
   */
  std::vector<TComponentType> components;

  /**
   * Pool size
   */
  size_t size = 0;
};

/**
 * @brief Sparse set based entity storage
 *
 * @tparam ComponentTypes Component types
 */
template <class... TComponentTypes> class EntityStorageSparseSet {
  static_assert(entity_utils::AreTypesUnique<TComponentTypes...>,
                "All types must be unique");

  static constexpr size_t DeadIndex = std::numeric_limits<size_t>::max();
  template <typename T> struct IterFnType { typedef T type; };

public:
  EntityStorageSparseSet() = default;
  EntityStorageSparseSet(const EntityStorageSparseSet &) = delete;
  EntityStorageSparseSet(EntityStorageSparseSet &&) = delete;
  EntityStorageSparseSet &operator=(const EntityStorageSparseSet &) = delete;
  EntityStorageSparseSet &operator=(EntityStorageSparseSet &&) = delete;

  /**
   * @brief Destroy storage
   *
   * Destroys all components and entities
   */
  ~EntityStorageSparseSet() { destroy(); }

  /**
   * @brief Duplicate contents into new storage
   *
   * @param rhs Other storage
   */
  void duplicate(EntityStorageSparseSet &rhs) {
    rhs.mComponentPools = mComponentPools;
    rhs.mLastEntity = mLastEntity;
    rhs.mDeleted = mDeleted;
    rhs.mNumEntities = mNumEntities;
  }

  /**
   * @brief Create entity
   *
   * @return Newly created entity
   */
  Entity create() {
    mNumEntities++;
    if (mDeleted.size() > 0) {
      auto eid = mDeleted.front();
      mDeleted.pop_front();
      return eid;
    }

    auto eid = mLastEntity++;
    return eid;
  }

  /**
   * @brief Check if entity exists
   *
   * @param entity Entity
   * @retval true Entity exists
   * @retval false Entity does not exist
   */
  bool exists(Entity entity) const {
    for (auto &deletedEntity : mDeleted) {
      if (deletedEntity == entity)
        return false;
    }

    return entity > EntityNull && entity < mLastEntity;
  }

  /**
   * @brief Get number of entities
   *
   * @return Number of entities
   */
  inline const size_t getEntityCount() const { return mNumEntities; }

  /**
   * @brief Delete entity
   *
   * @param entity Entity
   */
  void deleteEntity(Entity entity) {
    if (entity == EntityNull)
      return;

    deleteAllEntityComponents(entity);
    mDeleted.push_back(entity);
    this->mNumEntities--;
  }

  /**
   * @brief Set component
   *
   * Adds component if component does not
   * exist or updates component if it exists
   * in entity
   *
   * @tparam ComponentType Component type
   * @param entity Entity
   * @param value Component value
   */
  template <class TComponentType>
  void set(Entity entity, const TComponentType &value) {
    LIQUID_ASSERT(exists(entity),
                  "Entity " + std::to_string(entity) + " does not exist");

    auto &pool = getPoolForComponent<TComponentType>();

    if (entity >= pool.entityIndices.size()) {
      // TODO: Make this better
      pool.entityIndices.resize((entity + 1) * 2, DeadIndex);
    }

    size_t index = pool.entityIndices[entity];
    if (index != DeadIndex) {
      pool.components[index] = value;
      pool.entities[index] = entity;
    } else {
      pool.entities.push_back(entity);
      pool.components.push_back(value);
      pool.entityIndices[entity] = pool.entities.size() - 1;
    }
  }

  /**
   * @brief Get component
   *
   * @tparam ComponentType Component type
   * @param entity Entity
   * @return Component value
   */
  template <class TComponentType>
  const TComponentType &get(Entity entity) const {
    LIQUID_ASSERT(has<TComponentType>(entity),
                  "Component named " + String(typeid(TComponentType).name()) +
                      " does not exist for entity " + std::to_string(entity));
    const auto &pool = getPoolForComponent<TComponentType>();

    return pool.components[pool.entityIndices[entity]];
  }

  /**
   * @brief Get component
   *
   * @tparam ComponentType Component type
   * @param entity Entity
   * @return Component value
   */
  template <class TComponentType> TComponentType &get(Entity entity) {
    LIQUID_ASSERT(has<TComponentType>(entity),
                  "Component named " + String(typeid(TComponentType).name()) +
                      " does not exist for entity " + std::to_string(entity));
    auto &pool = getPoolForComponent<TComponentType>();

    return pool.components[pool.entityIndices[entity]];
  }

  /**
   * @brief Check if component exists in entity
   *
   * @tparam ComponentType Component type
   * @param entity Entity
   * @retval true Entity exists
   * @retval false Entity does not exist
   */
  template <class TComponentType> bool has(Entity entity) const {
    const auto &pool = getPoolForComponent<TComponentType>();
    return entity < pool.entityIndices.size() &&
           pool.entityIndices[entity] != DeadIndex;
  }

  /**
   * @brief Remove component from entity
   *
   * @tparam ComponentType Component type
   * @param entity Entity
   */
  template <class TComponentType> void remove(Entity entity) {
    auto &pool = getPoolForComponent<TComponentType>();
    LIQUID_ASSERT(entity < pool.entityIndices.size(),
                  "Component named " + String(typeid(TComponentType).name()) +
                      " does not exist for entity " + std::to_string(entity));

    Entity movedEntity = pool.entities.back();
    size_t entityIndexToDelete = pool.entityIndices[entity];

    // Move last entity in the array to place of deleted entity
    pool.entities[entityIndexToDelete] = movedEntity;

    // Change index of moved entity to the index of deleted entity
    pool.entityIndices[movedEntity] = entityIndexToDelete;

    // Delete last item from entities array
    pool.entities.pop_back();

    // Move last component in the array to place of deleted component
    pool.components[entityIndexToDelete] = pool.components.back();

    // Delete last item from components array
    pool.components.pop_back();

    pool.entityIndices[entity] = DeadIndex;
  }

  /**
   * @brief Count entities with component type
   *
   * @tparam TComponentType Component type
   * @return Number of entities
   */
  template <class TComponentType> size_t getEntityCountForComponent() const {
    return getPoolForComponent<TComponentType>().entities.size();
  }

  /**
   * @brief Get all entities with specified components
   *
   * @tparam TPickComponents Components to pick
   * @param iterFn Iterator function
   */
  template <class... TPickComponents>
  void iterateEntities(
      const typename IterFnType<
          std::function<void(Entity, TPickComponents &...)>>::type &iterFn) {
    iterateEntitiesInternal(iterFn,
                            std::index_sequence_for<TPickComponents...>{});
  }

  /**
   * @brief Destroys all entities and components
   */
  void destroy() {
    deleteAllComponents();
    deleteAllEntities();
  }

  /**
   * @brief Destroy all components of a specific type
   *
   * @tparam TComponent type to destroy
   */
  template <class TComponentType> void destroyComponents() {
    getPoolForComponent<TComponentType>().components.clear();
    getPoolForComponent<TComponentType>().entities.clear();
    getPoolForComponent<TComponentType>().entityIndices.clear();
  }

private:
  /**
   * @brief Get constant pool for component
   *
   * Retrieves component pool from the tuple in compile-time
   *
   * @tparam TComponentType Component type
   * @return Component pool for component type
   */
  template <class TComponentType>
  const EntityStorageSparseSetComponentPool<TComponentType> &
  getPoolForComponent() const {
    return std::get<EntityStorageSparseSetComponentPool<TComponentType>>(
        mComponentPools);
  }

  /**
   * @brief Get pool for component
   *
   * Retrieves component pool from the tuple in compile-time
   *
   * @tparam TComponentType Component type
   * @return Component pool for component type
   */
  template <class TComponentType>
  EntityStorageSparseSetComponentPool<TComponentType> &getPoolForComponent() {
    return std::get<EntityStorageSparseSetComponentPool<TComponentType>>(
        mComponentPools);
  }

  /**
   * @brief Get all entities with specified components
   *
   * Infers template parameter for indices using index sequence
   * and uses the index to get the needed values for components
   *
   * @tparam PickComponents Components to pick
   * @tparam PickComponentIndices Indices of picked components
   * @param iterFn Iterator function
   * @param sequence Index sequence of picked components
   */
  template <class... TPickComponents, size_t... TPickComponentIndices>
  void iterateEntitiesInternal(
      const std::function<void(Entity, TPickComponents &...)> &iterFn,
      std::index_sequence<TPickComponentIndices...> sequence) {
    std::tuple<EntityStorageSparseSetComponentPool<TPickComponents> &...>
        pickedPools = {
            std::get<EntityStorageSparseSetComponentPool<TPickComponents>>(
                mComponentPools)...};

    const auto &smallestEntities = getSmallestEntityListFromPools(pickedPools);

    for (size_t i = 0; i < smallestEntities.size(); ++i) {
      Entity entity = smallestEntities[i];
      bool isDead = std::apply(
          [entity](auto &&...args) {
            return ((entity >= args.entityIndices.size() ||
                     args.entityIndices[entity] == DeadIndex) ||
                    ...);
          },
          pickedPools);

      if (isDead) {
        continue;
      }

      const auto &indices =
          std::array{std::get<TPickComponentIndices>(pickedPools)
                         .entityIndices[entity]...};
      std::tuple<TPickComponents &...> components = {
          std::get<TPickComponentIndices>(pickedPools)
              .components[std::get<TPickComponentIndices>(indices)]...};

      iterFn(entity, std::get<TPickComponentIndices>(components)...);
    }
  }

  /**
   * @brief Delete all entity components
   *
   * Recursion function. Loops through component pools and deletes entity.
   *
   * @tparam Index Tuple index of component pool
   * @param entity Entity
   */
  template <size_t TIndex = 0> void deleteAllEntityComponents(Entity entity) {
    auto &pool = std::get<TIndex>(mComponentPools);
    if (entity < pool.entityIndices.size() &&
        pool.entityIndices[entity] < DeadIndex) {

      Entity movedEntity = pool.entities.back();
      size_t entityIndexToDelete = pool.entityIndices[entity];

      // Move last entity in the array to place of deleted entity
      pool.entities[entityIndexToDelete] = movedEntity;

      // Change index of moved entity to the index of deleted entity
      pool.entityIndices[movedEntity] = entityIndexToDelete;

      // Delete last item from entities array
      pool.entities.pop_back();

      // Move last component in the array to place of deleted component
      pool.components[entityIndexToDelete] = pool.components.back();

      // Delete last item from components array
      pool.components.pop_back();

      pool.entityIndices[entity] = DeadIndex;
    }

    if constexpr (TIndex + 1 != sizeof...(TComponentTypes)) {
      deleteAllEntityComponents<TIndex + 1>(entity);
    }
  }

  /**
   * @brief Delete all components
   *
   * @tparam Index Tuple index
   */
  template <size_t TIndex = 0> void deleteAllComponents() {
    std::get<TIndex>(mComponentPools).components.clear();
    std::get<TIndex>(mComponentPools).entityIndices.clear();
    std::get<TIndex>(mComponentPools).entities.clear();

    if constexpr (TIndex + 1 != sizeof...(TComponentTypes)) {
      deleteAllComponents<TIndex + 1>();
    }
  }

  /**
   * @brief Delete all entities
   */
  void deleteAllEntities() {
    mLastEntity = 1;
    mDeleted.clear();
    mNumEntities = 0;
  }

private:
  /**
   * @brief Recursively find the smallest entity list
   *
   * @tparam TIndex Current index
   * @tparam TIndexForSmallest Index for smallest pool
   * @tparam TPickComponents Component types to pick
   * @param pools Pools to search from
   * @return Reference to smallest entity list
   */
  template <size_t TIndex = 0, size_t TIndexForSmallest = 0,
            class... TPickComponents>
  static inline const std::vector<Entity> &getSmallestEntityListFromPools(
      const std::tuple<
          EntityStorageSparseSetComponentPool<TPickComponents> &...> &pools) {
    if constexpr (TIndex < sizeof...(TPickComponents)) {
      if (std::get<TIndex>(pools).entities.size() <
          std::get<TIndexForSmallest>(pools).entities.size()) {
        return getSmallestEntityListFromPools<TIndex + 1, TIndex,
                                              TPickComponents...>(pools);
      }
      return getSmallestEntityListFromPools<TIndex + 1, TIndexForSmallest,
                                            TPickComponents...>(pools);
    }

    return std::get<TIndexForSmallest>(pools).entities;
  }

private:
  std::tuple<EntityStorageSparseSetComponentPool<TComponentTypes>...>
      mComponentPools;

  Entity mLastEntity = 1;
  std::list<Entity> mDeleted;
  size_t mNumEntities = 0;
};

} // namespace liquid
