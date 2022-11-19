#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "ConfirmationDialog.h"
#include "SceneHierarchyPanel.h"
#include "Widgets.h"
#include "StyleStack.h"

namespace liquidator {

SceneHierarchyPanel::SceneHierarchyPanel(EntityManager &entityManager)
    : mEntityManager(entityManager) {}

void SceneHierarchyPanel::render(EditorManager &editorManager) {
  static constexpr ImVec2 TreeNodeItemPadding{4.0f, 8.0f};
  static constexpr float TreeNodeIndentSpacing = 10.0f;

  if (auto _ = widgets::Window("Hierarchy")) {
    StyleStack stack;
    stack.pushColor(ImGuiCol_Header,
                    ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered));
    stack.pushStyle(ImGuiStyleVar_ItemSpacing,
                    ImVec2(ImGui::GetStyle().ItemSpacing.x, 0.0f));
    stack.pushStyle(ImGuiStyleVar_FramePadding, TreeNodeItemPadding);
    stack.pushStyle(ImGuiStyleVar_IndentSpacing, TreeNodeIndentSpacing);

    auto &entityDatabase = mEntityManager.getActiveEntityDatabase();

    entityDatabase.iterateEntities<liquid::LocalTransform>(
        [this, &editorManager, &entityDatabase](auto entity,
                                                const auto &transform) {
          if (entityDatabase.has<liquid::Parent>(entity)) {
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
      mEntityManager.getActiveEntityDatabase().has<liquid::Name>(entity)
          ? mEntityManager.getActiveEntityDatabase()
                .get<liquid::Name>(entity)
                .name
          : "Entity #" + std::to_string(entity);

  bool isLeaf =
      !mEntityManager.getActiveEntityDatabase().has<liquid::Children>(entity);

  int treeNodeFlags = flags;
  if (isLeaf) {
    treeNodeFlags |=
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
  }

  if (mSelectedEntity == entity) {
    treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
  }

  treeNodeFlags |= ImGuiTreeNodeFlags_FramePadding;

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
      StyleStack stack;
      // TODO: Get default style instead
      // of manually passing it
      stack.pushStyle(ImGuiStyleVar_ItemSpacing,
                      ImVec2(ImGui::GetStyle().ItemSpacing.x,
                             ImGui::GetStyle().ItemSpacing.x));

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
    if (mEntityManager.getActiveEntityDatabase().has<liquid::Children>(
            entity)) {
      for (auto childEntity : mEntityManager.getActiveEntityDatabase()
                                  .get<liquid::Children>(entity)
                                  .children) {
        renderEntity(childEntity, 0, editorManager);
      }
    }

    ImGui::TreePop();
  }
}

} // namespace liquidator
