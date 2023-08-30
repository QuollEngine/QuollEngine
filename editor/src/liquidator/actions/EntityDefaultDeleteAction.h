#pragma once

namespace quoll::editor {

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

/**
 * @brief Entity default component delete action
 *
 * @tparam TComponent Component
 */
template <class TComponent> class EntityDefaultCreateComponent : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param component Component
   */
  EntityDefaultCreateComponent(Entity entity, TComponent component = {})
      : mEntity(entity), mComponent(component) {}

  /**
   * @brief Create component
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

    scene.entityDatabase.set<TComponent>(mEntity, mComponent);

    ActionExecutorResult res{};
    res.entitiesToSave.push_back(mEntity);
    res.addToHistory = true;
    return res;
  }

  /**
   * @brief Delete component
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

    scene.entityDatabase.remove<TComponent>(mEntity);

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

    return !scene.entityDatabase.has<TComponent>(mEntity);
  }

private:
  Entity mEntity;
  TComponent mComponent;
};

/**
 * @brief Entity default component delete action
 *
 * @tparam TComponent Component
 */
template <class TComponent> class EntityDefaultUpdateComponent : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param oldComponent Old component
   * @param newComponent New component
   */
  EntityDefaultUpdateComponent(
      Entity entity, TComponent oldComponent = {},
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
    auto &scene = state.mode == WorkspaceMode::Simulation
                      ? state.simulationScene
                      : state.scene;

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

    return mNewComponent.has_value();
  }

private:
  Entity mEntity;
  TComponent mOldComponent;
  std::optional<TComponent> mNewComponent;
};

} // namespace quoll::editor