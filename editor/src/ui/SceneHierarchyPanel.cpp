#include "liquid/core/Base.h"
#include "SceneHierarchyPanel.h"
#include "ConfirmationDialog.h"

#include <imgui.h>

namespace liquidator {

SceneHierarchyPanel::SceneHierarchyPanel(liquid::EntityContext &entityContext)
    : mEntityContext(entityContext) {}

void SceneHierarchyPanel::render(SceneManager &sceneManager) {
  ImGui::Begin("Hierarchy");

  mEntityContext.iterateEntities<liquid::TransformComponent>(
      [this, &sceneManager](auto entity, const auto &transform) {
        if (mEntityContext.hasComponent<liquid::ParentComponent>(entity)) {
          return;
        }

        renderEntity(entity, ImGuiTreeNodeFlags_DefaultOpen, sceneManager);
      });

  ImGui::End();
}

void SceneHierarchyPanel::setEntityClickHandler(
    const EntityClickHandler &handler) {
  mEntityClickHandler = handler;
}

void SceneHierarchyPanel::renderEntity(liquid::Entity entity, int flags,
                                       SceneManager &sceneManager) {
  liquid::String name =
      mEntityContext.hasComponent<liquid::NameComponent>(entity)
          ? mEntityContext.getComponent<liquid::NameComponent>(entity).name
          : "Entity #" + std::to_string(entity);

  bool isLeaf = !mEntityContext.hasComponent<liquid::ChildrenComponent>(entity);

  int treeNodeFlags = flags;
  if (isLeaf) {
    treeNodeFlags |=
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
  }

  if (mSelectedEntity == entity) {
    treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
  }

  bool open = false;
  if (ImGui::TreeNodeEx(name.c_str(), treeNodeFlags)) {
    open = !isLeaf;
    if (ImGui::IsItemClicked()) {
      mEntityClickHandler(entity);
      mSelectedEntity = entity;
    }
  }

  ConfirmationDialog confirmDeleteSceneNode(
      "Delete entity ",
      "Are you sure you want to delete node \"" + name + "\"?",
      [this, entity](SceneManager &sceneManager) {
        sceneManager.getEntityManager().deleteEntity(entity);
      },
      "Delete");

  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::MenuItem("Go to view")) {
      sceneManager.moveCameraToEntity(entity);
    }

    if (ImGui::MenuItem("Delete")) {
      confirmDeleteSceneNode.show();
    }

    ImGui::EndPopup();
  }

  confirmDeleteSceneNode.render(sceneManager);

  if (open) {
    if (mEntityContext.hasComponent<liquid::ChildrenComponent>(entity)) {
      for (auto childEntity :
           mEntityContext.getComponent<liquid::ChildrenComponent>(entity)
               .children) {
        renderEntity(childEntity, 0, sceneManager);
      }
    }

    ImGui::TreePop();
  }
}

} // namespace liquidator
