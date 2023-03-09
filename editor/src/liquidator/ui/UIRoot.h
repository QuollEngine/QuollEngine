#pragma once

#include "liquid/animation/AnimationSystem.h"
#include "liquid/physics/PhysicsSystem.h"
#include "liquid/renderer/Renderer.h"

#include "liquidator/editor-scene/EditorManager.h"
#include "MenuBar.h"
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
   * @param entityManager Entity manager
   * @param assetLoader Asset loader
   */
  UIRoot(EntityManager &entityManager, AssetLoader &assetLoader);

  /**
   * @brief Render UI Root
   *
   * Renders all components inside the root
   *
   * @param state Workspace state
   * @param editorManager Editor manager
   * @param renderer Renderer
   * @param assetManager Asset manager
   * @param physicsSystem Physics system
   * @param entityManager Entity manager
   */
  void render(WorkspaceState &state, EditorManager &editorManager,
              Renderer &renderer, AssetManager &assetManager,
              PhysicsSystem &physicsSystem, EntityManager &entityManager);

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

private:
  MenuBar mMenuBar;
  SceneHierarchyPanel mSceneHierarchyPanel;
  EntityPanel mEntityPanel;
  EnvironmentPanel mEnvironmentPanel;
  EditorGridPanel mEditorCameraPanel;
  AssetBrowser mAssetBrowser;
  StatusBar mStatusBar;
  Layout mLayout;
  IconRegistry mIconRegistry;
};

} // namespace liquid::editor
