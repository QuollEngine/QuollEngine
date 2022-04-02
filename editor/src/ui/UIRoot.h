#pragma once

#include "../editor-scene/SceneManager.h"

#include "MenuBar.h"
#include "SceneHierarchyPanel.h"
#include "EntityPanel.h"
#include "EditorGridPanel.h"
#include "StatusBar.h"
#include "Layout.h"
#include "liquid/animation/AnimationSystem.h"
#include "liquid/physics/PhysicsSystem.h"

namespace liquidator {

class UIRoot {
public:
  /**
   * @brief Create UI Root
   *
   * @param entityContext Entity context
   * @param gltfLoader GLTF loader
   * @param gltfImporter GLTF importer
   */
  UIRoot(liquid::EntityContext &entityContext,
         const liquid::GLTFLoader &gltfLoader, GLTFImporter &importer);

  /**
   * @brief Render UI Root
   *
   * Renders all components inside the root
   *
   * @param sceneManager Scene manager
   * @param animationSystem Animation system
   * @param physicsSystem Physics system
   */
  void render(SceneManager &sceneManager,
              const liquid::AnimationSystem &animationSystem,
              liquid::PhysicsSystem &physicsSystem);

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
  StatusBar mStatusBar;
  Layout mLayout;
};

} // namespace liquidator
