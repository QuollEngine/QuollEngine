#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/lua-scripting/Messages.h"
#include "quoll/lua-scripting/ScriptDecorator.h"
#include "EntitySpawner.h"
#include "EntitySpawnerLuaTable.h"

namespace quoll {

constexpr bool isPrefabEmpty(const PrefabAsset &prefab) {
  return prefab.animators.empty() && prefab.meshes.empty() &&
         prefab.skeletons.empty() && prefab.directionalLights.empty() &&
         prefab.pointLights.empty() && prefab.meshRenderers.empty() &&
         prefab.skinnedMeshRenderers.empty() && prefab.transforms.empty();
}

EntitySpawnerLuaTable::EntitySpawnerLuaTable(ScriptGlobals scriptGlobals)
    : mScriptGlobals(scriptGlobals) {}

EntityLuaTable EntitySpawnerLuaTable::spawnEmpty() {
  auto entity =
      EntitySpawner(mScriptGlobals.entityDatabase, mScriptGlobals.assetCache)
          .spawnEmpty({});
  return EntityLuaTable(entity, mScriptGlobals);
}

sol_maybe<EntityLuaTable>
EntitySpawnerLuaTable::spawnPrefab(AssetHandleType handle) {
  const AssetHandle<PrefabAsset> prefabHandle(handle);
  if (!mScriptGlobals.assetCache.getRegistry().has(prefabHandle)) {
    Engine::getUserLogger().error() << lua::Messages::assetNotFound(
        "EntitySpawner", "spawnPrefab", getAssetTypeString(AssetType::Prefab));
    return sol::nil;
  }

  const AssetRef<PrefabAsset> prefab(
      mScriptGlobals.assetCache.getRegistry().getMap<PrefabAsset>(),
      prefabHandle);

  if (isPrefabEmpty(prefab.get())) {
    Engine::getUserLogger().warning()
        << lua::Messages::nothingSpawnedBecauseEmptyPrefab(
               "EntitySpawner", "spawnPrefab", prefab.meta().name);

    return sol::nil;
  }

  auto entities =
      EntitySpawner(mScriptGlobals.entityDatabase, mScriptGlobals.assetCache)
          .spawnPrefab(prefab, {});

  return EntityLuaTable(entities.at(0), mScriptGlobals);
}

sol_maybe<EntityLuaTable>
EntitySpawnerLuaTable::spawnSprite(AssetHandleType handle) {
  const AssetHandle<TextureAsset> textureHandle(handle);
  if (!mScriptGlobals.assetCache.getRegistry().has(textureHandle)) {
    Engine::getUserLogger().error() << lua::Messages::assetNotFound(
        "EntitySpawner", "spawnSprite", getAssetTypeString(AssetType::Texture));

    return sol::nil;
  }

  const AssetRef<TextureAsset> texture(
      mScriptGlobals.assetCache.getRegistry().getMap<TextureAsset>(),
      textureHandle);

  auto entity =
      EntitySpawner(mScriptGlobals.entityDatabase, mScriptGlobals.assetCache)
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
