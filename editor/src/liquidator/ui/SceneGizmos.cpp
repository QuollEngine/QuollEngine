#include "liquid/core/Base.h"
#include "SceneGizmos.h"

#include "liquid/imgui/Imgui.h"
#include "ImGuizmo.h"

namespace liquid::editor {

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

  glm::vec3 worldPosition;
  glm::quat worldRotation;
  glm::vec3 worldScale;

  glm::vec3 noopSkew;
  glm::vec4 noopPerspective;

  glm::decompose(worldTransform, worldScale, worldRotation, worldPosition,
                 noopSkew, noopPerspective);

  if (!entityDatabase.has<Parent>(entity)) {
    return LocalTransform{worldPosition, worldRotation, worldScale};
  }

  Entity parent = entityDatabase.get<Parent>(entity).parent;

  LIQUID_ASSERT(entityDatabase.exists(parent) &&
                    entityDatabase.has<WorldTransform>(parent),
                "Parent entity does not exist or has no transform");

  const auto &parentWorld =
      entityDatabase.get<WorldTransform>(parent).worldTransform;

  glm::vec3 parentPosition;
  glm::quat parentRotation;
  glm::vec3 parentScale;

  glm::decompose(parentWorld, parentScale, parentRotation, parentPosition,
                 noopSkew, noopPerspective);

  return LocalTransform{
      worldPosition - parentPosition,
      glm::toQuat(glm::inverse(parentWorld) * glm::toMat4(worldRotation)),
      glm::vec3(worldScale.x / parentScale.x, worldScale.y / parentScale.y,
                worldScale.z / parentScale.z)};
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

} // namespace liquid::editor
