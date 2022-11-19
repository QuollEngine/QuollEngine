#pragma once

#include "Entity.h"
#include "EntityUtils.h"

namespace liquid {

/**
 * @brief Sparse set pool for entity storage
 */
struct EntityStorageSparseSetComponentPool {
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
  std::vector<std::any> components;

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
class EntityStorageSparseSet {
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
  ~EntityStorageSparseSet();

  /**
   * @brief Duplicate contents into new storage
   *
   * @param rhs Other storage
   */
  void duplicate(EntityStorageSparseSet &rhs);

  /**
   * @brief Register component
   *
   * @tparam TComponentType Component type
   */
  template <class TComponentType> void reg() {
    auto id = getComponentId<TComponentType>();

    LIQUID_ASSERT(mComponentPools.find(id) == mComponentPools.end(),
                  "Component pool " + String(typeid(TComponentType).name()) +
                      " already exists");

    mComponentPools.insert({id, {}});
  }

  /**
   * @brief Create entity
   *
   * @return Newly created entity
   */
  Entity create();

  /**
   * @brief Check if entity exists
   *
   * @param entity Entity
   * @retval true Entity exists
   * @retval false Entity does not exist
   */
  bool exists(Entity entity) const;

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
  void deleteEntity(Entity entity);

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

    return std::any_cast<const TComponentType &>(
        pool.components[pool.entityIndices[entity]]);
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

    return std::any_cast<TComponentType &>(
        pool.components[pool.entityIndices[entity]]);
  }

  /**
   * @brief Check if component exists in entity
   *
   * @tparam ComponentType Component type
   * @param entity Entity
   * @retval true Entity has component
   * @retval false Entity does not have component
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
  void destroy();

  /**
   * @brief Destroy all components of a specific type
   *
   * @tparam TComponent type to destroy
   */
  template <class TComponentType> void destroyComponents() {
    auto &pool = getPoolForComponent<TComponentType>();

    pool.components.clear();
    pool.entities.clear();
    pool.entityIndices.clear();
  }

private:
  /**
   * @brief Get component id from type
   *
   * @tparam TComponentType Component type
   * @return Component id
   */
  template <class TComponentType> static std::type_index getComponentId() {
    return std::type_index(typeid(TComponentType));
  }

  /**
   * @brief Get constant pool for component
   *
   * Retrieves component pool from the tuple in compile-time
   *
   * @tparam TComponentType Component type
   * @return Component pool for component type
   */
  template <class TComponentType>
  const EntityStorageSparseSetComponentPool &getPoolForComponent() const {
    auto id = getComponentId<TComponentType>();
    LIQUID_ASSERT(mComponentPools.find(id) != mComponentPools.end(),
                  "Component pool " + String(typeid(TComponentType).name()) +
                      " does not exists");

    return mComponentPools.at(id);
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
  EntityStorageSparseSetComponentPool &getPoolForComponent() {
    auto id = getComponentId<TComponentType>();
    LIQUID_ASSERT(mComponentPools.find(id) != mComponentPools.end(),
                  "Component pool " + String(typeid(TComponentType).name()) +
                      " does not exists");

    return mComponentPools.at(id);
  }

  /**
   * @brief Check if component pool exists
   *
   * @tparam TComponentType Component type
   * @retval true Component type exists
   * @retval false Component type does not exist
   */
  template <class TComponentType> bool hasComponentPool() const {
    return mComponentPools.find(getComponentId<TComponentType>()) !=
           mComponentPools.end();
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

    std::array<EntityStorageSparseSetComponentPool *,
               sizeof...(TPickComponents)>
        pickedPools{&getPoolForComponent<TPickComponents>()...};

    EntityStorageSparseSetComponentPool *smallestPool = pickedPools.at(0);
    for (auto *pool : pickedPools) {
      if (pool->entities.size() < smallestPool->entities.size()) {
        smallestPool = pool;
      }
    }

    const auto &smallestEntities = smallestPool->entities;

    for (size_t i = 0; i < smallestEntities.size(); ++i) {
      Entity entity = smallestEntities[i];

      bool isDead = false;
      for (size_t i = 0; i < pickedPools.size() && !isDead; ++i) {
        auto *pool = pickedPools.at(i);
        isDead = entity >= pool->entityIndices.size() ||
                 pool->entityIndices[entity] == DeadIndex;
      }

      if (isDead) {
        continue;
      }

      const auto &indices =
          std::array{std::get<TPickComponentIndices>(pickedPools)
                         ->entityIndices[entity]...};

      std::tuple<TPickComponents &...> components = {
          std::any_cast<TPickComponents &>(
              std::get<TPickComponentIndices>(pickedPools)
                  ->components[std::get<TPickComponentIndices>(indices)])...};

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
  void deleteAllEntityComponents(Entity entity);

  /**
   * @brief Delete all components
   */
  void deleteAllComponents();

  /**
   * @brief Delete all entities
   */
  void deleteAllEntities();

private:
  std::unordered_map<std::type_index, EntityStorageSparseSetComponentPool>
      mComponentPools;

  Entity mLastEntity = 1;
  std::list<Entity> mDeleted;
  size_t mNumEntities = 0;
};

} // namespace liquid
