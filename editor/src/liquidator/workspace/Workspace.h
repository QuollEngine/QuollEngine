#pragma once

#include "liquid/scene/SceneIO.h"
#include "liquidator/state/WorkspaceState.h"
#include "liquidator/actions/ActionExecutor.h"
#include "liquidator/asset/AssetManager.h"

#include "WorkspaceContext.h"

namespace liquid::editor {

/**
 * @brief Workspace
 */
class Workspace {
public:
  /**
   * @brief Create workspace
   *
   * @param project Project
   * @param assetManager Asset manager
   * @param scenePath Scene path
   */
  Workspace(Project project, AssetManager &assetManager, Path scenePath);

  Workspace(const Workspace &) = delete;
  Workspace &operator=(const Workspace &) = delete;
  Workspace(Workspace &&) = delete;
  Workspace &operator=(Workspace &&) = delete;

  /**
   * @brief Destroy workspace
   */
  ~Workspace();

  /**
   * @brief Get context
   *
   * @return Workspace context
   */
  inline WorkspaceContext getContext() {
    return WorkspaceContext{mState, mActionExecutor, mAssetManager};
  }

  /**
   * @brief Render layout
   */
  void renderLayout();

  /**
   * @brief Update
   */
  void update();

private:
  AssetManager &mAssetManager;
  WorkspaceState mState;
  ActionExecutor mActionExecutor;
  SceneIO mSceneIO;

  bool mRequiresDockspaceInit = false;

  String mLayoutPath;
};

} // namespace liquid::editor
