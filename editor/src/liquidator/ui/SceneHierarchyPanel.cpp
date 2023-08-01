#include "liquid/core/Base.h"
#include "liquid/imgui/Imgui.h"

#include "ConfirmationDialog.h"
#include "SceneHierarchyPanel.h"
#include "Widgets.h"
#include "StyleStack.h"
#include "FontAwesome.h"
#include "Theme.h"

#include "liquidator/actions/EditorCameraActions.h"
#include "liquidator/actions/DeleteEntityAction.h"
#include "liquidator/actions/EntityRelationActions.h"

namespace liquid::editor {

void SceneHierarchyPanel::render(WorkspaceState &state,
                                 ActionExecutor &actionExecutor) {
  static constexpr ImVec2 TreeNodeItemPadding{4.0f, 8.0f};
  static constexpr float TreeNodeIndentSpacing = 10.0f;

  float paddingY = ImGui::GetStyle().WindowPadding.y;
  StyleStack stack;
  stack.pushStyle(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

  uint32_t index = 0;

  if (auto _ = widgets::Window("Scene")) {
    auto &scene = state.mode == WorkspaceMode::Simulation
                      ? state.simulationScene
                      : state.scene;

    StyleStack stack;
    stack.pushStyle(ImGuiStyleVar_ItemSpacing,
                    ImVec2(ImGui::GetStyle().ItemSpacing.x, 0.0f));
    stack.pushStyle(ImGuiStyleVar_FramePadding, TreeNodeItemPadding);
    stack.pushStyle(ImGuiStyleVar_IndentSpacing, TreeNodeIndentSpacing);

    for (auto [entity, transform] :
         scene.entityDatabase.view<LocalTransform>()) {
      if (scene.entityDatabase.has<Parent>(entity)) {
        continue;
      }

      index = renderEntity(entity, index + 1, ImGuiTreeNodeFlags_DefaultOpen,
                           state, actionExecutor);
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
    return getNameAndIcon(name, fa::Sun);
  }

  if (entityDatabase.has<PointLight>(entity)) {
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

  if (entityDatabase.has<Sprite>(entity)) {
    return getNameAndIcon(name, fa::Image);
  }

  return name;
}

uint32_t SceneHierarchyPanel::renderEntity(Entity entity, uint32_t index,
                                           int flags, WorkspaceState &state,
                                           ActionExecutor &actionExecutor) {
  uint32_t innerIndex = index;
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  String name =
      scene.entityDatabase.has<Name>(entity)
          ? scene.entityDatabase.get<Name>(entity).name
          : "Entity #" + std::to_string(static_cast<uint32_t>(entity));

  bool isLeaf = !scene.entityDatabase.has<Children>(entity);

  int treeNodeFlags = flags;
  if (isLeaf) {
    treeNodeFlags |=
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
  }

  ConfirmationDialog confirmDeleteSceneNode(
      "Delete entity", "Are you sure you want to delete node \"" + name + "\"?",
      "Delete");

  treeNodeFlags |= ImGuiTreeNodeFlags_FramePadding |
                   ImGuiTreeNodeFlags_SpanFullWidth |
                   ImGuiTreeNodeFlags_Selected;

  bool open = false;
  auto nodeName = getNodeName(name, entity, scene.entityDatabase);

  {
    StyleStack styleStack;
    if (state.selectedEntity == entity) {
      styleStack.pushFont(Theme::getBoldFont());
      styleStack.pushColor(ImGuiCol_Header,
                           Theme::getColor(ThemeColor::Primary100));
    } else if ((innerIndex % 2) == 0) {
      styleStack.pushColor(ImGuiCol_Header,
                           Theme::getColor(ThemeColor::Neutral200));
    }

    ImGui::PushID(static_cast<int32_t>(entity));
    if (ImGui::TreeNodeEx(nodeName.c_str(), treeNodeFlags)) {
      open = !isLeaf;
    }
    ImGui::PopID();
  }

  if (ImGui::BeginDragDropSource()) {
    ImGui::SetDragDropPayload("Entity", &entity, sizeof(Entity));
    ImGui::Text("%s", nodeName.c_str());
    ImGui::EndDragDropSource();
  }

  if (ImGui::BeginDragDropTarget()) {
    if (auto *payload = ImGui::AcceptDragDropPayload("Entity")) {
      auto child = *static_cast<Entity *>(payload->Data);
      actionExecutor.execute<EntitySetParent>(child, entity);
    }
    ImGui::EndDragDropTarget();
  }

  if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
    mRightClickedEntity = entity;
  } else if (ImGui::IsItemClicked()) {
    state.selectedEntity = entity;
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
        actionExecutor.execute<MoveCameraToEntity>(entity);
      }

      if (ImGui::MenuItem("Delete")) {
        confirmDeleteSceneNode.show();
      }
    }
    confirmDeleteSceneNode.render();
    if (confirmDeleteSceneNode.isConfirmed()) {
      actionExecutor.execute<DeleteEntity>(entity);
    }
  }

  if (open) {
    if (scene.entityDatabase.has<Children>(entity)) {
      for (auto childEntity :
           scene.entityDatabase.get<Children>(entity).children) {
        innerIndex =
            renderEntity(childEntity, innerIndex + 1, 0, state, actionExecutor);
      }
    }
    ImGui::TreePop();
  }

  return innerIndex;
}

} // namespace liquid::editor
