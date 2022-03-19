#pragma once

#include "liquid/scene/Scene.h"
#include "liquid/entity/EntityContext.h"
#include "../editor-scene/SceneManager.h"
#include "../editor-scene/EditorCamera.h"

namespace liquidator {

class SceneHierarchyPanel {
  using NodeClickHandler = std::function<void(liquid::SceneNode *)>;

public:
  /**
   * @brief Create scene hierarchy panel
   *
   * @param entityContext Entity entityContext
   */
  SceneHierarchyPanel(liquid::EntityContext &entityContext);

  /**
   * @brief Render the UI
   *
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);

  /**
   * @brief Set node click handler
   *
   * @param handler Node click handler
   */
  void setNodeClickHandler(const NodeClickHandler &handler);

private:
  /**
   * @brief Render scene node as tree node
   *
   * @param node Scene node
   * @param flags Flags
   * @param sceneManager Scene manager
   */
  void renderNode(liquid::SceneNode *node, int flags,
                  SceneManager &sceneManager);

private:
  /**
   * @brief Handle node deletion
   *
   * @param node Scene node
   */
  void handleDelete(liquid::SceneNode *node);

  /**
   * @brief Handle moving to node
   *
   * @param node Scene node
   * @param camera Editor camera
   */
  void handleMoveToNode(liquid::SceneNode *node, EditorCamera &camera);

private:
  liquid::EntityContext &mEntityContext;
  NodeClickHandler mNodeClickHandler;
  liquid::SceneNode *mSelectedNode = nullptr;
};

} // namespace liquidator
