#pragma once

#include "liquidator/actions/Action.h"
#include "EntityDefaultDeleteAction.h"

namespace quoll::editor {

/**
 * @brief Create perspective lens entity action
 */
class EntityCreatePerspectiveLens : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  EntityCreatePerspectiveLens(Entity entity);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
};

using EntitySetPerspectiveLens = EntityDefaultUpdateComponent<PerspectiveLens>;

using EntitySetCameraAutoAspectRatio =
    EntityDefaultCreateComponent<AutoAspectRatio>;

using EntitySetCameraCustomAspectRatio =
    EntityDefaultDeleteAction<AutoAspectRatio>;

/**
 * @brief Delete camera action
 */
class EntityDeletePerspectiveLens : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  EntityDeletePerspectiveLens(Entity entity);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  PerspectiveLens mOldPerspectiveLens;
  std::optional<AutoAspectRatio> mOldAspectRatio;

  bool mIsActiveCamera = false;
  bool mIsStartingCamera = false;
};

} // namespace quoll::editor
