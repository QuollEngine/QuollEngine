#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/scripting/ScriptDecorator.h"
#include "quoll/scripting/LuaMessages.h"

#include "EntitySpawner.h"
#include "EntitySpawnerScriptingInterface.h"

namespace quoll {

/**
 * @brief Check if prefab is empty
 *
 * @param prefab Prefab asset data
 * @retval true Prefab asset is empty
 * @retval false Prefab asset is not empty
 */
static bool isPrefabEmpty(const PrefabAsset &prefab) {
  return prefab.animators.empty() && prefab.meshes.empty() &&
         prefab.skeletons.empty() && prefab.directionalLights.empty() &&
         prefab.pointLights.empty() && prefab.meshRenderers.empty() &&
         prefab.skinnedMeshRenderers.empty() && prefab.transforms.empty();
}

EntitySpawnerScriptingInterface::LuaInterface::LuaInterface(
    ScriptGlobals &scriptGlobals)
    : mScriptGlobals(scriptGlobals) {}

EntityTable EntitySpawnerScriptingInterface::LuaInterface::spawnEmpty() {
  auto entity =
      EntitySpawner(mScriptGlobals.entityDatabase, mScriptGlobals.assetRegistry)
          .spawnEmpty({});
  return EntityTable(entity, mScriptGlobals);
}

sol_maybe<EntityTable>
EntitySpawnerScriptingInterface::LuaInterface::spawnPrefab(
    PrefabAssetHandle prefab) {
  if (!mScriptGlobals.assetRegistry.getPrefabs().hasAsset(prefab)) {
    Engine::getUserLogger().error() << LuaMessages::assetNotFound(
        getName(), "spawn_prefab", getAssetTypeString(AssetType::Prefab));

    return sol::nil;
  }

  if (isPrefabEmpty(
          mScriptGlobals.assetRegistry.getPrefabs().getAsset(prefab).data)) {
    Engine::getUserLogger().warning()
        << LuaMessages::nothingSpawnedBecauseEmptyPrefab(
               getName(), "spawn_prefab",
               mScriptGlobals.assetRegistry.getPrefabs().getAsset(prefab).name);

    return sol::nil;
  }

  auto entities =
      EntitySpawner(mScriptGlobals.entityDatabase, mScriptGlobals.assetRegistry)
          .spawnPrefab(prefab, {});

  return EntityTable(entities.at(0), mScriptGlobals);
}

sol_maybe<EntityTable>
EntitySpawnerScriptingInterface::LuaInterface::spawnSprite(
    TextureAssetHandle texture) {
  if (!mScriptGlobals.assetRegistry.getTextures().hasAsset(texture)) {
    Engine::getUserLogger().error() << LuaMessages::assetNotFound(
        getName(), "spawn_sprite", getAssetTypeString(AssetType::Texture));

    return sol::nil;
  }

  auto entity =
      EntitySpawner(mScriptGlobals.entityDatabase, mScriptGlobals.assetRegistry)
          .spawnSprite(texture, {});

  return EntityTable(entity, mScriptGlobals);
}

void EntitySpawnerScriptingInterface::LuaInterface::create(
    sol::state_view state) {
  auto usertype =
      state.new_usertype<EntitySpawnerScriptingInterface::LuaInterface>(
          "EntitySpawner");

  usertype["spawn_empty"] = &LuaInterface::spawnEmpty;
  usertype["spawn_prefab"] = &LuaInterface::spawnPrefab;
  usertype["spawn_sprite"] = &LuaInterface::spawnSprite;
}

} // namespace quoll
