#include "quoll/core/Base.h"
#include "quoll/core/Assert.h"
#include "quoll/entity/EntityDatabase.h"
#include "Children.h"
#include "EntityRelationUtils.h"
#include "Parent.h"

namespace quoll {

void EntityRelationUtils::removeEntityParent(EntityDatabase &entityDatabase,
                                             Entity entity) {
  if (entity.has<Parent>()) {
    auto parent = entity.get_ref<Parent>()->parent;

    QuollAssert(parent.has<Children>(), "Parent entity must have children");

    auto &children = parent.get_ref<Children>()->children;
    auto it = std::find(children.begin(), children.end(), entity);

    QuollAssert(it != children.end(),
                "Entity must exist in children of parent");

    if (children.size() == 1) {
      parent.remove<Children>();
    } else {
      children.erase(it);
    }
    entity.remove<Parent>();
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

  if (!parent.has<Children>()) {
    parent.set<Children>({});
  }

  parent.get_ref<Children>()->children.push_back(entity);
  entity.set<Parent>({parent});

  return status;
}

EntityReparentStatus
EntityRelationUtils::isValidParentForEntity(EntityDatabase &entityDatabase,
                                            Entity entity, Entity parent) {
  if (entity.has<Parent>() && entity.get_ref<Parent>()->parent == parent) {
    return EntityReparentStatus::ParentHasNotChanged;
  }

  auto parentInner = parent;
  bool parentIsNotDescendant = parentInner != entity;
  while (parentIsNotDescendant && parentInner.is_valid() &&
         parentInner.has<Parent>()) {
    auto p = parentInner.get_ref<Parent>()->parent;
    parentIsNotDescendant = p != entity;
    parentInner = p;
  }

  return parentIsNotDescendant
             ? EntityReparentStatus::Ok
             : EntityReparentStatus::CannotParentEntityToDescendant;
}

} // namespace quoll
