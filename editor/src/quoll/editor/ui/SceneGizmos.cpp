#include "quoll/core/Base.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/JointAttachment.h"
#include "quoll/scene/Skeleton.h"
#include "quoll/scene/Camera.h"

#include "SceneGizmos.h"

#include "quoll/imgui/Imgui.h"
#include "ImGuizmo.h"

namespace quoll::editor {

/**
 * @brief Calculate local transforms from world
 *
 * @param state Workspace state
 * @param entity Entity
 * @param worldTransform World transform
 * @return Local transform
 */
static LocalTransform
calculateLocalTransformFromWorld(WorkspaceState &state, Entity entity,
                                 const glm::mat4 &worldTransform) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  auto &entityDatabase = scene.entityDatabase;

  glm::vec3 localScale;
  glm::quat localRotation;
  glm::vec3 localPosition;

  glm::vec3 noopSkew;
  glm::vec4 noopPerspective;

  if (!entityDatabase.has<Parent>(entity)) {
    glm::decompose(worldTransform, localScale, localRotation, localPosition,
                   noopSkew, noopPerspective);
    return LocalTransform{localPosition, localRotation, localScale};
  }

  Entity parent = entityDatabase.get<Parent>(entity).parent;

  QuollAssert(entityDatabase.exists(parent) &&
                  entityDatabase.has<WorldTransform>(parent),
              "Parent entity does not exist or has no transform");

  const auto &parentWorld =
      entityDatabase.get<WorldTransform>(parent).worldTransform;

  i16 jointId = -1;
  if (entityDatabase.has<JointAttachment>(entity) &&
      entityDatabase.has<Skeleton>(parent)) {
    jointId = entityDatabase.get<JointAttachment>(entity).joint;
  }

  if (jointId >= 0 && jointId < std::numeric_limits<u8>::max()) {
    const auto &jointTransform =
        entityDatabase.get<Skeleton>(parent).jointWorldTransforms.at(jointId);

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

/**
 * @brief Get imguizmo operation
 *
 * @param transformOperation Transform operation
 * @return Imguizmo operation
 */
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
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  const auto &pos = ImGui::GetItemRectMin();
  const auto &size = ImGui::GetItemRectSize();

  ImGuizmo::SetDrawlist();
  ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

  const auto &camera = scene.entityDatabase.get<Camera>(state.activeCamera);
  auto gizmoPerspective = camera.projectionMatrix;

  auto selected = state.selectedEntity;
  const auto &world = scene.entityDatabase.get<WorldTransform>(selected);
  auto worldTransform = world.worldTransform;
  if (ImGuizmo::Manipulate(
          glm::value_ptr(camera.viewMatrix), glm::value_ptr(gizmoPerspective),
          getImguizmoOperation(state.activeTransform), ImGuizmo::WORLD,
          glm::value_ptr(worldTransform), nullptr, nullptr, nullptr)) {
    if (!mAction) {
      const auto &localTransform =
          scene.entityDatabase.get<LocalTransform>(selected);
      mAction.reset(
          new EntitySetLocalTransformContinuous(selected, localTransform));
    }

    auto localTransform =
        calculateLocalTransformFromWorld(state, selected, worldTransform);
    mAction->setNewComponent(localTransform);

    scene.entityDatabase.set(selected, localTransform);
  }

  if (!ImGuizmo::IsUsing() && mAction) {
    actionExecutor.execute(std::move(mAction));
    mAction.reset();
  }

  return ImGuizmo::IsOver();
}

} // namespace quoll::editor
