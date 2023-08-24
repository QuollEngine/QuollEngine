#pragma once

#include "Action.h"

namespace liquid::editor {

/**
 * @brief Entity remove parent action
 */
class EntityRemoveParent : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  EntityRemoveParent(Entity entity);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Execution result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action undo
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Execution result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Parent is valid
   * @retval false Parent is not valid
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  Entity mPreviousParent = Entity::Null;
  std::ptrdiff_t mChildIndex = 0;
};

/**
 * @brief Entity set parent action
 */
class EntitySetParent : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param parent Parent
   */
  EntitySetParent(Entity entity, Entity parent);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Execution result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action undo
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Execution result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Parent is valid
   * @retval false Parent is not valid
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  Entity mParent;
  Entity mPreviousParent = Entity::Null;
};

} // namespace liquid::editor
