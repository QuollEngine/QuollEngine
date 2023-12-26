#pragma once

#include "quoll/animation/AnimationSystem.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/renderer/Renderer.h"
#include "quoll/scene/CameraAspectRatioUpdater.h"
#include "quoll/ui/UICanvasUpdater.h"

#include "quoll/editor/ui/Toolbar.h"
#include "quoll/editor/ui/StatusBar.h"
#include "quoll/editor/ui/IconRegistry.h"
#include "quoll/editor/ui/MainMenu.h"

#include "SceneHierarchyPanel.h"
#include "EntityPanel.h"
#include "EditorGridPanel.h"
#include "SceneView.h"
#include "SceneGizmos.h"
#include "Inspector.h"
#include "AssetBrowser.h"
#include "../core/SceneSimulator.h"

namespace quoll::editor {

/**
 * @brief Root UI entry point
 *
 * Renders all the UI in the scene
 */
class SceneEditorUI {
public:
  /**
   * @brief Create UI root
   *
   * @param assetManager Asset manager
   */
  SceneEditorUI(AssetManager &assetManager);

  /**
   * @brief Render UI Root
   *
   * Renders all components inside the root
   *
   * @param state Workspace state
   * @param assetManager Asset manager
   * @param actionExecutor Action executor
   */
  void render(WorkspaceState &state, AssetManager &assetManager,
              ActionExecutor &actionExecutor);

  /**
   * @brief Render scene view
   *
   * @param state Workspace state
   * @param actionExecutor Action executor
   * @param sceneTexture Scene texture
   * @param editorSimulator Editor simulator
   *
   * @retval true Entity is clicked
   * @retval false Entity is not clicked
   */
  bool renderSceneView(WorkspaceState &state, ActionExecutor &actionExecutor,
                       rhi::TextureHandle sceneTexture,
                       SceneSimulator &editorSimulator);

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
  Toolbar mToolbar;
  MainMenu mMainMenu;
  SceneGizmos mSceneGizmos;
};

} // namespace quoll::editor
