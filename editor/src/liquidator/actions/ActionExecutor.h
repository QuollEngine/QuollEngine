#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/asset/SceneWriter.h"
#include "liquid/scene/SceneIO.h"

namespace liquid::editor {

/**
 * @brief Action executor
 */
class ActionExecutor {
  using ActionStack = std::list<std::unique_ptr<Action>>;

public:
  /**
   * @brief Create action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @param scenePath Scene path
   */
  ActionExecutor(WorkspaceState &state, AssetRegistry &assetRegistry,
                 Path scenePath);

  /**
   * @brief Process actions
   */
  void process();

  /**
   * @brief Execute action
   *
   * @param action Action
   */
  void execute(std::unique_ptr<Action> action);

  /**
   * @brief Execute action
   *
   * @tparam TAction Action type
   * @tparam ...TArgs Action type arguments
   * @param ...args Action arguments
   */
  template <typename TAction, typename... TArgs> void execute(TArgs &&...args) {
    execute(std::make_unique<TAction>(std::forward<TArgs>(args)...));
  }

  /**
   * @brief Undo last action
   */
  void undo();

  /**
   * @brief Redo last action
   */
  void redo();

  /**
   * @brief Get undo stack
   *
   * @return Undo stack
   */
  inline const ActionStack &getUndoStack() const { return mUndoStack; }

  /**
   * @brief Get redo stack
   *
   * @return Redo stack
   */
  inline const ActionStack &getRedoStack() const { return mRedoStack; }

  /**
   * @brief Get scene writer
   *
   * @return Scene writer
   */
  inline SceneWriter &getSceneWriter() { return mSceneWriter; }

private:
  /**
   * @brief Save action result
   *
   * @param result Action executor result
   */
  void saveActionResult(const ActionExecutorResult &result);

private:
  WorkspaceState &mState;
  AssetRegistry &mAssetRegistry;
  Path mScenePath;
  SceneWriter mSceneWriter;

  std::unique_ptr<Action> mActionToProcess;
  ActionStack mUndoStack;
  ActionStack mRedoStack;
};

} // namespace liquid::editor
