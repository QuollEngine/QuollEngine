#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/audio/AudioSource.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/renderer/Mesh.h"
#include "quoll/renderer/SkinnedMesh.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/EnvironmentSkybox.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/Sprite.h"
#include "quoll/skeleton/Skeleton.h"
#include "quoll/text/Text.h"
#include "quoll/editor/actions/DeleteEntityAction.h"
#include "quoll/editor/actions/EditorCameraActions.h"
#include "quoll/editor/actions/EntityRelationActions.h"
#include "quoll/editor/ui/ConfirmationDialog.h"
#include "quoll/editor/ui/FontAwesome.h"
#include "quoll/editor/ui/StyleStack.h"
#include "quoll/editor/ui/Theme.h"
#include "quoll/editor/ui/Widgets.h"
#include "SceneHierarchyPanel.h"

namespace quoll::editor {

static String getNameAndIcon(const String &name, const char *icon) {
  return String(icon) + "  " + name;
}

static String getNodeName(const String &name, Entity entity,
                          EntityDatabase &entityDatabase) {
  if (entity.has<EnvironmentSkybox>()) {
    return getNameAndIcon(name, fa::Cloud);
  }

  if (entity.has<Camera>()) {
    return getNameAndIcon(name, fa::Video);
  }

  if (entity.has<DirectionalLight>()) {
    return getNameAndIcon(name, fa::Sun);
  }

  if (entity.has<PointLight>()) {
    return getNameAndIcon(name, fa::Lightbulb);
  }

  if (entity.has<Text>()) {
    return getNameAndIcon(name, fa::Font);
  }

  if (entity.has<Skeleton>()) {
    return getNameAndIcon(name, fa::Bone);
  }

  if (entity.has<AudioSource>()) {
    return getNameAndIcon(name, fa::Music);
  }

  if (entity.has<Mesh>()) {
    return getNameAndIcon(name, fa::Cubes);
  }

  if (entity.has<Sprite>()) {
    return getNameAndIcon(name, fa::Image);
  }

  return name;
}

void SceneHierarchyPanel::render(WorkspaceState &state,
                                 ActionExecutor &actionExecutor) {

  f32 paddingY = ImGui::GetStyle().WindowPadding.y;
  StyleStack stack;
  stack.pushStyle(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

  if (auto _ = widgets::Window("Scene")) {
    renderRoot(state, actionExecutor);
  }
}

void SceneHierarchyPanel::renderRoot(WorkspaceState &state,
                                     ActionExecutor &actionExecutor) {
  static constexpr ImVec2 TreeNodeItemPadding{4.0f, 8.0f};
  static constexpr f32 TreeNodeIndentSpacing = 10.0f;

  auto &scene = state.scene;

  int treeNodeFlags =
      ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth |
      ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_DefaultOpen;

  bool open = false;
  String nodeName = "Scene";

  StyleStack stack;
  stack.pushStyle(ImGuiStyleVar_ItemSpacing,
                  ImVec2(Theme::getStyles().itemSpacing.x, 0.0f));
  stack.pushStyle(ImGuiStyleVar_FramePadding, TreeNodeItemPadding);
  stack.pushStyle(ImGuiStyleVar_IndentSpacing, TreeNodeIndentSpacing);

  if (ImGui::TreeNodeEx(nodeName.c_str(), treeNodeFlags)) {
    open = true;
  }

  if (ImGui::BeginDragDropTarget()) {
    if (auto *payload = ImGui::AcceptDragDropPayload("Entity")) {
      auto entity = *static_cast<Entity *>(payload->Data);
      actionExecutor.execute<EntityRemoveParent>(entity);
    }
    ImGui::EndDragDropTarget();
  }

  if (open) {
    u32 index = 0;
    auto query = scene.entityDatabase.query_builder<LocalTransform>()
                     .without<Parent>()
                     .build();

    query.each([&](flecs::entity entity, LocalTransform &transform) {
      index = renderEntity(entity, index + 1, ImGuiTreeNodeFlags_DefaultOpen,
                           state, actionExecutor);
    });

    ImGui::TreePop();
  }
}

u32 SceneHierarchyPanel::renderEntity(Entity entity, u32 index, int flags,
                                      WorkspaceState &state,
                                      ActionExecutor &actionExecutor) {
  u32 innerIndex = index;
  auto &scene = state.scene;

  String name = entity.has<Name>()
                    ? entity.get_ref<Name>()->name
                    : "Entity #" + std::to_string(static_cast<u32>(entity));

  bool isLeaf = !entity.has<Children>();

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
                           Theme::getColor(ThemeColor::Sienna600));
    } else if ((innerIndex % 2) != 0) {
      styleStack.pushColor(ImGuiCol_Header,
                           Theme::getColor(ThemeColor::Charcoal500));
    }

    ImGui::PushID(static_cast<i32>(entity));
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
    StyleStack stack;
    stack.pushStyle(ImGuiStyleVar_WindowPadding,
                    Theme::getStyles().windowPadding);
    stack.pushStyle(ImGuiStyleVar_ItemSpacing, Theme::getStyles().itemSpacing);

    if (auto _ = widgets::ContextMenu()) {
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
    if (entity.has<Children>()) {
      for (auto childEntity : entity.get_ref<Children>()->children) {
        innerIndex =
            renderEntity(childEntity, innerIndex + 1, 0, state, actionExecutor);
      }
    }
    ImGui::TreePop();
  }

  return innerIndex;
}

} // namespace quoll::editor
