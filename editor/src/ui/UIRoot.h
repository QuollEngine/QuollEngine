#pragma once

#include "../editor-scene/SceneManager.h"

#include "MenuBar.h"
#include "SceneHierarchyPanel.h"
#include "EntityPanel.h"
#include "EditorGridPanel.h"
#include "StatusBar.h"

namespace liquidator {

class UIRoot {
public:
  /**
   * @brief Create UI Root
   *
   * @param context Entity context
   * @param gltfLoader GLTF loader
   */
  UIRoot(liquid::EntityContext &context,
         const liquid::TinyGLTFLoader &gltfLoader);

  /**
   * @brief Render UI Root
   *
   * Renders all components inside the root
   *
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);

private:
  /**
   * @brief Handle node click from scene hierarchy panel
   *
   * @param node Clicked node item
   */
  void handleNodeClick(liquid::SceneNode *node);

private:
  MenuBar menuBar;
  SceneHierarchyPanel sceneHierarchyPanel;
  EntityPanel entityPanel;
  EditorGridPanel editorCameraPanel;
  StatusBar statusBar;
};

} // namespace liquidator
