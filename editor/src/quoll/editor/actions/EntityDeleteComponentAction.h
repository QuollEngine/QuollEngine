#pragma once

#include "Action.h"

namespace quoll::editor {

template <class TComponent> class EntityDeleteComponent : public Action {
public:
  EntityDeleteComponent(Entity entity) : mEntity(entity) {}

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override {
    auto &scene = state.scene;

    mOldComponent = scene.entityDatabase.get<TComponent>(mEntity);

    scene.entityDatabase.remove<TComponent>(mEntity);

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);
    res.addToHistory = true;

    return res;
  }

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override {
    auto &scene = state.scene;

    scene.entityDatabase.set(mEntity, mOldComponent);

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);

    return res;
  }

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override {
    auto &scene = state.scene;

    return scene.entityDatabase.has<TComponent>(mEntity);
  }

private:
  Entity mEntity;
  TComponent mOldComponent;
};

} // namespace quoll::editor
