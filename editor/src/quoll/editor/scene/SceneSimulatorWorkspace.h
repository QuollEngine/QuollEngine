#pragma once

#include "quoll/io/SceneIO.h"
#include "quoll/loop/MainEngineModules.h"
#include "quoll/renderer/Renderer.h"
#include "quoll/renderer/SceneRenderer.h"
#include "quoll/system/SystemView.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/asset/AssetManager.h"
#include "quoll/editor/ui/ShortcutsManager.h"
#include "quoll/editor/workspace/Workspace.h"
#include "quoll/editor/workspace/WorkspaceMatchParams.h"
#include "quoll/editor/workspace/WorkspaceState.h"
#include "renderer/EditorRenderer.h"
#include "renderer/MousePickingGraph.h"
#include "ui/SceneSimulatorUI.h"

namespace quoll::editor {

class SceneSimulatorWorkspace : public Workspace {
public:
  SceneSimulatorWorkspace(Project project, AssetManager &assetManager,
                          const AssetRef<SceneAsset> &sceneAsset,
                          Scene &sourceScene, Renderer &renderer,
                          SceneRenderer &sceneRenderer,
                          EditorRenderer &editorRenderer,
                          MousePickingGraph &mousePickingGraph,
                          MainEngineModules &engineModules,
                          EditorCamera &editorCamera);

  virtual ~SceneSimulatorWorkspace();

  void prepare() override;

  void fixedUpdate(f32 dt) override;

  void update(f32 dt) override;

  void render() override;

  void processShortcuts(int key, int mods) override;

  void updateFrameData(rhi::RenderCommandList &commandList,
                       u32 frameIndex) override;

  WorkspaceMatchParams getMatchParams() const override;

  void reload() override;

private:
  void renderLayout();

private:
  AssetManager &mAssetManager;
  WorkspaceState mState;
  AssetRef<SceneAsset> mSceneAsset;
  ActionExecutor mActionExecutor;
  SceneIO mSceneIO;
  ShortcutsManager mShortcutsManager;
  SystemView mSystemView;

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
