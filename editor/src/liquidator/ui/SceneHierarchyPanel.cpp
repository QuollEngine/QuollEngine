#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "ConfirmationDialog.h"
#include "SceneHierarchyPanel.h"
#include "Widgets.h"
#include "StyleStack.h"
#include "FontAwesome.h"
#include "Theme.h"

#include "liquidator/actions/MoveCameraToEntityAction.h"

namespace liquid::editor {

SceneHierarchyPanel::SceneHierarchyPanel(EntityManager &entityManager)
    : mEntityManager(entityManager) {}

void SceneHierarchyPanel::render(WorkspaceState &state,
                                 ActionExecutor &actionExecutor) {
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

    for (auto [entity, transform] : entityDatabase.view<LocalTransform>()) {
      if (entityDatabase.has<Parent>(entity)) {
        continue;
      }

      renderEntity(entity, ImGuiTreeNodeFlags_DefaultOpen, state,
                   actionExecutor);
    }
  }
}

static String getNameAndIcon(const String &name, const char *icon) {
  return String(icon) + "  " + name;
}

static String getNodeName(const String &name, Entity entity,
                          EntityDatabase &entityDatabase) {

  if (entityDatabase.has<Camera>(entity)) {
    return getNameAndIcon(name, fa::Video);
  }

  if (entityDatabase.has<DirectionalLight>(entity)) {
    return getNameAndIcon(name, fa::Lightbulb);
  }

  if (entityDatabase.has<Text>(entity)) {
    return getNameAndIcon(name, fa::Font);
  }

  if (entityDatabase.has<Skeleton>(entity)) {
    return getNameAndIcon(name, fa::Bone);
  }

  if (entityDatabase.has<AudioSource>(entity)) {
    return getNameAndIcon(name, fa::Music);
  }

  if (entityDatabase.has<Mesh>(entity)) {
    return getNameAndIcon(name, fa::Cubes);
  }

  return name;
}

void SceneHierarchyPanel::renderEntity(Entity entity, int flags,
                                       WorkspaceState &state,
                                       ActionExecutor &actionExecutor) {
  auto &entityDatabase = mEntityManager.getActiveEntityDatabase();
  String name =
      entityDatabase.has<Name>(entity)
          ? mEntityManager.getActiveEntityDatabase().get<Name>(entity).name
          : "Entity #" + std::to_string(static_cast<uint32_t>(entity));

  bool isLeaf = !mEntityManager.getActiveEntityDatabase().has<Children>(entity);

  int treeNodeFlags = flags;
  if (isLeaf) {
    treeNodeFlags |=
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
  }

  ConfirmationDialog confirmDeleteSceneNode(
      "Delete entity", "Are you sure you want to delete node \"" + name + "\"?",
      "Delete");

  StyleStack fontStack;
  if (state.selectedEntity == entity) {
    treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
  }

  treeNodeFlags |= ImGuiTreeNodeFlags_FramePadding;

  bool open = false;

  if (state.selectedEntity == entity && isLeaf) {
    fontStack.pushFont(Theme::getBoldFont());
  }

  if (ImGui::TreeNodeEx(getNodeName(name, entity, entityDatabase).c_str(),
                        treeNodeFlags)) {
    open = !isLeaf;

    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
      mRightClickedEntity = entity;
    } else if (ImGui::IsItemClicked()) {
      state.selectedEntity = entity;
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
        actionExecutor.execute(MoveCameraToEntityAction, entity);
      }

      if (ImGui::MenuItem("Delete")) {
        confirmDeleteSceneNode.show();
      }
    }
    confirmDeleteSceneNode.render();
    if (confirmDeleteSceneNode.isConfirmed()) {
      if (entity == state.selectedEntity) {
        state.selectedEntity = Entity::Null;
      }

      mEntityManager.deleteEntity(entity);
    }
  }

  if (open) {
    if (mEntityManager.getActiveEntityDatabase().has<Children>(entity)) {
      for (auto childEntity : mEntityManager.getActiveEntityDatabase()
                                  .get<Children>(entity)
                                  .children) {
        renderEntity(childEntity, 0, state, actionExecutor);
      }
    }

    ImGui::TreePop();
  }
}

} // namespace liquid::editor
