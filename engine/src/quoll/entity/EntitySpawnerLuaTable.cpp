#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/lua-scripting/ScriptDecorator.h"
#include "quoll/lua-scripting/Messages.h"

#include "EntitySpawner.h"
#include "EntitySpawnerLuaTable.h"

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

EntitySpawnerLuaTable::EntitySpawnerLuaTable(ScriptGlobals &scriptGlobals)
    : mScriptGlobals(scriptGlobals) {}

EntityLuaTable EntitySpawnerLuaTable::spawnEmpty() {
  auto entity =
      EntitySpawner(mScriptGlobals.entityDatabase, mScriptGlobals.assetRegistry)
          .spawnEmpty({});
  return EntityLuaTable(entity, mScriptGlobals);
}

sol_maybe<EntityLuaTable>
EntitySpawnerLuaTable::spawnPrefab(PrefabAssetHandle prefab) {
  if (!mScriptGlobals.assetRegistry.getPrefabs().hasAsset(prefab)) {
    Engine::getUserLogger().error() << lua::Messages::assetNotFound(
        getName(), "spawn_prefab", getAssetTypeString(AssetType::Prefab));

    return sol::nil;
  }

  if (isPrefabEmpty(
          mScriptGlobals.assetRegistry.getPrefabs().getAsset(prefab).data)) {
    Engine::getUserLogger().warning()
        << lua::Messages::nothingSpawnedBecauseEmptyPrefab(
               getName(), "spawn_prefab",
               mScriptGlobals.assetRegistry.getPrefabs().getAsset(prefab).name);

    return sol::nil;
  }

  auto entities =
      EntitySpawner(mScriptGlobals.entityDatabase, mScriptGlobals.assetRegistry)
          .spawnPrefab(prefab, {});

  return EntityLuaTable(entities.at(0), mScriptGlobals);
}

sol_maybe<EntityLuaTable>
EntitySpawnerLuaTable::spawnSprite(TextureAssetHandle texture) {
  if (!mScriptGlobals.assetRegistry.getTextures().hasAsset(texture)) {
    Engine::getUserLogger().error() << lua::Messages::assetNotFound(
        getName(), "spawn_sprite", getAssetTypeString(AssetType::Texture));

    return sol::nil;
  }

  auto entity =
      EntitySpawner(mScriptGlobals.entityDatabase, mScriptGlobals.assetRegistry)
          .spawnSprite(texture, {});

  return EntityLuaTable(entity, mScriptGlobals);
}

void EntitySpawnerLuaTable::create(sol::state_view state) {
  auto usertype = state.new_usertype<EntitySpawnerLuaTable>("EntitySpawner");

  usertype["spawn_empty"] = &EntitySpawnerLuaTable::spawnEmpty;
  usertype["spawn_prefab"] = &EntitySpawnerLuaTable::spawnPrefab;
  usertype["spawn_sprite"] = &EntitySpawnerLuaTable::spawnSprite;
}

} // namespace quoll
