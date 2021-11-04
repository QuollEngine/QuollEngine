#include "SceneHierarchyPanel.h"
#include <imgui.h>

namespace liquidator {

SceneHierarchyPanel::SceneHierarchyPanel(liquid::EntityContext &context_)
    : context(context_) {}

void SceneHierarchyPanel::render(liquid::Scene *scene) {
  ImGui::Begin("Scene");
  for (auto *child : scene->getRootNode()->getChildren()) {
    renderNode(child, ImGuiTreeNodeFlags_DefaultOpen);
  }

  ImGui::End();
}

void SceneHierarchyPanel::renderNode(liquid::SceneNode *node, int flags) {
  liquid::String name;
  if (context.hasComponent<liquid::NameComponent>(node->getEntity())) {
    name = context.getComponent<liquid::NameComponent>(node->getEntity()).name;
  } else {
    name = "Entity " + std::to_string(node->getEntity());
  }

  bool isLeaf = node->getChildren().empty();

  int treeNodeFlags = flags;
  if (isLeaf) {
    treeNodeFlags |=
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
  }

  bool open = false;
  if (ImGui::TreeNodeEx(name.c_str(), treeNodeFlags)) {
    open = !isLeaf;
  }

  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::MenuItem("Delete")) {
      handleDelete(node);
    }
    ImGui::EndPopup();
  }

  if (open) {
    for (auto *child : node->getChildren()) {
      renderNode(child, 0);
    }
    ImGui::TreePop();
  }
}

void SceneHierarchyPanel::handleDelete(liquid::SceneNode *node) {
  auto entity = node->getEntity();
  auto *parent = node->getParent();

  if (parent) {
    parent->removeChild(node);
  }

  context.deleteEntity(entity);
}

} // namespace liquidator
