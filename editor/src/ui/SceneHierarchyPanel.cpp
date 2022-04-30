#include "liquid/core/Base.h"
#include "SceneHierarchyPanel.h"
#include "ConfirmationDialog.h"

#include <imgui.h>

namespace liquidator {

SceneHierarchyPanel::SceneHierarchyPanel(liquid::EntityContext &entityContext)
    : mEntityContext(entityContext) {}

void SceneHierarchyPanel::render(SceneManager &sceneManager) {
  ImGui::Begin("Hierarchy");
  for (auto *child :
       sceneManager.getActiveScene()->getRootNode()->getChildren()) {
    renderNode(child, ImGuiTreeNodeFlags_DefaultOpen, sceneManager);
  }

  ImGui::End();
}

void SceneHierarchyPanel::setNodeClickHandler(const NodeClickHandler &handler) {
  mNodeClickHandler = handler;
}

void SceneHierarchyPanel::renderNode(liquid::SceneNode *node, int flags,
                                     SceneManager &sceneManager) {
  liquid::String name;
  if (mEntityContext.hasComponent<liquid::NameComponent>(node->getEntity())) {
    name = mEntityContext.getComponent<liquid::NameComponent>(node->getEntity())
               .name;
  } else {
    name = "Entity " + std::to_string(node->getEntity());
  }

  bool isLeaf = node->getChildren().empty();

  int treeNodeFlags = flags;
  if (isLeaf) {
    treeNodeFlags |=
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
  }

  if (mSelectedNode == node) {
    treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
  }

  bool open = false;
  if (ImGui::TreeNodeEx(name.c_str(), treeNodeFlags)) {
    open = !isLeaf;
    if (ImGui::IsItemClicked()) {
      mNodeClickHandler(node);
      mSelectedNode = node;
    }
  }

  ConfirmationDialog confirmDeleteSceneNode(
      "Delete scene node#" + std::to_string(node->getEntity()),
      "Are you sure you want to delete node \"" + name + "\"?",
      [this, node](SceneManager &sceneManager) {
        handleDelete(node, sceneManager.getEntityManager());
      },
      "Delete");

  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::MenuItem("Go to view")) {
      handleMoveToNode(node, sceneManager);
    }

    if (ImGui::MenuItem("Delete")) {
      confirmDeleteSceneNode.show();
    }

    ImGui::EndPopup();
  }

  confirmDeleteSceneNode.render(sceneManager);

  if (open) {
    for (auto *child : node->getChildren()) {
      renderNode(child, 0, sceneManager);
    }
    ImGui::TreePop();
  }
}

void SceneHierarchyPanel::handleDelete(liquid::SceneNode *node,
                                       EntityManager &entityManager) {
  auto entity = node->getEntity();
  auto *parent = node->getParent();

  if (parent) {
    parent->removeChild(node);
  }

  entityManager.deleteEntity(entity);
}

void SceneHierarchyPanel::handleMoveToNode(liquid::SceneNode *node,
                                           SceneManager &sceneManager) {
  sceneManager.moveCameraToEntity(node->getEntity());
}

} // namespace liquidator
