#pragma once

#include "quoll/animation/AnimationSystem.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/renderer/Renderer.h"
#include "quoll/scene/CameraAspectRatioUpdater.h"
#include "quoll/ui/UICanvasUpdater.h"

#include "Toolbar.h"
#include "SceneHierarchyPanel.h"
#include "EntityPanel.h"
#include "EditorGridPanel.h"
#include "StatusBar.h"
#include "AssetBrowser.h"
#include "IconRegistry.h"
#include "SceneView.h"
#include "SceneGizmos.h"
#include "MainMenu.h"
#include "Inspector.h"

#include "quoll/editor/workspace/WorkspaceContext.h"
#include "quoll/editor/core/EditorSimulator.h"

namespace quoll::editor {

/**
 * @brief Root UI entry point
 *
 * Renders all the UI in the scene
 */
class UIRoot {
public:
  /**
   * @brief Create UI root
   *
   * @param assetManager Asset manager
   */
  UIRoot(AssetManager &assetManager);

  /**
   * @brief Render UI Root
   *
   * Renders all components inside the root
   *
   * @param context Workspace context
   */
  void render(WorkspaceContext &context);

  /**
   * @brief Render scene view
   *
   * @param context Workspace context
   * @param sceneTexture Scene texture
   * @param editorSimulator Editor simulator
   *
   * @retval true Entity is clicked
   * @retval false Entity is not clicked
   */
  bool renderSceneView(WorkspaceContext &context,
                       rhi::TextureHandle sceneTexture,
                       EditorSimulator &editorSimulator);

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
