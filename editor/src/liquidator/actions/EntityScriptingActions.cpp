#include "liquid/core/Base.h"
#include "EntityCameraActions.h"
#include "EntityScriptingActions.h"

namespace liquid::editor {

EntitySetScript::EntitySetScript(Entity entity, LuaScriptAssetHandle script)
    : mEntity(entity), mScript(script) {}

ActionExecutorResult EntitySetScript::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<Script>(mEntity, {mScript});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetScript::predicate(WorkspaceState &state) { return true; }

} // namespace liquid::editor
