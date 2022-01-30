#pragma once

#include "liquid/core/Base.h"
#include "Entity.h"
#include "EntityError.h"
#include "EntityUtils.h"

namespace liquid {

template <class ComponentType> struct EntityStorageSparseSetComponentPool {
  std::vector<size_t> entityIndices;
  std::vector<Entity> entities;
  std::vector<ComponentType> components;
  size_t size;
};

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
  ~EntityStorageSparseSet();

  /**
   * @brief Create entity
   *
   * @return Newly created entity
   */
  Entity createEntity();

  /**
   * @brief Check if entity exists
   *
   * @param entity Entity
   * @retval true Entity exists
   * @retval false Entity does not exist
   */
  bool hasEntity(Entity entity) const;

  /**
   * @brief Get number of entities
   *
   * @return Number of entities
   */
  inline const size_t getEntityCount() const;

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
  template <class ComponentType>
  void setComponent(Entity entity, const ComponentType &value);

  /**
   * @brief Get component
   *
   * @tparam ComponentType Component type
   * @param entity Entity
   * @return Component value
   * @throws EntityError Component not found
   */
  template <class ComponentType>
  const ComponentType &getComponent(Entity entity) const;

  /**
   * @brief Get component
   *
   * @tparam ComponentType Component type
   * @param entity Entity
   * @return Component value
   * @throws EntityError Component not found
   */
  template <class ComponentType> ComponentType &getComponent(Entity entity);

  /**
   * @brief Check if component exists in entity
   *
   * @tparam ComponentType Component type
   * @param entity Entity
   * @retval true Entity exists
   * @retval false Entity does not exist
   */
  template <class ComponentType> bool hasComponent(Entity entity) const;

  /**
   * @brief Deletes component from entity
   *
   * @tparam ComponentType Component type
   * @param entity Entity
   */
  template <class ComponentType> void deleteComponent(Entity entity);

  /**
   * @brief Count entities with component type
   *
   * @tparam ComponentType Component type
   */
  template <class ComponentType> size_t getEntityCountForComponent() const;

  /**
   * @brief Get all entities with specified components
   *
   * @tparam PickComponents Components to pick
   * @param iterFn Iterator function
   */
  template <class... PickComponents>
  void iterateEntities(
      const typename IterFnType<
          std::function<void(Entity, PickComponents &...)>>::type &iterFn);

  /**
   * @brief Destroys all entities and components
   */
  void destroy();

  /**
   * @brief Destroy all components of a specific type
   *
   * @tparam Component type to destroy
   */
  template <class ComponentType> void destroyComponents();

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
  getPoolForComponent() const;

  /**
   * @brief Get pool for component
   *
   * Retrieves component pool from the tuple in compile-time
   *
   * @tparam ComponentType Component type
   * @return Component pool for component type
   */
  template <class ComponentType>
  EntityStorageSparseSetComponentPool<ComponentType> &getPoolForComponent();

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
      std::index_sequence<PickComponentIndices...> sequence);

  /**
   * @brief Delete all entity components
   *
   * Recursion function. Loops through component pools and deletes entity.
   *
   * @tparam Index Tuple index of component pool
   * @param entity Entity
   */
  template <size_t Index = 0> void deleteAllEntityComponents(Entity entity);

  /**
   * @brief Delete all components
   *
   * @tparam Index Tuple index
   */
  template <size_t Index = 0> void deleteAllComponents();

  /**
   * @brief Delete all entities
   */
  void deleteAllEntities();

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
          &pools);

private:
  std::tuple<EntityStorageSparseSetComponentPool<ComponentTypes>...>
      componentPools;

  Entity lastEntity = 0;
  std::list<Entity> deleted;
  size_t numEntities = 0;
};

#include "EntityStorageSparseSet.impl.h"

} // namespace liquid
