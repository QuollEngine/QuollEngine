#include "quoll/core/Base.h"
#include "EntityCameraActions.h"
#include "EntityCreateComponentAction.h"
#include "EntityScriptingActions.h"

namespace quoll::editor {

EntityCreateScript::EntityCreateScript(Entity entity,
                                       AssetHandle<LuaScriptAsset> handle)
    : mEntity(entity), mHandle(handle) {}

ActionExecutorResult
EntityCreateScript::onExecute(WorkspaceState &state,
                              AssetRegistry &assetRegistry) {
  return EntityCreateComponent<LuaScript>(mEntity, {mHandle})
      .onExecute(state, assetRegistry);
}

ActionExecutorResult EntityCreateScript::onUndo(WorkspaceState &state,
                                                AssetRegistry &assetRegistry) {
  return EntityCreateComponent<LuaScript>(mEntity, {mHandle})
      .onUndo(state, assetRegistry);
}

bool EntityCreateScript::predicate(WorkspaceState &state,
                                   AssetRegistry &assetRegistry) {
  auto &scene = state.scene;
  return !scene.entityDatabase.has<LuaScript>(mEntity) &&
         assetRegistry.has(mHandle);
}

EntitySetScript::EntitySetScript(Entity entity,
                                 AssetHandle<LuaScriptAsset> script)
    : mEntity(entity), mScript(script) {}

ActionExecutorResult EntitySetScript::onExecute(WorkspaceState &state,
                                                AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldScript = scene.entityDatabase.get<LuaScript>(mEntity).handle;

  scene.entityDatabase.set<LuaScript>(mEntity, {mScript});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult EntitySetScript::onUndo(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  scene.entityDatabase.set<LuaScript>(mEntity, {mOldScript});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetScript::predicate(WorkspaceState &state,
                                AssetRegistry &assetRegistry) {
  return assetRegistry.has(mScript);
}

EntitySetScriptVariable::EntitySetScriptVariable(
    Entity entity, const String &name, const LuaScriptInputVariable &value)
    : mEntity(entity), mName(name), mValue(value) {}

ActionExecutorResult
EntitySetScriptVariable::onExecute(WorkspaceState &state,
                                   AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  auto &script = scene.entityDatabase.get<LuaScript>(mEntity);
  mOldScript = script;

  script.variables.insert_or_assign(mName, mValue);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
EntitySetScriptVariable::onUndo(WorkspaceState &state,
                                AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  scene.entityDatabase.set(mEntity, mOldScript);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetScriptVariable::predicate(WorkspaceState &state,
                                        AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  if (!scene.entityDatabase.has<LuaScript>(mEntity)) {
    return false;
  }

  auto scriptHandle = scene.entityDatabase.get<LuaScript>(mEntity).handle;
  if (!assetRegistry.has(scriptHandle)) {
    return false;
  }

  const auto &variables = assetRegistry.get(scriptHandle).data.variables;

  auto it = variables.find(mName);
  if (it == variables.end()) {
    return false;
  }

  if (!mValue.isType(it->second.type)) {
    return false;
  }

  if (mValue.isType(LuaScriptVariableType::AssetPrefab)) {
    auto handle = mValue.get<AssetHandle<PrefabAsset>>();
    if (!assetRegistry.has(handle)) {
      return false;
    }
  }

  return true;
}

void EntitySetScriptVariable::setValue(LuaScriptInputVariable value) {
  mValue = value;
}

} // namespace quoll::editor
