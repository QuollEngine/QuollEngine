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
   * @brief Execute action
   *
   * @param action Action
   * @param data Action data
   */
  void execute(const Action &action, std::any data = {});

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
};

} // namespace liquid::editor
