#pragma once

#include "quoll/scene/SceneIO.h"
#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/asset/AssetManager.h"
#include "quoll/editor/asset/SceneWriter.h"
#include "quoll/editor/ui/ShortcutsManager.h"

#include "WorkspaceMatchParams.h"
#include "WorkspaceContext.h"

#include "quoll/renderer/Renderer.h"
#include "quoll/renderer/SceneRenderer.h"
#include "quoll/editor/core/EditorRenderer.h"
#include "quoll/editor/core/MousePickingGraph.h"
#include "quoll/editor/core/EditorSimulator.h"

#include "quoll/editor/ui/UIRoot.h"

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
   * @param renderer Renderer
   * @param sceneRenderer Scene renderer
   * @param editorRenderer Editor renderer
   * @param mousePickingGraph Mouse picking graph
   * @param editorSimulator Editor simulator
   */
  Workspace(Project project, AssetManager &assetManager, SceneAssetHandle scene,
            Path scenePath, Renderer &renderer, SceneRenderer &sceneRenderer,
            EditorRenderer &editorRenderer,
            MousePickingGraph &mousePickingGraph,
            EditorSimulator &editorSimulator);

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
   *
   * @param dt Delta time
   */
  void update(f32 dt);

  /**
   * @brief Render
   */
  void render();

  /**
   * @brief Process shortcuts
   *
   * @param key Key
   * @param mods Modifiers
   */
  void processShortcuts(int key, int mods);

  /**
   * @brief Update frame data
   *
   * @param commandList Render command list
   * @param frameIndex Frame index
   */
  void updateFrameData(rhi::RenderCommandList &commandList, u32 frameIndex);

  /**
   * @brief Get match params
   *
   * @return Match params
   */
  WorkspaceMatchParams getMatchParams() const;

  /**
   * @brief Get UI root
   *
   * @return UI root
   */
  inline UIRoot &getUIRoot() { return mUIRoot; }

private:
  AssetManager &mAssetManager;
  WorkspaceState mState;
  SceneWriter mSceneWriter;
  SceneAssetHandle mSceneAssetHandle;
  ActionExecutor mActionExecutor;
  SceneIO mSceneIO;
  ShortcutsManager mShortcutsManager;

  UIRoot mUIRoot;

  Renderer &mRenderer;
  SceneRenderer &mSceneRenderer;
  EditorRenderer &mEditorRenderer;
  MousePickingGraph &mMousePickingGraph;
  EditorSimulator &mEditorSimulator;

  bool mMouseClicked = false;

  bool mRequiresDockspaceInit = false;

  String mLayoutPath;
};

} // namespace quoll::editor
