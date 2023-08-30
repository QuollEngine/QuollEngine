#pragma once

#include "quoll/scene/SceneIO.h"
#include "quoll/editor/state/WorkspaceState.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/asset/AssetManager.h"

#include "WorkspaceContext.h"

namespace quoll::editor {

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
   * @param scene Scene asset
   * @param scenePath Scene path
   */
  Workspace(Project project, AssetManager &assetManager, SceneAssetHandle scene,
            Path scenePath);

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

} // namespace quoll::editor
