#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll/skeleton/JointAttachment.h"
#include "quoll/skeleton/Skeleton.h"
#include "ImGuizmo.h"
#include "SceneGizmos.h"

namespace quoll::editor {

static LocalTransform
calculateLocalTransformFromWorld(WorkspaceState &state, Entity entity,
                                 const glm::mat4 &worldTransform) {
  auto &scene = state.scene;
  auto &entityDatabase = scene.entityDatabase;

  glm::vec3 localScale;
  glm::quat localRotation;
  glm::vec3 localPosition;

  glm::vec3 noopSkew;
  glm::vec4 noopPerspective;

  if (!entity.has<Parent>()) {
    glm::decompose(worldTransform, localScale, localRotation, localPosition,
                   noopSkew, noopPerspective);
    return LocalTransform{localPosition, localRotation, localScale};
  }

  auto parent = entity.get_ref<Parent>()->parent;

  QuollAssert(parent.is_valid() && parent.has<WorldTransform>(),
              "Parent entity does not exist or has no transform");

  const auto &parentWorld = parent.get_ref<WorldTransform>()->worldTransform;

  i16 jointId = -1;
  if (entity.has<JointAttachment>() && parent.has<Skeleton>()) {
    jointId = entity.get_ref<JointAttachment>()->joint;
  }

  if (jointId >= 0 && jointId < std::numeric_limits<u8>::max()) {
    const auto &jointTransform =
        parent.get_ref<Skeleton>()->jointWorldTransforms.at(jointId);

    auto localTransform =
        glm::inverse(parentWorld * jointTransform) * worldTransform;

    glm::decompose(localTransform, localScale, localRotation, localPosition,
                   noopSkew, noopPerspective);
  } else {
    auto localTransform = glm::inverse(parentWorld) * worldTransform;

    glm::decompose(localTransform, localScale, localRotation, localPosition,
                   noopSkew, noopPerspective);
  }

  return LocalTransform{localPosition, localRotation, localScale};
}

static ImGuizmo::OPERATION
getImguizmoOperation(TransformOperation transformOperation) {
  switch (transformOperation) {
  case TransformOperation::Scale:
    return ImGuizmo::SCALE;
  case TransformOperation::Rotate:
    return ImGuizmo::ROTATE;
  case TransformOperation::Move:
  default:
    return ImGuizmo::TRANSLATE;
  }
}

bool SceneGizmos::render(WorkspaceState &state,
                         ActionExecutor &actionExecutor) {
  auto &scene = state.scene;

  const auto &pos = ImGui::GetItemRectMin();
  const auto &size = ImGui::GetItemRectSize();

  ImGuizmo::SetDrawlist();
  ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

  auto camera = state.activeCamera.get_ref<Camera>();
  auto gizmoPerspective = camera->projectionMatrix;

  auto selected = state.selectedEntity;
  auto world = selected.get_ref<WorldTransform>();
  auto worldTransform = world->worldTransform;
  if (ImGuizmo::Manipulate(
          glm::value_ptr(camera->viewMatrix), glm::value_ptr(gizmoPerspective),
          getImguizmoOperation(state.activeTransform), ImGuizmo::WORLD,
          glm::value_ptr(worldTransform), nullptr, nullptr, nullptr)) {
    if (!mAction) {
      auto localTransform = *selected.get_ref<LocalTransform>().get();
      mAction.reset(
          new EntitySetLocalTransformContinuous(selected, localTransform));
    }

    auto localTransform =
        calculateLocalTransformFromWorld(state, selected, worldTransform);
    mAction->setNewComponent(localTransform);

    selected.set(localTransform);
  }

  if (!ImGuizmo::IsUsing() && mAction) {
    actionExecutor.execute(std::move(mAction));
    mAction.reset();
  }

  return ImGuizmo::IsOver();
}

} // namespace quoll::editor
