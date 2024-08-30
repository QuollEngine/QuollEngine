#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntityScriptingActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateScriptActionTest = ActionTestBase;

TEST_F(EntityCreateScriptActionTest, ExecutorCreatesScriptSourceComponent) {
  auto entity = state.scene.entityDatabase.create();

  quoll::editor::EntityCreateScript action(
      entity, quoll::AssetHandle<quoll::LuaScriptAsset>{15});
  auto res = action.onExecute(state, assetCache);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::LuaScript>(entity).handle,
            quoll::AssetHandle<quoll::LuaScriptAsset>{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityCreateScriptActionTest, UndoDeletesScriptSourceComponet) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScript>(
      entity, {quoll::AssetHandle<quoll::LuaScriptAsset>{25}});

  quoll::editor::EntityCreateScript action(
      entity, quoll::AssetHandle<quoll::LuaScriptAsset>{15});
  auto res = action.onUndo(state, assetCache);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::LuaScript>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityCreateScriptActionTest,
       PredicateReturnsFalseIfScriptAssetIsInvalid) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityCreateScript action(
      entity, quoll::AssetHandle<quoll::LuaScriptAsset>{15});
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntityCreateScriptActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasScript) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityCreateScript action(
      entity, quoll::AssetHandle<quoll::LuaScriptAsset>{15});
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntityCreateScriptActionTest,
       PredicateReturnsTrueIfScriptDoesNotExistAndAssetIsValid) {
  auto entity = state.scene.entityDatabase.create();
  auto handle = assetCache.getRegistry().add<quoll::LuaScriptAsset>({});

  quoll::editor::EntityCreateScript action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetCache));
}

using EntitySetScriptActionTest = ActionTestBase;

TEST_F(EntitySetScriptActionTest, ExecutorSetsScriptForEntity) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScript>(entity, {});

  quoll::editor::EntitySetScript action(
      entity, quoll::AssetHandle<quoll::LuaScriptAsset>{15});
  auto res = action.onExecute(state, assetCache);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::LuaScript>(entity).handle,
            quoll::AssetHandle<quoll::LuaScriptAsset>{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetScriptActionTest, UndoSetsPreviousScriptForEntity) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScript>(
      entity, {quoll::AssetHandle<quoll::LuaScriptAsset>{25}});

  quoll::editor::EntitySetScript action(
      entity, quoll::AssetHandle<quoll::LuaScriptAsset>{15});
  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::LuaScript>(entity).handle,
            quoll::AssetHandle<quoll::LuaScriptAsset>{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetScriptActionTest, PredicateReturnsFalseIfScriptIsInvalid) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntitySetScript action(
      entity, quoll::AssetHandle<quoll::LuaScriptAsset>{15});
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetScriptActionTest, PredicateReturnsTrueIfScriptExists) {
  auto entity = state.scene.entityDatabase.create();
  auto scriptHandle = assetCache.getRegistry().add<quoll::LuaScriptAsset>({});

  quoll::editor::EntitySetScript action(entity, scriptHandle);
  EXPECT_TRUE(action.predicate(state, assetCache));
}

using EntitySetScriptVariableActionTest = ActionTestBase;

TEST_F(EntitySetScriptVariableActionTest, ExecutorSetsScriptVariableForEntity) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScript>(
      entity, {quoll::AssetHandle<quoll::LuaScriptAsset>{25}});

  quoll::editor::EntitySetScriptVariable action(
      entity, "var1", quoll::AssetHandle<quoll::PrefabAsset>{15});

  auto res = action.onExecute(state, assetCache);

  const auto &variables =
      state.scene.entityDatabase.get<quoll::LuaScript>(entity).variables;

  EXPECT_EQ(variables.size(), 1);
  EXPECT_EQ(variables.at("var1").get<quoll::AssetHandle<quoll::PrefabAsset>>(),
            quoll::AssetHandle<quoll::PrefabAsset>{15});

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetScriptVariableActionTest,
       UndoSetsPreviousScriptVariableForEntity) {
  auto entity = state.scene.entityDatabase.create();

  quoll::LuaScript script;
  script.handle = quoll::AssetHandle<quoll::LuaScriptAsset>{45};
  script.variables.insert({"var1", quoll::AssetHandle<quoll::PrefabAsset>{25}});
  state.scene.entityDatabase.set(entity, script);

  quoll::editor::EntitySetScriptVariable action(
      entity, "var1", quoll::AssetHandle<quoll::PrefabAsset>{15});

  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  const auto &variables =
      state.scene.entityDatabase.get<quoll::LuaScript>(entity).variables;

  EXPECT_EQ(variables.size(), 1);
  EXPECT_EQ(variables.at("var1").get<quoll::AssetHandle<quoll::PrefabAsset>>(),
            quoll::AssetHandle<quoll::PrefabAsset>{25});

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
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScript>(
      entity, {quoll::AssetHandle<quoll::LuaScriptAsset>{15}});

  quoll::editor::EntitySetScriptVariable action(entity, "test",
                                                quoll::String("Hello world"));
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfVariableDoesNotExistInScriptAsset) {
  quoll::AssetData<quoll::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1",
      quoll::LuaScriptVariable{quoll::LuaScriptVariableType::String, "var1"});
  auto handle = assetCache.getRegistry().add(asset);

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScript>(entity, {handle});

  quoll::editor::EntitySetScriptVariable action(entity, "var2",
                                                quoll::String("Hello world"));
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfVariableTypeDoesNotMatchTypeInScriptAsset) {
  quoll::AssetData<quoll::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1", quoll::LuaScriptVariable{
                  quoll::LuaScriptVariableType::AssetPrefab, "var1"});
  auto handle = assetCache.getRegistry().add(asset);

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScript>(entity, {handle});

  quoll::editor::EntitySetScriptVariable action(entity, "var1",
                                                quoll::String("Test value"));
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfPrefabVariableDoesNotExistInRegistry) {
  quoll::AssetData<quoll::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1", quoll::LuaScriptVariable{
                  quoll::LuaScriptVariableType::AssetPrefab, "var1"});
  auto handle = assetCache.getRegistry().add(asset);

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScript>(entity, {handle});

  quoll::editor::EntitySetScriptVariable action(
      entity, "var1", quoll::AssetHandle<quoll::PrefabAsset>{25});
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetScriptVariableActionTest, PredicateReturnsTrueIfValidVariable) {
  quoll::AssetData<quoll::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1", quoll::LuaScriptVariable{
                  quoll::LuaScriptVariableType::AssetPrefab, "var1"});
  auto handle = assetCache.getRegistry().add(asset);

  auto prefabHandle = assetCache.getRegistry().add<quoll::PrefabAsset>({});

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::LuaScript>(entity, {handle});

  quoll::editor::EntitySetScriptVariable action(entity, "var1", prefabHandle);
  EXPECT_TRUE(action.predicate(state, assetCache));
}
