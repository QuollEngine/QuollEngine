#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/actions/EntityDefaultDeleteAction.h"

namespace liquid::editor {

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
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action undo
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
  AudioAssetHandle mAudio;
  AudioAssetHandle mOldAudio = AudioAssetHandle::Invalid;
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
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action undo
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
  AudioAssetHandle mHandle;
};

using EntityDeleteAudio = EntityDefaultDeleteAction<AudioSource>;

} // namespace liquid::editor
