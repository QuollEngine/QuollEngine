#pragma once

#include "liquid/animation/AnimationSystem.h"
#include "liquid/physics/PhysicsSystem.h"
#include "liquid/renderer/Renderer.h"

#include "liquidator/editor-scene/EditorManager.h"
#include "Toolbar.h"
#include "SceneHierarchyPanel.h"
#include "EntityPanel.h"
#include "EditorGridPanel.h"
#include "StatusBar.h"
#include "AssetBrowser.h"
#include "Layout.h"
#include "IconRegistry.h"
#include "EnvironmentPanel.h"
#include "SceneView.h"
#include "SceneGizmos.h"
#include "MainMenu.h"
#include "ShortcutsManager.h"

#include "liquidator/actions/ActionExecutor.h"

namespace liquid::editor {

/**
 * @brief Root UI entry point
 *
 * Renders all the UI in the scene
 */
class UIRoot {
public:
  /**
   * @brief Create UI Root
   *
   * @param actionExecutor Action executor
   * @param assetLoader Asset loader
   */
  UIRoot(ActionExecutor &actionExecutor, AssetLoader &assetLoader);

  /**
   * @brief Render UI Root
   *
   * Renders all components inside the root
   *
   * @param state Workspace state
   * @param editorManager Editor manager
   * @param assetManager Asset manager
   */
  void render(WorkspaceState &state, EditorManager &editorManager,
              AssetManager &assetManager);

  /**
   * @brief Render scene view
   *
   * @param state Workspace state
   * @param sceneTexture Scene texture
   * @param editorCamera Editor camera
   * @retval true Entity is clicked
   * @retval false Entity is not clicked
   */
  bool renderSceneView(WorkspaceState &state, rhi::TextureHandle sceneTexture,
                       EditorCamera &editorCamera);

  /**
   * @brief Get icon registry
   *
   * @return Icon registry
   */
  inline IconRegistry &getIconRegistry() { return mIconRegistry; }

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
   * @param eventSystem Event system
   */
  void processShortcuts(EventSystem &eventSystem);

private:
  ActionExecutor mActionExecutor;
  SceneHierarchyPanel mSceneHierarchyPanel;
  EntityPanel mEntityPanel;
  EnvironmentPanel mEnvironmentPanel;
  EditorGridPanel mEditorCameraPanel;
  AssetBrowser mAssetBrowser;
  StatusBar mStatusBar;
  Layout mLayout;
  IconRegistry mIconRegistry;
  Toolbar mToolbar;
  MainMenu mMainMenu;
  SceneGizmos mSceneGizmos;
  ShortcutsManager mShortcutsManager;
};

} // namespace liquid::editor
