#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

static Action DeleteEntityAction{
    "DeleteEntity", [](WorkspaceState &state, std::any data) {
      auto entity = std::any_cast<Entity>(data);

      auto &scene = state.mode == WorkspaceMode::Simulation
                        ? state.simulationScene
                        : state.scene;

      scene.entityDatabase.set<Delete>(entity, {});

      if (state.selectedEntity == entity) {
        state.selectedEntity = Entity::Null;
      }

      ActionExecutorResult res{};
      res.entitiesToDelete.push_back(entity);
      return res;
    }};

} // namespace liquid::editor
