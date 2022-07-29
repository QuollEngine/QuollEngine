#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "ConfirmationDialog.h"
#include "SceneHierarchyPanel.h"
#include "Widgets.h"

namespace liquidator {

SceneHierarchyPanel::SceneHierarchyPanel(EntityManager &entityManager)
    : mEntityManager(entityManager) {}

void SceneHierarchyPanel::render(EditorManager &editorManager) {
  if (widgets::Window::begin("Hierarchy")) {
    mEntityManager.getActiveEntityDatabase()
        .iterateEntities<liquid::LocalTransformComponent>(
            [this, &editorManager](auto entity, const auto &transform) {
              renderEntity(entity, ImGuiTreeNodeFlags_DefaultOpen,
                           editorManager);
            });
  }

  widgets::Window::end();
}

void SceneHierarchyPanel::setEntityClickHandler(
    const EntityClickHandler &handler) {
  mEntityClickHandler = handler;
}

void SceneHierarchyPanel::renderEntity(liquid::Entity entity, int flags,
                                       EditorManager &editorManager) {
  liquid::String name = mEntityManager.getActiveEntityDatabase()
                                .hasComponent<liquid::NameComponent>(entity)
                            ? mEntityManager.getActiveEntityDatabase()
                                  .getComponent<liquid::NameComponent>(entity)
                                  .name
                            : "Entity #" + std::to_string(entity);

  bool isLeaf = !mEntityManager.getActiveEntityDatabase()
                     .hasComponent<liquid::ChildrenComponent>(entity);

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
      [this, entity](EditorManager &editorManager) {
        mEntityManager.deleteEntity(entity);
      },
      "Delete");

  if (widgets::ContextMenu::begin()) {
    if (ImGui::MenuItem("Go to view")) {
      editorManager.moveCameraToEntity(entity);
    }

    if (ImGui::MenuItem("Delete")) {
      confirmDeleteSceneNode.show();
    }

    widgets::ContextMenu::end();
  }

  confirmDeleteSceneNode.render(editorManager);

  if (open) {
    if (mEntityManager.getActiveEntityDatabase()
            .hasComponent<liquid::ChildrenComponent>(entity)) {
      for (auto childEntity :
           mEntityManager.getActiveEntityDatabase()
               .getComponent<liquid::ChildrenComponent>(entity)
               .children) {
        renderEntity(childEntity, 0, editorManager);
      }
    }

    ImGui::TreePop();
  }
}

} // namespace liquidator
