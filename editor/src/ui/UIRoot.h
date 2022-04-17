#pragma once

#include "../editor-scene/SceneManager.h"

#include "MenuBar.h"
#include "SceneHierarchyPanel.h"
#include "EntityPanel.h"
#include "EditorGridPanel.h"
#include "StatusBar.h"
#include "AssetBrowser.h"
#include "Layout.h"
#include "IconRegistry.h"
#include "liquid/animation/AnimationSystem.h"
#include "liquid/physics/PhysicsSystem.h"

namespace liquidator {

class UIRoot {
public:
  /**
   * @brief Create UI Root
   *
   * @param entityContext Entity context
   * @param gltfImporter GLTF importer
   */
  UIRoot(liquid::EntityContext &entityContext, GLTFImporter &importer);

  /**
   * @brief Render UI Root
   *
   * Renders all components inside the root
   *
   * @param sceneManager Scene manager
   * @param assetManager Asset manager
   * @param physicsSystem Physics system
   */
  void render(SceneManager &sceneManager, liquid::AssetManager &assetManager,
              liquid::PhysicsSystem &physicsSystem);

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

private:
  /**
   * @brief Handle node click from scene hierarchy panel
   *
   * @param node Clicked node item
   */
  void handleNodeClick(liquid::SceneNode *node);

private:
  MenuBar mMenuBar;
  SceneHierarchyPanel mSceneHierarchyPanel;
  EntityPanel mEntityPanel;
  EditorGridPanel mEditorCameraPanel;
  AssetBrowser mAssetBrowser;
  StatusBar mStatusBar;
  Layout mLayout;
  IconRegistry mIconRegistry;
};

} // namespace liquidator
