#pragma once

#include "Action.h"

namespace quoll::editor {

/**
 * @brief Entity update component action
 *
 * @tparam TComponent Component
 */
template <class TComponent> class EntityUpdateComponent : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param oldComponent Old component
   * @param newComponent New component
   */
  EntityUpdateComponent(Entity entity, TComponent oldComponent = {},
                        std::optional<TComponent> newComponent = std::nullopt)
      : mEntity(entity), mOldComponent(oldComponent),
        mNewComponent(newComponent) {}

  /**
   * @brief Set new component
   *
   * @param newComponent New component
   */
  void setNewComponent(TComponent newComponent) {
    mNewComponent = newComponent;
  }

  /**
   * @brief Update to new component
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Execution result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override {
    auto &scene = state.scene;

    scene.entityDatabase.set(mEntity, mNewComponent.value());

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);
    res.addToHistory = true;

    return res;
  }

  /**
   * @brief Update to previous component
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Execution result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override {
    auto &scene = state.scene;

    scene.entityDatabase.set(mEntity, mOldComponent);

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);
    return res;
  }

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Entity has component
   * @retval false Entity does not have component
   */
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