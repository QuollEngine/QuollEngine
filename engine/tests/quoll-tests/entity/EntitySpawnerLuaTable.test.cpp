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
  auto entity = entityDatabase.create();
  auto state = call(entity, "entitySpawnerSpawnEmpty");

  EXPECT_TRUE(state["createdEntity"].is<quoll::EntityLuaTable>());

  auto createdEntityTable = state["createdEntity"].get<quoll::EntityLuaTable>();
  auto createdEntity = createdEntityTable.getEntity();

  EXPECT_NE(entity, createdEntity);
  EXPECT_TRUE(entityDatabase.exists(createdEntity));
  // The final position of root node is always at (0, 0)
  EXPECT_EQ(
      entityDatabase.get<quoll::LocalTransform>(createdEntity).localPosition,
      glm::vec3{0.0f});
  EXPECT_TRUE(entityDatabase.has<quoll::WorldTransform>(createdEntity));
}

TEST_F(EntitySpawnerLuaTableTest, SpawnPrefabReturnsNullIfPrefabIsEmpty) {
  auto prefab = createAsset<quoll::PrefabAsset>();
  auto entity = entityDatabase.create();

  auto state = call(entity, "entitySpawnerSpawnPrefab");
  EXPECT_TRUE(state["createdEntity"].is<sol::nil_t>());
}

TEST_F(EntitySpawnerLuaTableTest,
       SpawnPrefabCreatesPrefabEntityAndReturnsEntityTableForRootNode) {
  quoll::PrefabAsset assetData{};
  assetData.transforms.push_back(
      {0, quoll::PrefabTransformData{glm::vec3{5.0f}}});

  auto prefab = createAsset(assetData);
  auto entity = entityDatabase.create();

  auto state = call(entity, "entitySpawnerSpawnPrefab");

  EXPECT_TRUE(state["createdEntity"].is<quoll::EntityLuaTable>());

  auto createdEntityTable = state["createdEntity"].get<quoll::EntityLuaTable>();
  auto createdEntity = createdEntityTable.getEntity();

  EXPECT_NE(entity, createdEntity);
  EXPECT_TRUE(entityDatabase.exists(createdEntity));

  EXPECT_TRUE(entityDatabase.has<quoll::LocalTransform>(createdEntity));

  // The final position of root node is always at (0, 0)
  EXPECT_EQ(
      entityDatabase.get<quoll::LocalTransform>(createdEntity).localPosition,
      glm::vec3{0.0f});
  EXPECT_TRUE(entityDatabase.has<quoll::WorldTransform>(createdEntity));
}

TEST_F(EntitySpawnerLuaTableTest, SpawnSpriteReturnsNullIfTextureDoesNotExist) {
  auto entity = entityDatabase.create();
  auto state = call(entity, "entitySpawnerSpawnSprite");

  EXPECT_TRUE(state["createdEntity"].is<sol::nil_t>());
}

TEST_F(EntitySpawnerLuaTableTest,
       SpawnSpriteCreatesSpriteEntityAndReturnsEntityTable) {
  auto texture = createAsset<quoll::TextureAsset>();
  auto entity = entityDatabase.create();

  auto state = call(entity, "entitySpawnerSpawnSprite");

  EXPECT_TRUE(state["createdEntity"].is<quoll::EntityLuaTable>());

  auto createdEntityTable = state["createdEntity"].get<quoll::EntityLuaTable>();
  auto createdEntity = createdEntityTable.getEntity();

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
