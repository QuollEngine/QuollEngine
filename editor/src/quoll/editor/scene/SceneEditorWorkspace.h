#pragma once

#include "quoll/io/SceneIO.h"
#include "quoll/loop/MainEngineModules.h"
#include "quoll/renderer/Renderer.h"
#include "quoll/renderer/SceneRenderer.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/asset/AssetManager.h"
#include "quoll/editor/ui/ShortcutsManager.h"
#include "quoll/editor/workspace/Workspace.h"
#include "quoll/editor/workspace/WorkspaceManager.h"
#include "quoll/editor/workspace/WorkspaceMatchParams.h"
#include "quoll/editor/workspace/WorkspaceState.h"
#include "asset/SceneWriter.h"
#include "renderer/EditorRenderer.h"
#include "renderer/MousePickingGraph.h"
#include "ui/SceneEditorUI.h"

namespace quoll::editor {

class SceneEditorWorkspace : public Workspace {
public:
  SceneEditorWorkspace(Project project, AssetManager &assetManager,
                       SceneAssetHandle scene, Path scenePath,
                       Renderer &renderer, SceneRenderer &sceneRenderer,
                       EditorRenderer &editorRenderer,
                       MousePickingGraph &mousePickingGraph,
                       MainEngineModules &engineModules,
                       EditorCamera &editorCamera,
                       WorkspaceManager &workspaceManager);

  virtual ~SceneEditorWorkspace();

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
