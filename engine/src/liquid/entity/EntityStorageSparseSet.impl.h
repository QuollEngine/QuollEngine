template <class... ComponentTypes>
EntityStorageSparseSet<ComponentTypes...>::~EntityStorageSparseSet() {
  destroy();
}

template <class... ComponentTypes>
Entity EntityStorageSparseSet<ComponentTypes...>::createEntity() {
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

template <class... ComponentTypes>
inline const size_t
EntityStorageSparseSet<ComponentTypes...>::getEntityCount() const {
  return mNumEntities;
}

template <class... ComponentTypes>
bool EntityStorageSparseSet<ComponentTypes...>::hasEntity(Entity entity) const {
  for (auto &deletedEntity : mDeleted) {
    if (deletedEntity == entity)
      return false;
  }

  return entity < mLastEntity;
}

template <class... ComponentTypes>
void EntityStorageSparseSet<ComponentTypes...>::deleteEntity(Entity entity) {
  if (entity >= ENTITY_MAX)
    return;

  deleteAllEntityComponents(entity);
  mDeleted.push_back(entity);
  this->mNumEntities--;
}

template <class... ComponentTypes>
template <class ComponentType>
void EntityStorageSparseSet<ComponentTypes...>::setComponent(
    Entity entity, const ComponentType &value) {
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

template <class... ComponentTypes>
template <class ComponentType>
const ComponentType &
EntityStorageSparseSet<ComponentTypes...>::getComponent(Entity entity) const {
  LIQUID_ASSERT(hasComponent<ComponentType>(entity),
                "Component named " + String(typeid(ComponentType).name()) +
                    " does not exist for entity " + std::to_string(entity));
  const auto &pool = getPoolForComponent<ComponentType>();

  return pool.components[pool.entityIndices[entity]];
}

template <class... ComponentTypes>
template <class ComponentType>
ComponentType &
EntityStorageSparseSet<ComponentTypes...>::getComponent(Entity entity) {
  LIQUID_ASSERT(hasComponent<ComponentType>(entity),
                "Component named " + String(typeid(ComponentType).name()) +
                    " does not exist for entity " + std::to_string(entity));
  auto &pool = getPoolForComponent<ComponentType>();

  return pool.components[pool.entityIndices[entity]];
}

template <class... ComponentTypes>
template <class ComponentType>
bool EntityStorageSparseSet<ComponentTypes...>::hasComponent(
    Entity entity) const {
  const auto &pool = getPoolForComponent<ComponentType>();
  return entity < pool.entityIndices.size() &&
         pool.entityIndices[entity] != DEAD_INDEX;
}

template <class... ComponentTypes>
template <class ComponentType>
void EntityStorageSparseSet<ComponentTypes...>::deleteComponent(Entity entity) {
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

template <class... ComponentTypes>
template <class ComponentType>
size_t
EntityStorageSparseSet<ComponentTypes...>::getEntityCountForComponent() const {
  return getPoolForComponent<ComponentType>().entities.size();
}

template <class... ComponentTypes>
void EntityStorageSparseSet<ComponentTypes...>::destroy() {
  deleteAllComponents();
  deleteAllEntities();
}

template <class... ComponentTypes>
template <class ComponentType>
void EntityStorageSparseSet<ComponentTypes...>::destroyComponents() {
  getPoolForComponent<ComponentType>().components.clear();
  getPoolForComponent<ComponentType>().entities.clear();
  getPoolForComponent<ComponentType>().entityIndices.clear();
}

template <class... ComponentTypes>
template <size_t Index, size_t IndexForSmallest, class... PickComponents>
inline const std::vector<Entity> &
EntityStorageSparseSet<ComponentTypes...>::getSmallestEntityListFromPools(
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

template <class... ComponentTypes>
template <class... PickComponents, size_t... PickComponentIndices>
void EntityStorageSparseSet<ComponentTypes...>::iterateEntitiesInternal(
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

template <class... ComponentTypes>
template <class... PickComponents>
void EntityStorageSparseSet<ComponentTypes...>::iterateEntities(
    const typename EntityStorageSparseSet::template IterFnType<
        std::function<void(Entity, PickComponents &...)>>::type &iterFn) {
  iterateEntitiesInternal(iterFn, std::index_sequence_for<PickComponents...>{});
}

template <class... ComponentTypes>
template <class ComponentType>
const EntityStorageSparseSetComponentPool<ComponentType> &
EntityStorageSparseSet<ComponentTypes...>::getPoolForComponent() const {
  return std::get<EntityStorageSparseSetComponentPool<ComponentType>>(
      mComponentPools);
}

template <class... ComponentTypes>
template <class ComponentType>
EntityStorageSparseSetComponentPool<ComponentType> &
EntityStorageSparseSet<ComponentTypes...>::getPoolForComponent() {
  return std::get<EntityStorageSparseSetComponentPool<ComponentType>>(
      mComponentPools);
}

template <class... ComponentTypes>
template <size_t Index>
void EntityStorageSparseSet<ComponentTypes...>::deleteAllEntityComponents(
    Entity entity) {
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

template <class... ComponentTypes>
template <size_t Index>
void EntityStorageSparseSet<ComponentTypes...>::deleteAllComponents() {
  std::get<Index>(mComponentPools).components.clear();
  std::get<Index>(mComponentPools).entityIndices.clear();
  std::get<Index>(mComponentPools).entities.clear();

  if constexpr (Index + 1 != sizeof...(ComponentTypes)) {
    deleteAllComponents<Index + 1>();
  }
}

template <class... ComponentTypes>
void EntityStorageSparseSet<ComponentTypes...>::deleteAllEntities() {
  mLastEntity = 0;
  mDeleted.clear();
  mNumEntities = 0;
}
