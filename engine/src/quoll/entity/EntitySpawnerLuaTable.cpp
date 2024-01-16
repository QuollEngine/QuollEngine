#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/lua-scripting/ScriptDecorator.h"
#include "quoll/lua-scripting/Messages.h"

#include "EntitySpawner.h"
#include "EntitySpawnerLuaTable.h"

namespace quoll {

static bool isPrefabEmpty(const PrefabAsset &prefab) {
  return prefab.animators.empty() && prefab.meshes.empty() &&
         prefab.skeletons.empty() && prefab.directionalLights.empty() &&
         prefab.pointLights.empty() && prefab.meshRenderers.empty() &&
         prefab.skinnedMeshRenderers.empty() && prefab.transforms.empty();
}

EntitySpawnerLuaTable::EntitySpawnerLuaTable(ScriptGlobals scriptGlobals)
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
        "EntitySpawner", "spawnPrefab", getAssetTypeString(AssetType::Prefab));

    return sol::nil;
  }

  if (isPrefabEmpty(
          mScriptGlobals.assetRegistry.getPrefabs().getAsset(prefab).data)) {
    Engine::getUserLogger().warning()
        << lua::Messages::nothingSpawnedBecauseEmptyPrefab(
               "EntitySpawner", "spawnPrefab",
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
        "EntitySpawner", "spawnSprite", getAssetTypeString(AssetType::Texture));

    return sol::nil;
  }

  auto entity =
      EntitySpawner(mScriptGlobals.entityDatabase, mScriptGlobals.assetRegistry)
          .spawnSprite(texture, {});

  return EntityLuaTable(entity, mScriptGlobals);
}

void EntitySpawnerLuaTable::create(sol::state_view state) {
  auto usertype = state.new_usertype<EntitySpawnerLuaTable>("EntitySpawner");

  usertype["spawnEmpty"] = &EntitySpawnerLuaTable::spawnEmpty;
  usertype["spawnPrefab"] = &EntitySpawnerLuaTable::spawnPrefab;
  usertype["spawnSprite"] = &EntitySpawnerLuaTable::spawnSprite;
}

} // namespace quoll
