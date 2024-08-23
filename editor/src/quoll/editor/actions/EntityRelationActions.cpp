#include "quoll/core/Base.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/EntityRelationUtils.h"
#include "quoll/scene/Parent.h"
#include "EntityRelationActions.h"

namespace quoll::editor {

EntitySetParent::EntitySetParent(Entity entity, Entity parent)
    : mEntity(entity), mParent(parent) {}

ActionExecutorResult EntitySetParent::onExecute(WorkspaceState &state,
                                                AssetRegistry &assetRegistry) {
  auto &scene = state.scene;
  auto &db = scene.entityDatabase;

  if (mEntity.has<Parent>()) {
    mPreviousParent = mEntity.get_ref<Parent>()->parent;
  } else {
    mPreviousParent = flecs::entity::null();
  }

  EntityRelationUtils::setEntityParent(db, mEntity, mParent);
  ActionExecutorResult res;
  res.addToHistory = true;

  res.entitiesToSave = {mEntity, mParent};
  if (mPreviousParent.is_valid()) {
    res.entitiesToSave.push_back(mPreviousParent);
  }

  return res;
}

ActionExecutorResult EntitySetParent::onUndo(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;
  auto &db = scene.entityDatabase;

  if (mPreviousParent.is_valid()) {
    EntityRelationUtils::setEntityParent(db, mEntity, mPreviousParent);
  } else {
    EntityRelationUtils::removeEntityParent(db, mEntity);
  }

  ActionExecutorResult res;

  res.entitiesToSave = {mEntity, mParent};
  if (mPreviousParent.is_valid()) {
    res.entitiesToSave.push_back(mPreviousParent);
  }

  return res;
}

bool EntitySetParent::predicate(WorkspaceState &state,
                                AssetRegistry &assetRegistry) {

  return mParent.is_valid() && EntityRelationUtils::isValidParentForEntity(
                                   state.scene.entityDatabase, mEntity,
                                   mParent) == EntityReparentStatus::Ok;
}

EntityRemoveParent::EntityRemoveParent(Entity entity) : mEntity(entity) {}

ActionExecutorResult
EntityRemoveParent::onExecute(WorkspaceState &state,
                              AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  auto parent = mEntity.get_ref<Parent>()->parent;

  mEntity.remove<Parent>();

  auto &children = parent.get_ref<Children>()->children;
  if (children.size() > 1) {
    auto it = std::find(children.begin(), children.end(), mEntity);

    mChildIndex = std::distance(children.begin(), it);
    children.erase(it);
  } else {
    parent.remove<Children>();
  }

  mPreviousParent = parent;

  ActionExecutorResult res{};
  res.addToHistory = true;
  res.entitiesToSave.push_back(mEntity);

  return res;
}

ActionExecutorResult EntityRemoveParent::onUndo(WorkspaceState &state,
                                                AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.set<Parent>({mPreviousParent});
  if (mPreviousParent.has<Children>()) {
    auto &children = mPreviousParent.get_ref<Children>()->children;
    children.insert(children.begin() + mChildIndex, mEntity);
  } else {
    mPreviousParent.set<Children>({{mEntity}});
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);

  return res;
}

bool EntityRemoveParent::predicate(WorkspaceState &state,
                                   AssetRegistry &assetRegistry) {
  return mEntity.has<Parent>();
}

} // namespace quoll::editor
