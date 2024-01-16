#pragma once

#include "quoll/io/SceneIO.h"
#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/asset/AssetManager.h"
#include "quoll/editor/ui/ShortcutsManager.h"
#include "quoll/loop/MainEngineModules.h"

#include "quoll/editor/workspace/WorkspaceMatchParams.h"
#include "quoll/editor/workspace/Workspace.h"

#include "quoll/renderer/Renderer.h"
#include "quoll/renderer/SceneRenderer.h"

#include "renderer/EditorRenderer.h"
#include "renderer/MousePickingGraph.h"
#include "ui/SceneSimulatorUI.h"

namespace quoll::editor {

/**
 * @brief Workspace
 */
class SceneSimulatorWorkspace : public Workspace {
public:
  /**
   * @brief Create workspace
   *
   * @param project Project
   * @param assetManager Asset manager
   * @param scene Scene asset
   * @param sourceScene Source scene
   * @param renderer Renderer
   * @param sceneRenderer Scene renderer
   * @param editorRenderer Editor renderer
   * @param mousePickingGraph Mouse picking graph
   * @param engineModules Main engine modules
   * @param editorCamera Editor camera
   */
  SceneSimulatorWorkspace(Project project, AssetManager &assetManager,
                          SceneAssetHandle scene, Scene &sourceScene,
                          Renderer &renderer, SceneRenderer &sceneRenderer,
                          EditorRenderer &editorRenderer,
                          MousePickingGraph &mousePickingGraph,
                          MainEngineModules &engineModules,
                          EditorCamera &editorCamera);

  SceneSimulatorWorkspace(const SceneSimulatorWorkspace &) = delete;
  SceneSimulatorWorkspace &operator=(const SceneSimulatorWorkspace &) = delete;
  SceneSimulatorWorkspace(SceneSimulatorWorkspace &&) = delete;
  SceneSimulatorWorkspace &operator=(SceneSimulatorWorkspace &&) = delete;

  /**
   * @brief Destroy workspace
   */
  virtual ~SceneSimulatorWorkspace();

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
  SceneAssetHandle mSceneAssetHandle;
  ActionExecutor mActionExecutor;
  SceneIO mSceneIO;
  ShortcutsManager mShortcutsManager;

  MainEngineModules &mEngineModules;
  EditorCamera &mEditorCamera;

  SceneSimulatorUI mUIRoot;

  Renderer &mRenderer;
  SceneRenderer &mSceneRenderer;
  EditorRenderer &mEditorRenderer;
  MousePickingGraph &mMousePickingGraph;

  bool mRequiresDockspaceInit = false;
  bool mMouseClicked = false;

  String mLayoutPath;
};

} // namespace quoll::editor
