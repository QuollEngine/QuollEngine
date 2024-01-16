#include "quoll/core/Base.h"
#include "quoll/core/Assert.h"
#include "Children.h"
#include "EntityRelationUtils.h"
#include "Parent.h"

namespace quoll {

void EntityRelationUtils::removeEntityParent(EntityDatabase &entityDatabase,
                                             Entity entity) {
  if (entityDatabase.has<Parent>(entity)) {
    auto parent = entityDatabase.get<Parent>(entity).parent;

    QuollAssert(entityDatabase.has<Children>(parent),
                "Parent entity must have children");

    auto &children = entityDatabase.get<Children>(parent).children;
    auto it = std::find(children.begin(), children.end(), entity);

    QuollAssert(it != children.end(),
                "Entity must exist in children of parent");

    if (children.size() == 1) {
      entityDatabase.remove<Children>(parent);
    } else {
      children.erase(it);
    }
    entityDatabase.remove<Parent>(entity);
  }
}

EntityReparentStatus
EntityRelationUtils::setEntityParent(EntityDatabase &entityDatabase,
                                     Entity entity, Entity parent) {
  auto status = isValidParentForEntity(entityDatabase, entity, parent);
  if (status != EntityReparentStatus::Ok) {
    return status;
  }

  removeEntityParent(entityDatabase, entity);

  if (!entityDatabase.has<Children>(parent)) {
    entityDatabase.set<Children>(parent, {});
  }

  entityDatabase.get<Children>(parent).children.push_back(entity);
  entityDatabase.set<Parent>(entity, {parent});

  return status;
}

EntityReparentStatus
EntityRelationUtils::isValidParentForEntity(EntityDatabase &entityDatabase,
                                            Entity entity, Entity parent) {
  if (entityDatabase.has<Parent>(entity) &&
      entityDatabase.get<Parent>(entity).parent == parent) {
    return EntityReparentStatus::ParentHasNotChanged;
  }

  auto parentInner = parent;
  bool parentIsNotDescendant = parentInner != entity;
  while (parentIsNotDescendant && entityDatabase.has<Parent>(parentInner)) {
    auto p = entityDatabase.get<Parent>(parentInner).parent;
    parentIsNotDescendant = p != entity;
    parentInner = p;
  }

  return parentIsNotDescendant
             ? EntityReparentStatus::Ok
             : EntityReparentStatus::CannotParentEntityToDescendant;
}

} // namespace quoll
