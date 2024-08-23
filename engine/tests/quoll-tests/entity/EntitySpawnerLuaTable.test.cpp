#include "quoll/core/Base.h"
#include "quoll/entity/EntityLuaTable.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/Sprite.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using EntitySpawnerLuaTableTest = LuaScriptingInterfaceTestBase;

TEST_F(EntitySpawnerLuaTableTest,
       SpawnEmptyCreatesEmptyEntityAndReturnsEntityTable) {
  auto entity = entityDatabase.entity();
  auto state = call(entity, "entitySpawnerSpawnEmpty");

  EXPECT_TRUE(state["createdEntity"].is<quoll::EntityLuaTable>());

  auto createdEntityTable = state["createdEntity"].get<quoll::EntityLuaTable>();
  auto createdEntity = createdEntityTable.getEntity();

  EXPECT_NE(entity, createdEntity);
  EXPECT_TRUE(createdEntity.is_valid());
  // The final position of root node is always at (0, 0)
  EXPECT_EQ(createdEntity.get_ref<quoll::LocalTransform>()->localPosition,
            glm::vec3{0.0f});
  EXPECT_TRUE(createdEntity.has<quoll::WorldTransform>());
}

TEST_F(EntitySpawnerLuaTableTest, SpawnPrefabReturnsNullIfPrefabIsEmpty) {
  auto prefab = assetCache.getRegistry().getPrefabs().addAsset({});
  ASSERT_EQ(prefab, quoll::PrefabAssetHandle{1});

  auto entity = entityDatabase.entity();

  auto state = call(entity, "entitySpawnerSpawnPrefab");
  EXPECT_TRUE(state["createdEntity"].is<sol::nil_t>());
}

TEST_F(EntitySpawnerLuaTableTest,
       SpawnPrefabCreatesPrefabEntityAndReturnsEntityTableForRootNode) {
  quoll::AssetData<quoll::PrefabAsset> asset{};
  asset.data.transforms.push_back(
      {0, quoll::PrefabTransformData{glm::vec3{5.0f}}});

  auto prefab = assetCache.getRegistry().getPrefabs().addAsset(asset);
  ASSERT_EQ(prefab, quoll::PrefabAssetHandle{1});

  auto entity = entityDatabase.entity();

  auto state = call(entity, "entitySpawnerSpawnPrefab");

  EXPECT_TRUE(state["createdEntity"].is<quoll::EntityLuaTable>());

  auto createdEntityTable = state["createdEntity"].get<quoll::EntityLuaTable>();
  auto createdEntity = createdEntityTable.getEntity();

  EXPECT_NE(entity, createdEntity);
  EXPECT_TRUE(createdEntity.is_valid());

  EXPECT_TRUE(createdEntity.has<quoll::LocalTransform>());

  // The final position of root node is always at (0, 0)
  EXPECT_EQ(createdEntity.get_ref<quoll::LocalTransform>()->localPosition,
            glm::vec3{0.0f});
  EXPECT_TRUE(createdEntity.has<quoll::WorldTransform>());
}

TEST_F(EntitySpawnerLuaTableTest, SpawnSpriteReturnsNullIfTextureDoesNotExist) {
  auto entity = entityDatabase.entity();
  auto state = call(entity, "entitySpawnerSpawnSprite");

  EXPECT_TRUE(state["createdEntity"].is<sol::nil_t>());
}

TEST_F(EntitySpawnerLuaTableTest,
       SpawnSpriteCreatesSpriteEntityAndReturnsEntityTable) {
  auto texture = assetCache.getRegistry().getTextures().addAsset({});
  ASSERT_EQ(texture, quoll::TextureAssetHandle{1});

  auto entity = entityDatabase.entity();

  auto state = call(entity, "entitySpawnerSpawnSprite");

  EXPECT_TRUE(state["createdEntity"].is<quoll::EntityLuaTable>());

  auto createdEntityTable = state["createdEntity"].get<quoll::EntityLuaTable>();
  auto createdEntity = createdEntityTable.getEntity();

  EXPECT_NE(entity, createdEntity);
  EXPECT_TRUE(createdEntity.is_valid());

  EXPECT_TRUE(createdEntity.has<quoll::LocalTransform>());

  EXPECT_EQ(createdEntity.get_ref<quoll::LocalTransform>()->localPosition,
            glm::vec3{0.0f});
  EXPECT_TRUE(createdEntity.has<quoll::WorldTransform>());
  EXPECT_TRUE(createdEntity.has<quoll::Sprite>());
  EXPECT_EQ(createdEntity.get_ref<quoll::Sprite>()->handle, texture);
}
