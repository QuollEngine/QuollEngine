#pragma once

#include "Action.h"

namespace quoll::editor {

/**
 * @brief Set audio for entity action
 */
class EntitySetAudio : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param audio Audio asset handle
   */
  EntitySetAudio(Entity entity, AudioAssetHandle audio);

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
   * @brief Action undo
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
  AudioAssetHandle mAudio;
  AudioAssetHandle mOldAudio = AudioAssetHandle::Null;
};

/**
 * @brief Create audio entity action
 */
class EntityCreateAudio : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param handle Audio asset handle
   */
  EntityCreateAudio(Entity entity, AudioAssetHandle handle);

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
   * @brief Action undo
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
  AudioAssetHandle mHandle;
};

} // namespace quoll::editor
