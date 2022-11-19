#include "liquid/core/Base.h"
#include "EntityStorageSparseSet.h"

namespace liquid {

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

  auto eid = mLastEntity++;
  return eid;
}

bool EntityStorageSparseSet::exists(Entity entity) const {
  for (auto &deletedEntity : mDeleted) {
    if (deletedEntity == entity)
      return false;
  }

  return entity > EntityNull && entity < mLastEntity;
}

void EntityStorageSparseSet::deleteEntity(Entity entity) {
  if (entity == EntityNull)
    return;

  deleteAllEntityComponents(entity);
  mDeleted.push_back(entity);
  this->mNumEntities--;
}

void EntityStorageSparseSet::destroy() {
  deleteAllComponents();
  deleteAllEntities();
}

void EntityStorageSparseSet::deleteAllEntityComponents(Entity entity) {
  for (auto &[_, pool] : mComponentPools) {
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
  }
}

void EntityStorageSparseSet::deleteAllEntities() {
  mLastEntity = 1;
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

} // namespace liquid
