#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "ConfirmationDialog.h"
#include "SceneHierarchyPanel.h"
#include "Widgets.h"

namespace liquidator {

SceneHierarchyPanel::SceneHierarchyPanel(EntityManager &entityManager)
    : mEntityManager(entityManager) {}

void SceneHierarchyPanel::render(EditorManager &editorManager) {
  if (auto _ = widgets::Window("Hierarchy")) {
    auto &entityDatabase = mEntityManager.getActiveEntityDatabase();

    entityDatabase.iterateEntities<liquid::LocalTransformComponent>(
        [this, &editorManager, &entityDatabase](auto entity,
                                                const auto &transform) {
          if (entityDatabase.has<liquid::ParentComponent>(entity)) {
            return;
          }

          renderEntity(entity, ImGuiTreeNodeFlags_DefaultOpen, editorManager);
        });
  }
}

void SceneHierarchyPanel::setSelectedEntity(liquid::Entity entity) {
  mSelectedEntity = entity;
}

void SceneHierarchyPanel::renderEntity(liquid::Entity entity, int flags,
                                       EditorManager &editorManager) {
  liquid::String name =
      mEntityManager.getActiveEntityDatabase().has<liquid::NameComponent>(
          entity)
          ? mEntityManager.getActiveEntityDatabase()
                .get<liquid::NameComponent>(entity)
                .name
          : "Entity #" + std::to_string(entity);

  bool isLeaf =
      !mEntityManager.getActiveEntityDatabase().has<liquid::ChildrenComponent>(
          entity);

  int treeNodeFlags = flags;
  if (isLeaf) {
    treeNodeFlags |=
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
  }

  if (mSelectedEntity == entity) {
    treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
  }

  ConfirmationDialog confirmDeleteSceneNode(
      "Delete entity", "Are you sure you want to delete node \"" + name + "\"?",
      "Delete");

  bool open = false;
  if (ImGui::TreeNodeEx(name.c_str(), treeNodeFlags)) {
    open = !isLeaf;

    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
      mRightClickedEntity = entity;
    } else if (ImGui::IsItemClicked()) {
      mSelectedEntity = entity;
    }
  }

  if (mRightClickedEntity == entity) {
    if (auto _ = widgets::ContextMenu()) {
      if (ImGui::MenuItem("Go to view")) {
        editorManager.moveCameraToEntity(entity);
      }

      if (ImGui::MenuItem("Delete")) {
        confirmDeleteSceneNode.show();
      }
    }
    confirmDeleteSceneNode.render();
    if (confirmDeleteSceneNode.isConfirmed()) {
      mEntityManager.deleteEntity(entity);
    }
  }

  if (open) {
    if (mEntityManager.getActiveEntityDatabase().has<liquid::ChildrenComponent>(
            entity)) {
      for (auto childEntity : mEntityManager.getActiveEntityDatabase()
                                  .get<liquid::ChildrenComponent>(entity)
                                  .children) {
        renderEntity(childEntity, 0, editorManager);
      }
    }

    ImGui::TreePop();
  }
}

} // namespace liquidator
