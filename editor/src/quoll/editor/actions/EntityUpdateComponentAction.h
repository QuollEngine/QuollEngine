#pragma once

#include "Action.h"

namespace quoll::editor {

template <class TComponent> class EntityUpdateComponent : public Action {
public:
  EntityUpdateComponent(Entity entity, TComponent oldComponent = {},
                        std::optional<TComponent> newComponent = std::nullopt)
      : mEntity(entity), mOldComponent(oldComponent),
        mNewComponent(newComponent) {}

  EntityUpdateComponent(Entity entity, flecs::ref<TComponent> oldComponent,
                        std::optional<TComponent> newComponent = std::nullopt)
      : mEntity(entity), mOldComponent(*oldComponent.get()),
        mNewComponent(newComponent) {}

  void setNewComponent(TComponent newComponent) {
    mNewComponent = newComponent;
  }

  void setNewComponent(flecs::ref<TComponent> newComponent) {
    mNewComponent = *newComponent.get();
  }

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override {
    auto &scene = state.scene;

    mEntity.set(mNewComponent.value());

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

    return mNewComponent.has_value() && mEntity.has<TComponent>();
  }

private:
  Entity mEntity;
  TComponent mOldComponent;
  std::optional<TComponent> mNewComponent;
};

} // namespace quoll::editor