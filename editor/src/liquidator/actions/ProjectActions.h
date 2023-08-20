#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/asset/AssetManager.h"

namespace liquid::editor {

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

} // namespace liquid::editor
