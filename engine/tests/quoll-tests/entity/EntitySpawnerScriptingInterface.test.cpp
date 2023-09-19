#include "quoll/core/Base.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using EntitySpawnerLuaInterfaceTest = LuaScriptingInterfaceTestBase;

TEST_F(EntitySpawnerLuaInterfaceTest,
       SpawnEmptyCreatesEmptyEntityAndReturnsEntityTable) {
  auto entity = entityDatabase.create();
  auto &scope = call(entity, "entity_spawner_spawn_empty");

  EXPECT_TRUE(scope.isGlobal<quoll::LuaTable>("created_entity"));
  auto createdEntityTable = scope.getGlobal<quoll::LuaTable>("created_entity");
  createdEntityTable.get("id");
  auto createdEntity = scope.get<quoll::Entity>();

  EXPECT_NE(entity, createdEntity);
  EXPECT_TRUE(entityDatabase.exists(createdEntity));
  // The final position of root node is always at (0, 0)
  EXPECT_EQ(
      entityDatabase.get<quoll::LocalTransform>(createdEntity).localPosition,
      glm::vec3{0.0f});
  EXPECT_TRUE(entityDatabase.has<quoll::WorldTransform>(createdEntity));
}

TEST_F(EntitySpawnerLuaInterfaceTest,
       SpawnPrefabReturnsNullIfInvalidArguments) {
  auto entity = entityDatabase.create();
  auto &scope = call(entity, "entity_spawner_spawn_prefab_invalid");
}

TEST_F(EntitySpawnerLuaInterfaceTest, SpawnPrefabReturnsNullIfPrefabIsEmpty) {
  auto prefab = assetCache.getRegistry().getPrefabs().addAsset({});
  ASSERT_EQ(prefab, quoll::PrefabAssetHandle{1});

  auto entity = entityDatabase.create();

  auto &scope = call(entity, "entity_spawner_spawn_prefab");
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("created_entity"));
}

TEST_F(EntitySpawnerLuaInterfaceTest,
       SpawnPrefabCreatesPrefabEntityAndReturnsEntityTableForRootNode) {
  quoll::AssetData<quoll::PrefabAsset> asset{};
  asset.data.transforms.push_back(
      {0, quoll::PrefabTransformData{glm::vec3{5.0f}}});

  auto prefab = assetCache.getRegistry().getPrefabs().addAsset(asset);
  ASSERT_EQ(prefab, quoll::PrefabAssetHandle{1});

  auto entity = entityDatabase.create();

  auto &scope = call(entity, "entity_spawner_spawn_prefab");

  EXPECT_TRUE(scope.isGlobal<quoll::LuaTable>("created_entity"));
  auto createdEntityTable = scope.getGlobal<quoll::LuaTable>("created_entity");
  createdEntityTable.get("id");
  auto createdEntity = scope.get<quoll::Entity>();

  EXPECT_NE(entity, createdEntity);
  EXPECT_TRUE(entityDatabase.exists(createdEntity));

  EXPECT_TRUE(entityDatabase.has<quoll::LocalTransform>(createdEntity));

  // The final position of root node is always at (0, 0)
  EXPECT_EQ(
      entityDatabase.get<quoll::LocalTransform>(createdEntity).localPosition,
      glm::vec3{0.0f});
  EXPECT_TRUE(entityDatabase.has<quoll::WorldTransform>(createdEntity));
}

TEST_F(EntitySpawnerLuaInterfaceTest,
       SpawnSpriteReturnsNullIfInvalidArguments) {
  auto entity = entityDatabase.create();
  auto &scope = call(entity, "entity_spawner_spawn_sprite_invalid");
}

TEST_F(EntitySpawnerLuaInterfaceTest,
       SpawnSpriteCreatesSpriteEntityAndReturnsEntityTable) {
  auto texture = assetCache.getRegistry().getTextures().addAsset({});
  ASSERT_EQ(texture, quoll::TextureAssetHandle{1});

  auto entity = entityDatabase.create();

  auto &scope = call(entity, "entity_spawner_spawn_sprite");

  EXPECT_TRUE(scope.isGlobal<quoll::LuaTable>("created_entity"));
  auto createdEntityTable = scope.getGlobal<quoll::LuaTable>("created_entity");
  createdEntityTable.get("id");
  auto createdEntity = scope.get<quoll::Entity>();

  EXPECT_NE(entity, createdEntity);
  EXPECT_TRUE(entityDatabase.exists(createdEntity));

  EXPECT_TRUE(entityDatabase.has<quoll::LocalTransform>(createdEntity));

  EXPECT_EQ(
      entityDatabase.get<quoll::LocalTransform>(createdEntity).localPosition,
      glm::vec3{0.0f});
  EXPECT_TRUE(entityDatabase.has<quoll::WorldTransform>(createdEntity));
  EXPECT_TRUE(entityDatabase.has<quoll::Sprite>(createdEntity));
  EXPECT_EQ(entityDatabase.get<quoll::Sprite>(createdEntity).handle, texture);
}
