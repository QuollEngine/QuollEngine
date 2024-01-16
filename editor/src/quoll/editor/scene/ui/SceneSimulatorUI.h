#pragma once

#include "quoll/animation/AnimationSystem.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/renderer/Renderer.h"
#include "quoll/scene/CameraAspectRatioUpdater.h"
#include "quoll/ui/UICanvasUpdater.h"
#include "quoll/renderer/Renderer.h"
#include "quoll/renderer/SceneRenderer.h"
#include "quoll/loop/MainEngineModules.h"

#include "quoll/editor/ui/StatusBar.h"
#include "quoll/editor/ui/IconRegistry.h"
#include "quoll/editor/workspace/WorkspaceManager.h"

#include "../renderer/EditorRenderer.h"
#include "../renderer/MousePickingGraph.h"

#include "SceneHierarchyPanel.h"
#include "EntityPanel.h"
#include "EditorGridPanel.h"
#include "SceneView.h"
#include "SceneGizmos.h"
#include "Inspector.h"
#include "AssetBrowser.h"

namespace quoll::editor {

/**
 * @brief Root UI entry point
 *
 * Renders all the UI in the scene
 */
class SceneSimulatorUI {
public:
  /**
   * @brief Render UI Root
   *
   * Renders all components inside the root
   *
   * @param state Workspace state
   * @param assetManager Asset manager
   * @param actionExecutor Action executor
   * @param renderer Renderer
   * @param sceneRenderer Scene renderer
   * @param editorRenderer Editor renderer
   * @param mousePickingGraph Mouse picking graph
   * @param engineModules Engine modules
   */
  void render(WorkspaceState &state, AssetManager &assetManager,
              ActionExecutor &actionExecutor, Renderer &renderer,
              SceneRenderer &sceneRenderer, EditorRenderer &editorRenderer,
              MousePickingGraph &mousePickingGraph,
              MainEngineModules &engineModules);

  /**
   * @brief Render scene view
   *
   * @param state Workspace state
   * @param assetManager Asset manager
   * @param actionExecutor Action executor
   * @param sceneTexture Scene texture
   * @param engineModules Engine modules
   * @param editorCamera Editor camera
   *
   * @retval true Entity is clicked
   * @retval false Entity is not clicked
   */
  bool renderSceneView(WorkspaceState &state, AssetManager &assetManager,
                       ActionExecutor &actionExecutor,
                       rhi::TextureHandle sceneTexture,
                       MainEngineModules &engineModules,
                       EditorCamera &editorCamera);

  /**
   * @brief Get asset browser panel
   *
   * @return Asset browser panel
   */
  inline AssetBrowser &getAssetBrowser() { return mAssetBrowser; }

  /**
   * @brief Get scene hierarchy panel
   *
   * @return Scene hierarchy panel
   */
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
