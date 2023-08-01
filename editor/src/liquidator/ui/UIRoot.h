#pragma once

#include "liquid/animation/AnimationSystem.h"
#include "liquid/physics/PhysicsSystem.h"
#include "liquid/renderer/Renderer.h"

#include "Toolbar.h"
#include "SceneHierarchyPanel.h"
#include "EntityPanel.h"
#include "EditorGridPanel.h"
#include "StatusBar.h"
#include "AssetBrowser.h"
#include "IconRegistry.h"
#include "EnvironmentPanel.h"
#include "SceneView.h"
#include "SceneGizmos.h"
#include "MainMenu.h"
#include "ShortcutsManager.h"
#include "Inspector.h"

#include "liquidator/workspace/WorkspaceContext.h"

namespace liquid::editor {

/**
 * @brief Root UI entry point
 *
 * Renders all the UI in the scene
 */
class UIRoot {
public:
  /**
   * @brief Create UI root
   */
  UIRoot();

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
   * @param editorCamera Editor camera
   * @retval true Entity is clicked
   * @retval false Entity is not clicked
   */
  bool renderSceneView(WorkspaceContext &context,
                       rhi::TextureHandle sceneTexture,
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

  /**
   * @brief Process shortcuts
   *
   * @param context Workspace context
   * @param eventSystem Event system
   */
  void processShortcuts(WorkspaceContext &context, EventSystem &eventSystem);

private:
  SceneHierarchyPanel mSceneHierarchyPanel;
  EntityPanel mEntityPanel;
  EnvironmentPanel mEnvironmentPanel;
  EditorGridPanel mEditorCameraPanel;
  AssetBrowser mAssetBrowser;
  Inspector mInspector;

  // Unclosable UI
  StatusBar mStatusBar;
  Toolbar mToolbar;
  MainMenu mMainMenu;
  SceneGizmos mSceneGizmos;
  ShortcutsManager mShortcutsManager;
};

} // namespace liquid::editor
