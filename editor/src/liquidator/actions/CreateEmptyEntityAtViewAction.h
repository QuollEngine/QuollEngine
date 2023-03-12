#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

static Action CreateEmptyEntityAtViewAction{
    "CreateEmptyEntityAtView", [](WorkspaceState &state, std::any data) {
      auto &scene = state.mode == WorkspaceMode::Simulation
                        ? state.simulationScene
                        : state.scene;

      const auto &viewMatrix =
          scene.entityDatabase.get<Camera>(state.camera).viewMatrix;

      static constexpr glm::vec3 DistanceFromEye{0.0f, 0.0f, -10.0f};
      const auto &invViewMatrix = glm::inverse(viewMatrix);
      const auto &orientation = invViewMatrix * glm::translate(DistanceFromEye);

      LocalTransform transform{};
      transform.localPosition = orientation[3];

      auto entity = scene.entityDatabase.create();
      scene.entityDatabase.set(entity, transform);
      scene.entityDatabase.set<WorldTransform>(entity, {});
      scene.entityDatabase.set<Name>(entity, {"New entity"});

      ActionExecutorResult res{};
      res.entitiesToSave.push_back(entity);
      return res;
    }};

} // namespace liquid::editor
