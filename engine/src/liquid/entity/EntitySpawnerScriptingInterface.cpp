#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/scripting/ScriptDecorator.h"
#include "liquid/scripting/LuaMessages.h"

#include "EntitySpawner.h"
#include "EntitySpawnerScriptingInterface.h"

namespace liquid {

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

int EntitySpawnerScriptingInterface::LuaInterface::spawnEmpty(void *state) {
  LuaScope scope(state);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);
  AssetRegistry &assetRegistry = *static_cast<AssetRegistry *>(
      scope.getGlobal<LuaUserData>("__privateAssetRegistry").pointer);

  auto entity = EntitySpawner(entityDatabase, assetRegistry).spawnEmpty({});
  ScriptDecorator::createEntityTable(scope, entity);

  return 1;
}

int EntitySpawnerScriptingInterface::LuaInterface::spawnPrefab(void *state) {
  LuaScope scope(state);

  if (!scope.is<uint32_t>(1)) {
    scope.set(nullptr);
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<uint32_t>(getName(), "spawn_prefab");

    return 1;
  }

  auto prefabHandle = static_cast<PrefabAssetHandle>(scope.get<uint32_t>(1));

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);
  AssetRegistry &assetRegistry = *static_cast<AssetRegistry *>(
      scope.getGlobal<LuaUserData>("__privateAssetRegistry").pointer);

  if (!assetRegistry.getPrefabs().hasAsset(prefabHandle)) {
    scope.set(nullptr);
    Engine::getUserLogger().error() << LuaMessages::assetNotFound(
        getName(), "spawn_prefab", getAssetTypeString(AssetType::Prefab));

    return 1;
  }

  if (isPrefabEmpty(assetRegistry.getPrefabs().getAsset(prefabHandle).data)) {
    scope.set(nullptr);
    Engine::getUserLogger().warning()
        << LuaMessages::nothingSpawnedBecauseEmptyPrefab(
               getName(), "spawn_prefab",
               assetRegistry.getPrefabs().getAsset(prefabHandle).name);

    return 1;
  }

  auto entities = EntitySpawner(entityDatabase, assetRegistry)
                      .spawnPrefab(prefabHandle, {});

  ScriptDecorator::createEntityTable(scope, entities.at(0));
  return 1;
}

int EntitySpawnerScriptingInterface::LuaInterface::spawnSprite(void *state) {
  LuaScope scope(state);

  if (!scope.is<uint32_t>(1)) {
    scope.set(nullptr);
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<uint32_t>(getName(), "spawn_sprite");

    return 1;
  }

  auto textureHandle = static_cast<TextureAssetHandle>(scope.get<uint32_t>(1));

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);
  AssetRegistry &assetRegistry = *static_cast<AssetRegistry *>(
      scope.getGlobal<LuaUserData>("__privateAssetRegistry").pointer);

  if (!assetRegistry.getTextures().hasAsset(textureHandle)) {
    scope.set(nullptr);
    Engine::getUserLogger().error() << LuaMessages::assetNotFound(
        getName(), "spawn_sprite", getAssetTypeString(AssetType::Texture));

    return 1;
  }

  auto entity = EntitySpawner(entityDatabase, assetRegistry)
                    .spawnSprite(textureHandle, {});

  ScriptDecorator::createEntityTable(scope, entity);
  return 1;
}

} // namespace liquid
