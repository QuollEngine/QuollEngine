#include "SceneHierarchy.h"
#include <imgui.h>

using liquid::Scene;
using liquid::SceneNode;
using liquid::SharedPtr;
using liquid::String;

SceneHierarchy::SceneHierarchy(liquid::EntityContext &context_)
    : context(context_) {}

void SceneHierarchy::setScene(const SharedPtr<Scene> &scene_) {
  scene = scene_;
}

void SceneHierarchy::render() {
  ImGui::Begin("Scene", &panelOpen);

  SceneNode *node = scene->getRootNode();
  renderNode(node, ImGuiTreeNodeFlags_DefaultOpen);

  ImGui::End();
}

void SceneHierarchy::renderNode(SceneNode *node, int flags) {
  String name;
  if (context.hasComponent<liquid::NameComponent>(node->getEntity())) {
    name = context.getComponent<liquid::NameComponent>(node->getEntity()).name;
  } else {
    name = "Entity " + std::to_string(node->getEntity());
  }
  bool isLeaf = node->getChildren().size() == 0;

  if (isLeaf) {
    ImGui::TreeNodeEx(name.c_str(), flags | ImGuiTreeNodeFlags_Leaf |
                                        ImGuiTreeNodeFlags_NoTreePushOnOpen);
    if (ImGui::IsItemClicked() && clickHandler) {
      clickHandler(node->getEntity());
    }

  } else if (ImGui::TreeNodeEx(name.c_str(), flags)) {
    for (auto &child : node->getChildren()) {
      renderNode(child, 0);
    }

    ImGui::TreePop();
  }
}
