#pragma once

#include "quoll/io/SceneIO.h"
#include "quoll/loop/MainEngineModules.h"
#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/asset/AssetManager.h"
#include "quoll/editor/ui/ShortcutsManager.h"
#include "quoll/loop/MainEngineModules.h"

#include "quoll/editor/workspace/WorkspaceMatchParams.h"
#include "quoll/editor/workspace/Workspace.h"
#include "quoll/editor/workspace/WorkspaceManager.h"

#include "quoll/renderer/Renderer.h"
#include "quoll/renderer/SceneRenderer.h"

#include "renderer/EditorRenderer.h"
#include "renderer/MousePickingGraph.h"
#include "asset/SceneWriter.h"
#include "ui/SceneEditorUI.h"

namespace quoll::editor {

/**
 * @brief Workspace
 */
class SceneEditorWorkspace : public Workspace {
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
   * @param engineModules Main engine modules
   * @param editorCamera Editor camera
   * @param workspaceManager Workspace manager
   */
  SceneEditorWorkspace(Project project, AssetManager &assetManager,
                       SceneAssetHandle scene, Path scenePath,
                       Renderer &renderer, SceneRenderer &sceneRenderer,
                       EditorRenderer &editorRenderer,
                       MousePickingGraph &mousePickingGraph,
                       MainEngineModules &engineModules,
                       EditorCamera &editorCamera,
                       WorkspaceManager &workspaceManager);

  SceneEditorWorkspace(const SceneEditorWorkspace &) = delete;
  SceneEditorWorkspace &operator=(const SceneEditorWorkspace &) = delete;
  SceneEditorWorkspace(SceneEditorWorkspace &&) = delete;
  SceneEditorWorkspace &operator=(SceneEditorWorkspace &&) = delete;

  /**
   * @brief Destroy workspace
   */
  virtual ~SceneEditorWorkspace();

  /**
   * @brief Data preparation step
   */
  void prepare() override;

  /**
   * @brief Fixed update
   *
   * @param dt Delta time
   */
  void fixedUpdate(f32 dt) override;

  /**
   * @brief Update
   *
   * @param dt Delta time
   */
  void update(f32 dt) override;

  /**
   * @brief Render
   */
  void render() override;

  /**
   * @brief Process shortcuts
   *
   * @param key Key
   * @param mods Modifiers
   */
  void processShortcuts(int key, int mods) override;

  /**
   * @brief Update frame data
   *
   * @param commandList Render command list
   * @param frameIndex Frame index
   */
  void updateFrameData(rhi::RenderCommandList &commandList,
                       u32 frameIndex) override;

  /**
   * @brief Get match params
   *
   * @return Match params
   */
  WorkspaceMatchParams getMatchParams() const override;

  /**
   * @brief Reload workspace UI
   */
  void reload() override;

private:
  /**
   * @brief Render layout
   */
  void renderLayout();

private:
  AssetManager &mAssetManager;
  WorkspaceState mState;
  SceneWriter mSceneWriter;
  SceneAssetHandle mSceneAssetHandle;
  ActionExecutor mActionExecutor;
  SceneIO mSceneIO;
  ShortcutsManager mShortcutsManager;

  MainEngineModules &mEngineModules;
  EditorCamera &mEditorCamera;

  SceneEditorUI mUIRoot;

  Renderer &mRenderer;
  SceneRenderer &mSceneRenderer;
  EditorRenderer &mEditorRenderer;
  MousePickingGraph &mMousePickingGraph;
  WorkspaceManager &mWorkspaceManager;

  bool mMouseClicked = false;

  bool mRequiresDockspaceInit = false;

  String mLayoutPath;
};

} // namespace quoll::editor
