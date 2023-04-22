#pragma once

#include "liquidator/actions/Action.h"
#include "liquid/scene/SceneIO.h"

namespace liquid::editor {

/**
 * @brief Action executor
 */
class ActionExecutor {
public:
  /**
   * @brief Create action executor
   *
   * @param state Workspace state
   * @param scenePath Scene path
   */
  ActionExecutor(WorkspaceState &state, Path scenePath);

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
   * @brief Get scene IO
   *
   * @return Scene IO
   */
  inline SceneIO &getSceneIO() { return mSceneIO; }

private:
  WorkspaceState &mState;
  SceneIO mSceneIO;
  Path mScenePath;

  std::unique_ptr<Action> mActionToProcess;
};

} // namespace liquid::editor
