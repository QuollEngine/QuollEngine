#pragma once

#include "quoll/editor/actions/Action.h"
#include "quoll/editor/asset/AssetManager.h"

namespace quoll::editor {

/**
 * @brief Export as game action
 */
class ExportAsGame : public Action {
public:
  /**
   * @brief Create action
   *
   * @param assetManager Asset manager
   */
  ExportAsGame(AssetManager &assetManager);

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
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  AssetManager &mAssetManager;
};

} // namespace quoll::editor
