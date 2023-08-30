#include "quoll/core/Base.h"
#include "EntityRelationActions.h"

namespace quoll::editor {

EntitySetParent::EntitySetParent(Entity entity, Entity parent)
    : mEntity(entity), mParent(parent) {}

ActionExecutorResult EntitySetParent::onExecute(WorkspaceState &state,
                                                AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  auto &db = scene.entityDatabase;

  if (db.has<Parent>(mEntity)) {
    mPreviousParent = db.get<Parent>(mEntity).parent;

    QuollAssert(db.has<Children>(mPreviousParent),
                "Parent entity must have children");
    auto &children = db.get<Children>(mPreviousParent).children;

    auto it = std::find(children.begin(), children.end(), mEntity);

    QuollAssert(it != children.end(),
                "Entity must exist in children of parent");

    if (children.size() == 1) {
      db.remove<Children>(mPreviousParent);
    } else {
      children.erase(it);
    }
  } else {
    mPreviousParent = Entity::Null;
  }

  db.set<Parent>(mEntity, {mParent});
  if (db.has<Children>(mParent)) {
    db.get<Children>(mParent).children.push_back(mEntity);
  } else {
    db.set<Children>(mParent, {{mEntity}});
  }

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
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  auto &db = scene.entityDatabase;

  if (mPreviousParent != Entity::Null) {
    if (db.has<Children>(mPreviousParent)) {
      db.get<Children>(mPreviousParent).children.push_back(mEntity);
    } else {
      db.set<Children>(mPreviousParent, {{mEntity}});
    }

    db.set<Parent>(mEntity, {mPreviousParent});
  } else {
    db.remove<Parent>(mEntity);
  }

  QuollAssert(db.has<Children>(mParent), "Entity parent has no children");
  auto &children = db.get<Children>(mParent).children;
  auto it = std::find(children.begin(), children.end(), mEntity);

  QuollAssert(it != children.end(), "Entity must exist in children of parent");

  if (children.size() == 1) {
    db.remove<Children>(mParent);
  } else {
    children.erase(it);
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
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  auto &db = scene.entityDatabase;

  // Parent does not exist
  if (!db.exists(mParent)) {
    return false;
  }

  // Parent is already a parent of entity
  if (db.has<Parent>(mEntity) && db.get<Parent>(mEntity).parent == mParent) {
    return false;
  }

  auto parent = mParent;
  bool parentIsNotDescendant = parent != mEntity;
  while (parentIsNotDescendant && db.has<Parent>(parent)) {
    auto p = db.get<Parent>(parent).parent;
    parentIsNotDescendant = p != mEntity;
    parent = p;
  }

  return parentIsNotDescendant;
}

EntityRemoveParent::EntityRemoveParent(Entity entity) : mEntity(entity) {}

ActionExecutorResult
EntityRemoveParent::onExecute(WorkspaceState &state,
                              AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

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
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

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
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<Parent>(mEntity);
}

} // namespace quoll::editor
