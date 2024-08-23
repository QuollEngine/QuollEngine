#pragma once

#include "Action.h"

namespace quoll::editor {

template <class TComponent> class EntityDeleteComponent : public Action {
public:
  EntityDeleteComponent(Entity entity) : mEntity(entity) {}

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override {
    auto &scene = state.scene;

    mOldComponent = *mEntity.get_ref<TComponent>().get();

    mEntity.remove<TComponent>();

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);
    res.addToHistory = true;

    return res;
  }

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override {
    auto &scene = state.scene;

    mEntity.set(mOldComponent);

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);

    return res;
  }

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override {
    auto &scene = state.scene;

    return mEntity.has<TComponent>();
  }

private:
  Entity mEntity;
  TComponent mOldComponent;
};

} // namespace quoll::editor
