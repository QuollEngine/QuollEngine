#pragma once

namespace liquid::editor {

/**
 * @brief Entity default component delete action
 *
 * @tparam TComponent Component
 */
template <class TComponent> class EntityDefaultDeleteAction : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  EntityDefaultDeleteAction(Entity entity) : mEntity(entity) {}

  /**
   * @brief Execute action
   *
   * @param state Workspace state
   * @return Execution result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) {
    auto &scene = state.mode == WorkspaceMode::Simulation
                      ? state.simulationScene
                      : state.scene;

    scene.entityDatabase.remove<TComponent>(mEntity);

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);
    return res;
  }

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Entity has component
   * @retval false Entity does not have component
   */
  bool predicate(WorkspaceState &state) {
    auto &scene = state.mode == WorkspaceMode::Simulation
                      ? state.simulationScene
                      : state.scene;

    return scene.entityDatabase.has<TComponent>(mEntity);
  }

private:
  Entity mEntity;
};

} // namespace liquid::editor