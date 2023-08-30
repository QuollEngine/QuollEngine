#include "liquid/core/Base.h"
#include "EntityStorageSparseSet.h"

namespace quoll {

EntityStorageSparseSet::~EntityStorageSparseSet() { destroy(); }

void EntityStorageSparseSet::duplicate(EntityStorageSparseSet &rhs) {
  rhs.mComponentPools = mComponentPools;
  rhs.mLastEntity = mLastEntity;
  rhs.mDeleted = mDeleted;
  rhs.mNumEntities = mNumEntities;
}

Entity EntityStorageSparseSet::create() {
  mNumEntities++;
  if (mDeleted.size() > 0) {
    auto eid = mDeleted.front();
    mDeleted.pop_front();
    return eid;
  }

  auto eid = mLastEntity;
  mLastEntity = Entity{static_cast<uint32_t>(mLastEntity) + 1};
  return eid;
}

bool EntityStorageSparseSet::exists(Entity entity) const {
  for (auto &deletedEntity : mDeleted) {
    if (deletedEntity == entity)
      return false;
  }

  return entity > Entity::Null && entity < mLastEntity;
}

void EntityStorageSparseSet::deleteEntity(Entity entity) {
  if (entity == Entity::Null)
    return;

  deleteAllEntityComponents(entity);
  mDeleted.push_back(entity);
  this->mNumEntities--;
}

void EntityStorageSparseSet::destroy() {
  deleteAllObservers();
  deleteAllComponents();
  deleteAllEntities();
}

void EntityStorageSparseSet::deleteAllEntityComponents(Entity entity) {
  for (auto &[index, pool] : mComponentPools) {
    size_t sEntity = static_cast<size_t>(entity);

    if (sEntity < pool.entityIndices.size() &&
        pool.entityIndices[sEntity] < DeadIndex) {

      size_t movedEntity = static_cast<size_t>(pool.entities.back());
      size_t entityIndexToDelete = pool.entityIndices[sEntity];

      auto &observers = mRemoveObserverPools.at(index);
      for (auto &observer : observers) {
        observer.entities.push_back(entity);
        observer.components.push_back(pool.components[entityIndexToDelete]);
      }

      // Move last entity in the array to place of deleted entity
      pool.entities[entityIndexToDelete] = static_cast<Entity>(movedEntity);

      // Change index of moved entity to the index of deleted entity
      pool.entityIndices[movedEntity] = entityIndexToDelete;

      // Delete last item from entities array
      pool.entities.pop_back();

      // Move last component in the array to place of deleted component
      pool.components[entityIndexToDelete] = pool.components.back();

      // Delete last item from components array
      pool.components.pop_back();

      pool.entityIndices[sEntity] = DeadIndex;
    }
  }
}

void EntityStorageSparseSet::deleteAllEntities() {
  mLastEntity = Entity{1};
  mDeleted.clear();
  mNumEntities = 0;
}

void EntityStorageSparseSet::deleteAllComponents() {
  for (auto &[_, pool] : mComponentPools) {
    pool.components.clear();
    pool.entities.clear();
    pool.entityIndices.clear();
  }
}

void EntityStorageSparseSet::deleteAllObservers() {
  for (auto &[_, pool] : mRemoveObserverPools) {
    pool.clear();
  }
}

} // namespace quoll
