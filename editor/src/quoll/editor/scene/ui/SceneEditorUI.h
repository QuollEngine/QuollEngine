#pragma once

#include "quoll/animation/AnimationSystem.h"
#include "quoll/loop/MainEngineModules.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/renderer/Renderer.h"
#include "quoll/renderer/SceneRenderer.h"
#include "quoll/scene/CameraAspectRatioUpdater.h"
#include "quoll/editor/ui/IconRegistry.h"
#include "quoll/editor/ui/StatusBar.h"
#include "quoll/editor/workspace/WorkspaceManager.h"
#include "../renderer/EditorRenderer.h"
#include "../renderer/MousePickingGraph.h"
#include "AssetBrowser.h"
#include "EditorGridPanel.h"
#include "EntityPanel.h"
#include "Inspector.h"
#include "SceneGizmos.h"
#include "SceneHierarchyPanel.h"
#include "SceneView.h"

namespace quoll::editor {

class SceneEditorUI {
public:
  void render(WorkspaceState &state, AssetManager &assetManager,
              ActionExecutor &actionExecutor,
              AssetHandle<SceneAsset> sceneHandle, Renderer &renderer,
              SceneRenderer &sceneRenderer, EditorRenderer &editorRenderer,
              MousePickingGraph &mousePickingGraph,
              MainEngineModules &engineModules, EditorCamera &editorCamera,
              WorkspaceManager &workspaceManager);

  bool renderSceneView(WorkspaceState &state, AssetManager &assetManager,
                       ActionExecutor &actionExecutor,
                       rhi::TextureHandle sceneTexture,
                       MainEngineModules &engineModules,
                       EditorCamera &editorCamera);

  inline AssetBrowser &getAssetBrowser() { return mAssetBrowser; }

  inline SceneHierarchyPanel &getSceneHierarchyPanel() {
    return mSceneHierarchyPanel;
  }

private:
  SceneHierarchyPanel mSceneHierarchyPanel;
  EntityPanel mEntityPanel;
  EditorGridPanel mEditorCameraPanel;
  AssetBrowser mAssetBrowser;
  Inspector mInspector;

  // Unclosable UI
  StatusBar mStatusBar;
  SceneGizmos mSceneGizmos;
};

} // namespace quoll::editor
