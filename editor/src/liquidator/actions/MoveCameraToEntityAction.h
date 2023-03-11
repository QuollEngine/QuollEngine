#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/editor-scene/EditorCamera.h"

#include <glm/gtc/matrix_access.hpp>

namespace liquid::editor {

static Action MoveCameraToEntityAction{
    "MoveCameraToEntity", "", [](WorkspaceState &state, std::any data) {
      auto entity = std::any_cast<Entity>(data);

      auto &scene = state.scene;

      auto &transformComponent =
          scene.entityDatabase.get<WorldTransform>(entity);

      const auto &translation =
          glm::vec3(glm::column(transformComponent.worldTransform, 3));

      static constexpr glm::vec3 DistanceFromCenter{0.0f, 0.0f, 10.0f};

      auto &lookAt = scene.entityDatabase.get<CameraLookAt>(state.camera);

      lookAt.up = EditorCamera::DefaultUp;
      lookAt.center = translation;
      lookAt.eye = translation - DistanceFromCenter;
      return ActionExecutorResult{};
    }};

} // namespace liquid::editor
