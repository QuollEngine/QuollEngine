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
  size_t size;
};

/**
 * @brief Sparse set based entity storage
 *
 * @tparam ComponentTypes Component types
 */
template <class... ComponentTypes> class EntityStorageSparseSet {
  static_assert(entity_utils::are_types_unique<ComponentTypes...>,
                "All types must be unique");

  static constexpr size_t DEAD_INDEX = std::numeric_limits<size_t>::max();
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
  Entity createEntity() {
    mNumEntities++;
    if (mDeleted.size() > 0) {
      auto eid = mDeleted.front();
      mDeleted.pop_front();
      return eid;
    } else {
      auto eid = mLastEntity++;
      return eid;
    }
  }

  /**
   * @brief Check if entity exists
   *
   * @param entity Entity
   * @retval true Entity exists
   * @retval false Entity does not exist
   */
  bool hasEntity(Entity entity) const {
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
  template <class ComponentType>
  void setComponent(Entity entity, const ComponentType &value) {
    LIQUID_ASSERT(hasEntity(entity),
                  "Entity " + std::to_string(entity) + " does not exist");

    auto &pool = getPoolForComponent<ComponentType>();

    if (entity >= pool.entityIndices.size()) {
      // TODO: Make this better
      pool.entityIndices.resize((entity + 1) * 2, DEAD_INDEX);
    }

    size_t index = pool.entityIndices[entity];
    if (index != DEAD_INDEX) {
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
  template <class ComponentType>
  const ComponentType &getComponent(Entity entity) const {
    LIQUID_ASSERT(hasComponent<ComponentType>(entity),
                  "Component named " + String(typeid(ComponentType).name()) +
                      " does not exist for entity " + std::to_string(entity));
    const auto &pool = getPoolForComponent<ComponentType>();

    return pool.components[pool.entityIndices[entity]];
  }

  /**
   * @brief Get component
   *
   * @tparam ComponentType Component type
   * @param entity Entity
   * @return Component value
   */
  template <class ComponentType> ComponentType &getComponent(Entity entity) {
    LIQUID_ASSERT(hasComponent<ComponentType>(entity),
                  "Component named " + String(typeid(ComponentType).name()) +
                      " does not exist for entity " + std::to_string(entity));
    auto &pool = getPoolForComponent<ComponentType>();

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
  template <class ComponentType> bool hasComponent(Entity entity) const {
    const auto &pool = getPoolForComponent<ComponentType>();
    return entity < pool.entityIndices.size() &&
           pool.entityIndices[entity] != DEAD_INDEX;
  }

  /**
   * @brief Deletes component from entity
   *
   * @tparam ComponentType Component type
   * @param entity Entity
   */
  template <class ComponentType> void deleteComponent(Entity entity) {
    auto &pool = getPoolForComponent<ComponentType>();
    LIQUID_ASSERT(entity < pool.entityIndices.size(),
                  "Component named " + String(typeid(ComponentType).name()) +
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

    pool.entityIndices[entity] = DEAD_INDEX;
  }

  /**
   * @brief Count entities with component type
   *
   * @tparam ComponentType Component type
   * @return Number of entities
   */
  template <class ComponentType> size_t getEntityCountForComponent() const {
    return getPoolForComponent<ComponentType>().entities.size();
  }

  /**
   * @brief Get all entities with specified components
   *
   * @tparam PickComponents Components to pick
   * @param iterFn Iterator function
   */
  template <class... PickComponents>
  void iterateEntities(
      const typename IterFnType<
          std::function<void(Entity, PickComponents &...)>>::type &iterFn) {
    iterateEntitiesInternal(iterFn,
                            std::index_sequence_for<PickComponents...>{});
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
   * @tparam Component type to destroy
   */
  template <class ComponentType> void destroyComponents() {
    getPoolForComponent<ComponentType>().components.clear();
    getPoolForComponent<ComponentType>().entities.clear();
    getPoolForComponent<ComponentType>().entityIndices.clear();
  }

private:
  /**
   * @brief Get constant pool for component
   *
   * Retrieves component pool from the tuple in compile-time
   *
   * @tparam ComponentType Component type
   * @return Component pool for component type
   */
  template <class ComponentType>
  const EntityStorageSparseSetComponentPool<ComponentType> &
  getPoolForComponent() const {
    return std::get<EntityStorageSparseSetComponentPool<ComponentType>>(
        mComponentPools);
  }

  /**
   * @brief Get pool for component
   *
   * Retrieves component pool from the tuple in compile-time
   *
   * @tparam ComponentType Component type
   * @return Component pool for component type
   */
  template <class ComponentType>
  EntityStorageSparseSetComponentPool<ComponentType> &getPoolForComponent() {
    return std::get<EntityStorageSparseSetComponentPool<ComponentType>>(
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
  template <class... PickComponents, size_t... PickComponentIndices>
  void iterateEntitiesInternal(
      const std::function<void(Entity, PickComponents &...)> &iterFn,
      std::index_sequence<PickComponentIndices...> sequence) {
    std::tuple<EntityStorageSparseSetComponentPool<PickComponents> &...>
        pickedPools = {
            std::get<EntityStorageSparseSetComponentPool<PickComponents>>(
                mComponentPools)...};

    const auto &smallestEntities = getSmallestEntityListFromPools(pickedPools);

    for (size_t i = 0; i < smallestEntities.size(); ++i) {
      Entity entity = smallestEntities[i];
      bool isDead = std::apply(
          [entity](auto &&...args) {
            return ((entity >= args.entityIndices.size() ||
                     args.entityIndices[entity] == DEAD_INDEX) ||
                    ...);
          },
          pickedPools);

      if (isDead) {
        continue;
      }

      const auto &indices = std::array{
          std::get<PickComponentIndices>(pickedPools).entityIndices[entity]...};
      std::tuple<PickComponents &...> components = {
          std::get<PickComponentIndices>(pickedPools)
              .components[std::get<PickComponentIndices>(indices)]...};

      iterFn(entity, std::get<PickComponentIndices>(components)...);
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
  template <size_t Index = 0> void deleteAllEntityComponents(Entity entity) {
    auto &pool = std::get<Index>(mComponentPools);
    if (entity < pool.entityIndices.size() &&
        pool.entityIndices[entity] < DEAD_INDEX) {

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

      pool.entityIndices[entity] = DEAD_INDEX;
    }

    if constexpr (Index + 1 != sizeof...(ComponentTypes)) {
      deleteAllEntityComponents<Index + 1>(entity);
    }
  }

  /**
   * @brief Delete all components
   *
   * @tparam Index Tuple index
   */
  template <size_t Index = 0> void deleteAllComponents() {
    std::get<Index>(mComponentPools).components.clear();
    std::get<Index>(mComponentPools).entityIndices.clear();
    std::get<Index>(mComponentPools).entities.clear();

    if constexpr (Index + 1 != sizeof...(ComponentTypes)) {
      deleteAllComponents<Index + 1>();
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
   * @tparam Index Current index
   * @tparam IndexForSmallest Index for smallest pool
   * @tparam PickComponents Component types to pick
   * @param pools Pools to search from
   * @return Reference to smallest entity list
   */
  template <size_t Index = 0, size_t IndexForSmallest = 0,
            class... PickComponents>
  static inline const std::vector<Entity> &getSmallestEntityListFromPools(
      const std::tuple<EntityStorageSparseSetComponentPool<PickComponents> &...>
          &pools) {
    if constexpr (Index < sizeof...(PickComponents)) {
      if (std::get<Index>(pools).entities.size() <
          std::get<IndexForSmallest>(pools).entities.size()) {
        return getSmallestEntityListFromPools<Index + 1, Index,
                                              PickComponents...>(pools);
      } else {
        return getSmallestEntityListFromPools<Index + 1, IndexForSmallest,
                                              PickComponents...>(pools);
      }
    }

    return std::get<IndexForSmallest>(pools).entities;
  }

private:
  std::tuple<EntityStorageSparseSetComponentPool<ComponentTypes>...>
      mComponentPools;

  Entity mLastEntity = 1;
  std::list<Entity> mDeleted;
  size_t mNumEntities = 0;
};

} // namespace liquid
