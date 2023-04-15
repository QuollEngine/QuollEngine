#include "liquid/core/Base.h"

#include "liquid-tests/Testing.h"
#include "liquid-tests/test-utils/ScriptingInterfaceTestBase.h"

using EntitySpawnerLuaInterfaceTest = LuaScriptingInterfaceTestBase;

TEST_F(EntitySpawnerLuaInterfaceTest,
       SpawnEmptyCreatesEmptyEntityAndReturnsEntityTable) {
  auto entity = entityDatabase.create();
  auto &scope = call(entity, "entity_spawner_spawn_empty");

  EXPECT_TRUE(scope.isGlobal<liquid::LuaTable>("created_entity"));
  auto createdEntityTable = scope.getGlobal<liquid::LuaTable>("created_entity");
  createdEntityTable.get("id");
  auto createdEntity = scope.get<liquid::Entity>();

  EXPECT_NE(entity, createdEntity);
  EXPECT_TRUE(entityDatabase.exists(createdEntity));
  // The final position of root node is always at (0, 0)
  EXPECT_EQ(
      entityDatabase.get<liquid::LocalTransform>(createdEntity).localPosition,
      glm::vec3{0.0f});
  EXPECT_TRUE(entityDatabase.has<liquid::WorldTransform>(createdEntity));
}

TEST_F(EntitySpawnerLuaInterfaceTest,
       SpawnPrefabReturnsNullIfInvalidArguments) {
  auto entity = entityDatabase.create();

  {
    auto &scope = call(entity, "entity_spawner_spawn_prefab_no_param");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("created_entity"));
  }

  {
    auto &scope = call(entity, "entity_spawner_spawn_prefab_param_nil");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("created_entity"));
  }

  {
    auto &scope = call(entity, "entity_spawner_spawn_prefab_param_boolean");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("created_entity"));
  }

  {
    auto &scope = call(entity, "entity_spawner_spawn_prefab_param_table");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("created_entity"));
  }

  {
    auto &scope = call(entity, "entity_spawner_spawn_prefab_param_string");
    EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("created_entity"));
  }
}

TEST_F(EntitySpawnerLuaInterfaceTest,
       SpawnPrefabReturnsNullIfPrefabDoesNotExist) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "entity_spawner_spawn_prefab_unknown_handle");
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("created_entity"));
}

TEST_F(EntitySpawnerLuaInterfaceTest, SpawnPrefabReturnsNullIfPrefabIsEmpty) {
  auto prefab = assetCache.getRegistry().getPrefabs().addAsset({});
  ASSERT_EQ(prefab, liquid::PrefabAssetHandle{1});

  auto entity = entityDatabase.create();

  auto &scope = call(entity, "entity_spawner_spawn_prefab");
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("created_entity"));
}

TEST_F(EntitySpawnerLuaInterfaceTest,
       SpawnPrefabDoesNothingIfPrefabDoesNotExist) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "entity_spawner_spawn_prefab_unknown_handle");
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("created_entity"));
}

TEST_F(EntitySpawnerLuaInterfaceTest,
       SpawnPrefabCreatesPrefabEntityAndReturnsEntityTableForRootNode) {
  liquid::AssetData<liquid::PrefabAsset> asset{};
  asset.data.transforms.push_back(
      {0, liquid::PrefabTransformData{glm::vec3{5.0f}}});

  auto prefab = assetCache.getRegistry().getPrefabs().addAsset(asset);
  ASSERT_EQ(prefab, liquid::PrefabAssetHandle{1});

  auto entity = entityDatabase.create();

  auto &scope = call(entity, "entity_spawner_spawn_prefab");

  EXPECT_TRUE(scope.isGlobal<liquid::LuaTable>("created_entity"));
  auto createdEntityTable = scope.getGlobal<liquid::LuaTable>("created_entity");
  createdEntityTable.get("id");
  auto createdEntity = scope.get<liquid::Entity>();

  EXPECT_NE(entity, createdEntity);
  EXPECT_TRUE(entityDatabase.exists(createdEntity));

  EXPECT_TRUE(entityDatabase.has<liquid::LocalTransform>(createdEntity));

  // The final position of root node is always at (0, 0)
  EXPECT_EQ(
      entityDatabase.get<liquid::LocalTransform>(createdEntity).localPosition,
      glm::vec3{0.0f});
  EXPECT_TRUE(entityDatabase.has<liquid::WorldTransform>(createdEntity));
}
