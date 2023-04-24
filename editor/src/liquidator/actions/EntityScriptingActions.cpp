#include "liquid/core/Base.h"
#include "EntityCameraActions.h"
#include "EntityScriptingActions.h"

namespace liquid::editor {

EntityCreateScript::EntityCreateScript(Entity entity,
                                       LuaScriptAssetHandle handle)
    : mEntity(entity), mHandle(handle) {}

ActionExecutorResult EntityCreateScript::onExecute(WorkspaceState &state) {
  return EntityDefaultCreateComponent<Script>(mEntity, {mHandle})
      .onExecute(state);
}

ActionExecutorResult EntityCreateScript::onUndo(WorkspaceState &state) {
  return EntityDefaultCreateComponent<Script>(mEntity, {mHandle}).onUndo(state);
}

bool EntityCreateScript::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  return !scene.entityDatabase.has<Script>(mEntity) &&
         state.assetRegistry.getLuaScripts().hasAsset(mHandle);
}

EntitySetScript::EntitySetScript(Entity entity, LuaScriptAssetHandle script)
    : mEntity(entity), mScript(script) {}

ActionExecutorResult EntitySetScript::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  mOldScript = scene.entityDatabase.get<Script>(mEntity).handle;

  scene.entityDatabase.set<Script>(mEntity, {mScript});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult EntitySetScript::onUndo(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<Script>(mEntity, {mOldScript});

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

  auto &script = scene.entityDatabase.get<Script>(mEntity);
  mOldScript = script;

  script.variables.insert_or_assign(mName, mValue);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult EntitySetScriptVariable::onUndo(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(mEntity, mOldScript);

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
