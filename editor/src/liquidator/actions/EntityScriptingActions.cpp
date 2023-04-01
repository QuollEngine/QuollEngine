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

bool EntitySetScript::predicate(WorkspaceState &state) {
  return state.assetRegistry.getLuaScripts().hasAsset(mScript);
}

EntitySetScriptVariable::EntitySetScriptVariable(
    Entity entity, const String &name, const LuaScriptInputVariable &value)
    : mEntity(entity), mName(name), mValue(value) {}

ActionExecutorResult EntitySetScriptVariable::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.get<Script>(mEntity).variables.insert_or_assign(mName,
                                                                       mValue);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetScriptVariable::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  if (!scene.entityDatabase.has<Script>(mEntity)) {
    return false;
  }

  auto scriptHandle = scene.entityDatabase.get<Script>(mEntity).handle;
  if (!state.assetRegistry.getLuaScripts().hasAsset(scriptHandle)) {
    return false;
  }

  const auto &variables =
      state.assetRegistry.getLuaScripts().getAsset(scriptHandle).data.variables;

  auto it = variables.find(mName);
  if (it == variables.end()) {
    return false;
  }

  if (!mValue.isType(it->second.type)) {
    return false;
  }

  if (mValue.isType(LuaScriptVariableType::AssetPrefab)) {
    auto handle = mValue.get<PrefabAssetHandle>();
    if (!state.assetRegistry.getPrefabs().hasAsset(handle)) {
      return false;
    }
  }

  return true;
}

void EntitySetScriptVariable::setValue(LuaScriptInputVariable value) {
  mValue = value;
}

} // namespace liquid::editor
