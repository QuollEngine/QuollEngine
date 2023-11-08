#pragma once

#include "Action.h"

namespace quoll::editor {

/**
 * @brief Entity delete component action
 *
 * @tparam TComponent Component
 */
template <class TComponent> class EntityDeleteComponent : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  EntityDeleteComponent(Entity entity) : mEntity(entity) {}

  /**
   * @brief Execute action
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Execution result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override {
    auto &scene = state.mode == WorkspaceMode::Simulation
                      ? state.simulationScene
                      : state.scene;

    mOldComponent = scene.entityDatabase.get<TComponent>(mEntity);

    scene.entityDatabase.remove<TComponent>(mEntity);

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);
    res.addToHistory = true;

    return res;
  }

  /**
   * @brief Recreate component
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Execution result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override {
    auto &scene = state.mode == WorkspaceMode::Simulation
                      ? state.simulationScene
                      : state.scene;

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
    auto &scene = state.mode == WorkspaceMode::Simulation
                      ? state.simulationScene
                      : state.scene;

    return scene.entityDatabase.has<TComponent>(mEntity);
  }

private:
  Entity mEntity;
  TComponent mOldComponent;
};

} // namespace quoll::editor
