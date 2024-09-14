#include "quoll/core/Base.h"
#include "quoll/scene/WorldTransform.h"
#include "EditorCameraActions.h"

namespace quoll::editor {

MoveCameraToEntity::MoveCameraToEntity(Entity entity) : mEntity(entity) {}

ActionExecutorResult MoveCameraToEntity::onExecute(WorkspaceState &state,
                                                   AssetCache &assetCache) {
  auto &scene = state.scene;

  auto &transformComponent = scene.entityDatabase.get<WorldTransform>(mEntity);

  const auto &translation =
      glm::vec3(glm::column(transformComponent.worldTransform, 3));

  static constexpr glm::vec3 DistanceFromCenter{0.0f, 0.0f, 10.0f};

  auto &lookAt = scene.entityDatabase.get<CameraLookAt>(state.camera);

  lookAt.up = EditorCamera::DefaultUp;
  lookAt.center = translation;
  lookAt.eye = translation - DistanceFromCenter;
  return ActionExecutorResult{};
}

bool MoveCameraToEntity::predicate(WorkspaceState &state,
                                   AssetCache &assetCache) {
  return true;
}

} // namespace quoll::editor
