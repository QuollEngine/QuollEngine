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

  if (db.has<Parent>(mEntity)) {
    mPreviousParent = db.get<Parent>(mEntity).parent;
  } else {
    mPreviousParent = Entity::Null;
  }

  EntityRelationUtils::setEntityParent(db, mEntity, mParent);
  ActionExecutorResult res;
  res.addToHistory = true;

  res.entitiesToSave = {mEntity, mParent};
  if (mPreviousParent != Entity::Null) {
    res.entitiesToSave.push_back(mPreviousParent);
  }

  return res;
}

ActionExecutorResult EntitySetParent::onUndo(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;
  auto &db = scene.entityDatabase;

  if (mPreviousParent != Entity::Null) {
    EntityRelationUtils::setEntityParent(db, mEntity, mPreviousParent);
  } else {
    EntityRelationUtils::removeEntityParent(db, mEntity);
  }

  ActionExecutorResult res;

  res.entitiesToSave = {mEntity, mParent};
  if (mPreviousParent != Entity::Null) {
    res.entitiesToSave.push_back(mPreviousParent);
  }

  return res;
}

bool EntitySetParent::predicate(WorkspaceState &state,
                                AssetRegistry &assetRegistry) {

  return state.scene.entityDatabase.exists(mParent) &&
         EntityRelationUtils::isValidParentForEntity(state.scene.entityDatabase,
                                                     mEntity, mParent) ==
             EntityReparentStatus::Ok;
}

EntityRemoveParent::EntityRemoveParent(Entity entity) : mEntity(entity) {}

ActionExecutorResult
EntityRemoveParent::onExecute(WorkspaceState &state,
                              AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  auto parent = scene.entityDatabase.get<Parent>(mEntity).parent;

  scene.entityDatabase.remove<Parent>(mEntity);

  auto &children = scene.entityDatabase.get<Children>(parent).children;
  if (children.size() > 1) {
    auto it = std::find(children.begin(), children.end(), mEntity);

    mChildIndex = std::distance(children.begin(), it);
    children.erase(it);
  } else {
    scene.entityDatabase.remove<Children>(parent);
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

  scene.entityDatabase.set<Parent>(mEntity, {mPreviousParent});
  if (scene.entityDatabase.has<Children>(mPreviousParent)) {
    auto &children =
        scene.entityDatabase.get<Children>(mPreviousParent).children;
    children.insert(children.begin() + mChildIndex, mEntity);
  } else {
    scene.entityDatabase.set<Children>(mPreviousParent, {{mEntity}});
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);

  return res;
}

bool EntityRemoveParent::predicate(WorkspaceState &state,
                                   AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  return scene.entityDatabase.has<Parent>(mEntity);
}

} // namespace quoll::editor
