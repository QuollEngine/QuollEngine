#pragma once

#include "liquidator/actions/Action.h"
#include "EntityDefaultDeleteAction.h"

namespace liquid::editor {

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
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

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
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  Entity mEntity;
  PerspectiveLens mOldPerspectiveLens;
  std::optional<AutoAspectRatio> mOldAspectRatio;

  bool mIsActiveCamera = false;
  bool mIsStartingCamera = false;
};

} // namespace liquid::editor
