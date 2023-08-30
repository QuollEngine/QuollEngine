#include "liquid/core/Base.h"
#include "liquidator/actions/EntityScriptingActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateScriptActionTest = ActionTestBase;

TEST_P(EntityCreateScriptActionTest, ExecutorCreatesScriptSourceComponent) {
  auto entity = activeScene().entityDatabase.create();

  quoll::editor::EntityCreateScript action(entity,
                                           quoll::LuaScriptAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Script>(entity).handle,
            quoll::LuaScriptAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityCreateScriptActionTest, UndoDeletesScriptSourceComponet) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Script>(
      entity, {quoll::LuaScriptAssetHandle{25}});

  quoll::editor::EntityCreateScript action(entity,
                                           quoll::LuaScriptAssetHandle{15});
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Script>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityCreateScriptActionTest,
       PredicateReturnsFalseIfScriptAssetIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntityCreateScript action(entity,
                                           quoll::LuaScriptAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntityCreateScriptActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasScript) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntityCreateScript action(entity,
                                           quoll::LuaScriptAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntityCreateScriptActionTest,
       PredicateReturnsTrueIfScriptDoesNotExistAndAssetIsValid) {
  auto entity = activeScene().entityDatabase.create();
  auto handle = assetRegistry.getLuaScripts().addAsset({});

  quoll::editor::EntityCreateScript action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntityCreateScriptActionTest);

using EntitySetScriptActionTest = ActionTestBase;

TEST_P(EntitySetScriptActionTest, ExecutorSetsScriptForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Script>(entity, {});

  quoll::editor::EntitySetScript action(entity,
                                        quoll::LuaScriptAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Script>(entity).handle,
            quoll::LuaScriptAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetScriptActionTest, UndoSetsPreviousScriptForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Script>(
      entity, {quoll::LuaScriptAssetHandle{25}});

  quoll::editor::EntitySetScript action(entity,
                                        quoll::LuaScriptAssetHandle{15});
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Script>(entity).handle,
            quoll::LuaScriptAssetHandle{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetScriptActionTest, PredicateReturnsFalseIfScriptIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntitySetScript action(entity,
                                        quoll::LuaScriptAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetScriptActionTest, PredicateReturnsTrueIfScriptExists) {
  auto entity = activeScene().entityDatabase.create();
  auto scriptHandle = assetRegistry.getLuaScripts().addAsset({});

  quoll::editor::EntitySetScript action(entity, scriptHandle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntitySetScriptActionTest);

using EntitySetScriptVariableActionTest = ActionTestBase;

TEST_P(EntitySetScriptVariableActionTest, ExecutorSetsScriptVariableForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Script>(
      entity, {quoll::LuaScriptAssetHandle{25}});

  quoll::editor::EntitySetScriptVariable action(entity, "var1",
                                                quoll::PrefabAssetHandle{15});

  auto res = action.onExecute(state, assetRegistry);

  const auto &variables =
      activeScene().entityDatabase.get<quoll::Script>(entity).variables;

  EXPECT_EQ(variables.size(), 1);
  EXPECT_EQ(variables.at("var1").get<quoll::PrefabAssetHandle>(),
            quoll::PrefabAssetHandle{15});

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetScriptVariableActionTest,
       UndoSetsPreviousScriptVariableForEntity) {
  auto entity = activeScene().entityDatabase.create();

  quoll::Script script;
  script.handle = quoll::LuaScriptAssetHandle{45};
  script.variables.insert({"var1", quoll::PrefabAssetHandle{25}});
  activeScene().entityDatabase.set(entity, script);

  quoll::editor::EntitySetScriptVariable action(entity, "var1",
                                                quoll::PrefabAssetHandle{15});

  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  const auto &variables =
      activeScene().entityDatabase.get<quoll::Script>(entity).variables;

  EXPECT_EQ(variables.size(), 1);
  EXPECT_EQ(variables.at("var1").get<quoll::PrefabAssetHandle>(),
            quoll::PrefabAssetHandle{25});

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfEntityHasNoScript) {
  auto entity = activeScene().entityDatabase.create();

  quoll::editor::EntitySetScriptVariable action(entity, "test",
                                                quoll::String("Hello world"));

  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfEntityScriptIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Script>(
      entity, {quoll::LuaScriptAssetHandle{15}});

  quoll::editor::EntitySetScriptVariable action(entity, "test",
                                                quoll::String("Hello world"));
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfVariableDoesNotExistInScriptAsset) {
  quoll::AssetData<quoll::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1",
      quoll::LuaScriptVariable{quoll::LuaScriptVariableType::String, "var1"});
  auto handle = assetRegistry.getLuaScripts().addAsset(asset);

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Script>(entity, {handle});

  quoll::editor::EntitySetScriptVariable action(entity, "var2",
                                                quoll::String("Hello world"));
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfVariableTypeDoesNotMatchTypeInScriptAsset) {
  quoll::AssetData<quoll::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1", quoll::LuaScriptVariable{
                  quoll::LuaScriptVariableType::AssetPrefab, "var1"});
  auto handle = assetRegistry.getLuaScripts().addAsset(asset);

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Script>(entity, {handle});

  quoll::editor::EntitySetScriptVariable action(entity, "var1",
                                                quoll::String("Test value"));
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfPrefabVariableDoesNotExistInRegistry) {
  quoll::AssetData<quoll::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1", quoll::LuaScriptVariable{
                  quoll::LuaScriptVariableType::AssetPrefab, "var1"});
  auto handle = assetRegistry.getLuaScripts().addAsset(asset);

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Script>(entity, {handle});

  quoll::editor::EntitySetScriptVariable action(entity, "var1",
                                                quoll::PrefabAssetHandle{25});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetScriptVariableActionTest, PredicateReturnsTrueIfValidVariable) {
  quoll::AssetData<quoll::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1", quoll::LuaScriptVariable{
                  quoll::LuaScriptVariableType::AssetPrefab, "var1"});
  auto handle = assetRegistry.getLuaScripts().addAsset(asset);

  auto prefabHandle = assetRegistry.getPrefabs().addAsset({});

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Script>(entity, {handle});

  quoll::editor::EntitySetScriptVariable action(entity, "var1", prefabHandle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntitySetScriptVariableActionTest);

using EntityDeleteScriptActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteScriptActionTest,
                                EntityDeleteScript, Script);
InitActionsTestSuite(EntityActionsTest, EntityDeleteScriptActionTest);
