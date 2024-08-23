#pragma once

#include "Action.h"

namespace quoll::editor {

template <class TComponent> class EntityCreateComponent : public Action {
public:
  EntityCreateComponent(Entity entity, TComponent component = {})
      : mEntity(entity), mComponent(component) {}

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override {
    auto &scene = state.scene;

    mEntity.set<TComponent>(mComponent);

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);
    res.addToHistory = true;
    return res;
  }

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override {
    auto &scene = state.scene;

    mEntity.remove<TComponent>();

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);

    return res;
  }

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override {
    auto &scene = state.scene;

    return !mEntity.has<TComponent>();
  }

private:
  Entity mEntity;
  TComponent mComponent;
};

} // namespace quoll::editor
