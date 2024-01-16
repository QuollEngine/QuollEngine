#pragma once

#include "Action.h"

namespace quoll::editor {

template <class TComponent> class EntityUpdateComponent : public Action {
public:
  EntityUpdateComponent(Entity entity, TComponent oldComponent = {},
                        std::optional<TComponent> newComponent = std::nullopt)
      : mEntity(entity), mOldComponent(oldComponent),
        mNewComponent(newComponent) {}

  void setNewComponent(TComponent newComponent) {
    mNewComponent = newComponent;
  }

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override {
    auto &scene = state.scene;

    scene.entityDatabase.set(mEntity, mNewComponent.value());

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);
    res.addToHistory = true;

    return res;
  }

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override {
    auto &scene = state.scene;

    scene.entityDatabase.set(mEntity, mOldComponent);

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);
    return res;
  }

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override {
    auto &scene = state.scene;

    return mNewComponent.has_value() &&
           scene.entityDatabase.has<TComponent>(mEntity);
  }

private:
  Entity mEntity;
  TComponent mOldComponent;
  std::optional<TComponent> mNewComponent;
};

} // namespace quoll::editor