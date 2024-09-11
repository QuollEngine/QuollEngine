#include "quoll/core/Base.h"
#include "EntityCameraActions.h"
#include "EntityCreateComponentAction.h"
#include "EntityScriptingActions.h"

namespace quoll::editor {

EntitySetScriptVariable::EntitySetScriptVariable(
    Entity entity, const String &name, const LuaScriptInputVariable &value)
    : mEntity(entity), mName(name), mValue(value) {}

ActionExecutorResult
EntitySetScriptVariable::onExecute(WorkspaceState &state,
                                   AssetCache &assetCache) {
  auto &scene = state.scene;

  auto &script = scene.entityDatabase.get<LuaScriptAssetRef>(mEntity);
  mOldScript = script;

  script.variables.insert_or_assign(mName, mValue);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult EntitySetScriptVariable::onUndo(WorkspaceState &state,
                                                     AssetCache &assetCache) {
  auto &scene = state.scene;

  scene.entityDatabase.set(mEntity, mOldScript);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetScriptVariable::predicate(WorkspaceState &state,
                                        AssetCache &assetCache) {
  auto &scene = state.scene;

  if (!scene.entityDatabase.has<LuaScriptAssetRef>(mEntity)) {
    return false;
  }

  const auto &scriptAsset =
      scene.entityDatabase.get<LuaScriptAssetRef>(mEntity).asset;
  if (!scriptAsset) {
    return false;
  }

  const auto &variables = scriptAsset->variables;

  auto it = variables.find(mName);
  if (it == variables.end()) {
    return false;
  }

  if (!mValue.isType(it->second.type)) {
    return false;
  }

  if (mValue.isType(LuaScriptVariableType::AssetPrefab) &&
      !mValue.get<AssetRef<PrefabAsset>>()) {
    return false;
  }

  if (mValue.isType(LuaScriptVariableType::AssetTexture) &&
      !mValue.get<AssetRef<TextureAsset>>()) {
    return false;
  }

  return true;
}

void EntitySetScriptVariable::setValue(LuaScriptInputVariable value) {
  mValue = value;
}

} // namespace quoll::editor
