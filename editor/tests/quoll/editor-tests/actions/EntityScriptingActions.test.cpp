#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntityScriptingActions.h"
#include "quoll-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntitySetScriptVariableActionTest = ActionTestBase;

TEST_F(EntitySetScriptVariableActionTest, ExecutorSetsScriptVariableForEntity) {
  auto scriptAsset = createAsset<quoll::LuaScriptAsset>();
  auto prefab = createAsset<quoll::PrefabAsset>();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScriptAssetRef>(entity,
                                                           {scriptAsset});

  quoll::editor::EntitySetScriptVariable action(entity, "var1", prefab);

  auto res = action.onExecute(state, assetCache);

  const auto &variables =
      state.scene.entityDatabase.get<quoll::LuaScriptAssetRef>(entity)
          .variables;

  EXPECT_EQ(variables.size(), 1);
  EXPECT_EQ(
      variables.at("var1").get<quoll::AssetRef<quoll::PrefabAsset>>().handle(),
      prefab.handle());

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetScriptVariableActionTest,
       UndoSetsPreviousScriptVariableForEntity) {
  auto scriptAsset = createAsset<quoll::LuaScriptAsset>();
  auto prefab1 = createAsset<quoll::PrefabAsset>();
  auto prefab2 = createAsset<quoll::PrefabAsset>();

  auto entity = state.scene.entityDatabase.create();

  quoll::LuaScriptAssetRef script;
  script.asset = scriptAsset;
  script.variables.insert({"var1", prefab1});
  state.scene.entityDatabase.set(entity, script);

  quoll::editor::EntitySetScriptVariable action(entity, "var1", prefab2);

  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  const auto &variables =
      state.scene.entityDatabase.get<quoll::LuaScriptAssetRef>(entity)
          .variables;

  EXPECT_EQ(variables.size(), 1);
  EXPECT_EQ(
      variables.at("var1").get<quoll::AssetRef<quoll::PrefabAsset>>().handle(),
      prefab1.handle());

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfEntityHasNoScript) {
  auto entity = state.scene.entityDatabase.create();

  quoll::editor::EntitySetScriptVariable action(entity, "test",
                                                quoll::String("Hello world"));

  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfEntityScriptIsInvalid) {
  auto scriptAsset = createAsset<quoll::LuaScriptAsset>();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScriptAssetRef>(entity,
                                                           {scriptAsset});

  quoll::editor::EntitySetScriptVariable action(entity, "test",
                                                quoll::String("Hello world"));
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfVariableDoesNotExistInScriptAsset) {
  quoll::LuaScriptAsset assetData{};
  assetData.variables.insert_or_assign(
      "var1",
      quoll::LuaScriptVariable{quoll::LuaScriptVariableType::String, "var1"});

  auto scriptAsset = createAsset<quoll::LuaScriptAsset>(assetData);

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScriptAssetRef>(entity,
                                                           {scriptAsset});

  quoll::editor::EntitySetScriptVariable action(entity, "var2",
                                                quoll::String("Hello world"));
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfVariableTypeDoesNotMatchTypeInScriptAsset) {
  quoll::LuaScriptAsset assetData{};
  assetData.variables.insert_or_assign(
      "var1", quoll::LuaScriptVariable{
                  quoll::LuaScriptVariableType::AssetPrefab, "var1"});
  auto scriptAsset = createAsset<quoll::LuaScriptAsset>(assetData);

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScriptAssetRef>(entity,
                                                           {scriptAsset});

  quoll::editor::EntitySetScriptVariable action(entity, "var1",
                                                quoll::String("Test value"));
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfPrefabAssetDoesNotExist) {
  quoll::LuaScriptAsset assetData{};
  assetData.variables.insert_or_assign(
      "var1", quoll::LuaScriptVariable{
                  quoll::LuaScriptVariableType::AssetPrefab, "var1"});

  auto scriptAsset = createAsset<quoll::LuaScriptAsset>(assetData);

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScriptAssetRef>(entity,
                                                           {scriptAsset});

  quoll::editor::EntitySetScriptVariable action(
      entity, "var1", quoll::AssetRef<quoll::PrefabAsset>());
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfTextureAssetDoesNotExist) {
  quoll::LuaScriptAsset assetData{};
  assetData.variables.insert_or_assign(
      "var1", quoll::LuaScriptVariable{
                  quoll::LuaScriptVariableType::AssetTexture, "var1"});

  auto scriptAsset = createAsset<quoll::LuaScriptAsset>(assetData);

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScriptAssetRef>(entity,
                                                           {scriptAsset});

  quoll::editor::EntitySetScriptVariable action(
      entity, "var1", quoll::AssetRef<quoll::TextureAsset>());
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetScriptVariableActionTest, PredicateReturnsTrueIfValidVariable) {
  quoll::LuaScriptAsset assetData{};
  assetData.variables.insert_or_assign(
      "var1", quoll::LuaScriptVariable{
                  quoll::LuaScriptVariableType::AssetPrefab, "var1"});
  auto scriptAsset = createAsset<quoll::LuaScriptAsset>(assetData);
  auto prefab = createAsset<quoll::PrefabAsset>();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScriptAssetRef>(entity,
                                                           {scriptAsset});

  quoll::editor::EntitySetScriptVariable action(entity, "var1", prefab);
  EXPECT_TRUE(action.predicate(state, assetCache));
}
