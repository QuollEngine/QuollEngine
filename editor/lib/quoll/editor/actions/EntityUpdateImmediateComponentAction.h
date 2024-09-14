#pragma once

#include "Action.h"

namespace quoll::editor {

template <class TComponent>
class EntityUpdateImmediateComponent : public Action {
public:
  EntityUpdateImmediateComponent(Entity entity, TComponent newComponent)
      : mEntity(entity), mNewComponent(newComponent) {}

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override {
    auto &scene = state.scene;

    mOldComponent = scene.entityDatabase.get<TComponent>(mEntity);
    scene.entityDatabase.set(mEntity, mNewComponent);

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
    return state.scene.entityDatabase.has<TComponent>(mEntity);
  }

private:
  Entity mEntity;
  TComponent mOldComponent;
  TComponent mNewComponent;
};

} // namespace quoll::editor
