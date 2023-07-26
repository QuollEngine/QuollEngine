#include "liquid/core/Base.h"
#include "liquidator/actions/EntityScriptingActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateScriptActionTest = ActionTestBase;

TEST_P(EntityCreateScriptActionTest, ExecutorCreatesScriptSourceComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityCreateScript action(entity,
                                            liquid::LuaScriptAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Script>(entity).handle,
            liquid::LuaScriptAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityCreateScriptActionTest, UndoDeletesScriptSourceComponet) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(
      entity, {liquid::LuaScriptAssetHandle{25}});

  liquid::editor::EntityCreateScript action(entity,
                                            liquid::LuaScriptAssetHandle{15});
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Script>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityCreateScriptActionTest,
       PredicateReturnsFalseIfScriptAssetIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntityCreateScript action(entity,
                                            liquid::LuaScriptAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntityCreateScriptActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasScript) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntityCreateScript action(entity,
                                            liquid::LuaScriptAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntityCreateScriptActionTest,
       PredicateReturnsTrueIfScriptDoesNotExistAndAssetIsValid) {
  auto entity = activeScene().entityDatabase.create();
  auto handle = assetRegistry.getLuaScripts().addAsset({});

  liquid::editor::EntityCreateScript action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntityCreateScriptActionTest);

using EntitySetScriptActionTest = ActionTestBase;

TEST_P(EntitySetScriptActionTest, ExecutorSetsScriptForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(entity, {});

  liquid::editor::EntitySetScript action(entity,
                                         liquid::LuaScriptAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Script>(entity).handle,
            liquid::LuaScriptAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetScriptActionTest, UndoSetsPreviousScriptForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(
      entity, {liquid::LuaScriptAssetHandle{25}});

  liquid::editor::EntitySetScript action(entity,
                                         liquid::LuaScriptAssetHandle{15});
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Script>(entity).handle,
            liquid::LuaScriptAssetHandle{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetScriptActionTest, PredicateReturnsFalseIfScriptIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntitySetScript action(entity,
                                         liquid::LuaScriptAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetScriptActionTest, PredicateReturnsTrueIfScriptExists) {
  auto entity = activeScene().entityDatabase.create();
  auto scriptHandle = assetRegistry.getLuaScripts().addAsset({});

  liquid::editor::EntitySetScript action(entity, scriptHandle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntitySetScriptActionTest);

using EntitySetScriptVariableActionTest = ActionTestBase;

TEST_P(EntitySetScriptVariableActionTest, ExecutorSetsScriptVariableForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(
      entity, {liquid::LuaScriptAssetHandle{25}});

  liquid::editor::EntitySetScriptVariable action(entity, "var1",
                                                 liquid::PrefabAssetHandle{15});

  auto res = action.onExecute(state, assetRegistry);

  const auto &variables =
      activeScene().entityDatabase.get<liquid::Script>(entity).variables;

  EXPECT_EQ(variables.size(), 1);
  EXPECT_EQ(variables.at("var1").get<liquid::PrefabAssetHandle>(),
            liquid::PrefabAssetHandle{15});

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetScriptVariableActionTest,
       UndoSetsPreviousScriptVariableForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::Script script;
  script.handle = liquid::LuaScriptAssetHandle{45};
  script.variables.insert({"var1", liquid::PrefabAssetHandle{25}});
  activeScene().entityDatabase.set(entity, script);

  liquid::editor::EntitySetScriptVariable action(entity, "var1",
                                                 liquid::PrefabAssetHandle{15});

  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  const auto &variables =
      activeScene().entityDatabase.get<liquid::Script>(entity).variables;

  EXPECT_EQ(variables.size(), 1);
  EXPECT_EQ(variables.at("var1").get<liquid::PrefabAssetHandle>(),
            liquid::PrefabAssetHandle{25});

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfEntityHasNoScript) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetScriptVariable action(entity, "test",
                                                 liquid::String("Hello world"));

  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfEntityScriptIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(
      entity, {liquid::LuaScriptAssetHandle{15}});

  liquid::editor::EntitySetScriptVariable action(entity, "test",
                                                 liquid::String("Hello world"));
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfVariableDoesNotExistInScriptAsset) {
  liquid::AssetData<liquid::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1",
      liquid::LuaScriptVariable{liquid::LuaScriptVariableType::String, "var1"});
  auto handle = assetRegistry.getLuaScripts().addAsset(asset);

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(entity, {handle});

  liquid::editor::EntitySetScriptVariable action(entity, "var2",
                                                 liquid::String("Hello world"));
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfVariableTypeDoesNotMatchTypeInScriptAsset) {
  liquid::AssetData<liquid::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1", liquid::LuaScriptVariable{
                  liquid::LuaScriptVariableType::AssetPrefab, "var1"});
  auto handle = assetRegistry.getLuaScripts().addAsset(asset);

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(entity, {handle});

  liquid::editor::EntitySetScriptVariable action(entity, "var1",
                                                 liquid::String("Test value"));
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfPrefabVariableDoesNotExistInRegistry) {
  liquid::AssetData<liquid::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1", liquid::LuaScriptVariable{
                  liquid::LuaScriptVariableType::AssetPrefab, "var1"});
  auto handle = assetRegistry.getLuaScripts().addAsset(asset);

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(entity, {handle});

  liquid::editor::EntitySetScriptVariable action(entity, "var1",
                                                 liquid::PrefabAssetHandle{25});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetScriptVariableActionTest, PredicateReturnsTrueIfValidVariable) {
  liquid::AssetData<liquid::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1", liquid::LuaScriptVariable{
                  liquid::LuaScriptVariableType::AssetPrefab, "var1"});
  auto handle = assetRegistry.getLuaScripts().addAsset(asset);

  auto prefabHandle = assetRegistry.getPrefabs().addAsset({});

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(entity, {handle});

  liquid::editor::EntitySetScriptVariable action(entity, "var1", prefabHandle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntitySetScriptVariableActionTest);

using EntityDeleteScriptActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteScriptActionTest,
                                EntityDeleteScript, Script);
InitActionsTestSuite(EntityActionsTest, EntityDeleteScriptActionTest);
