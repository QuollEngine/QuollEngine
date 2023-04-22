#include "liquid/core/Base.h"
#include "liquidator/actions/EntityScriptingActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetScriptActionTest = ActionTestBase;

TEST_P(EntitySetScriptActionTest, ExecutorSetsScriptForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetScript action(entity,
                                         liquid::LuaScriptAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Script>(entity).handle,
            liquid::LuaScriptAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetScriptActionTest, PredicateReturnsFalseIfScriptIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntitySetScript action(entity,
                                         liquid::LuaScriptAssetHandle{15});
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntitySetScriptActionTest, PredicateReturnsTrueIfScriptExists) {
  auto entity = activeScene().entityDatabase.create();
  auto scriptHandle = state.assetRegistry.getLuaScripts().addAsset({});

  liquid::editor::EntitySetScript action(entity, scriptHandle);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntitySetScriptActionTest);

using EntitySetScriptVariableActionTest = ActionTestBase;

TEST_P(EntitySetScriptVariableActionTest, ExecutorSetsScriptVariableForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(
      entity, {liquid::LuaScriptAssetHandle{25}});

  liquid::editor::EntitySetScriptVariable action(entity, "var1",
                                                 liquid::PrefabAssetHandle{15});

  auto res = action.onExecute(state);

  const auto &variables =
      activeScene().entityDatabase.get<liquid::Script>(entity).variables;

  EXPECT_EQ(variables.size(), 1);
  EXPECT_EQ(variables.at("var1").get<liquid::PrefabAssetHandle>(),
            liquid::PrefabAssetHandle{15});

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfEntityHasNoScript) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetScriptVariable action(entity, "test",
                                                 liquid::String("Hello world"));

  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfEntityScriptIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(
      entity, {liquid::LuaScriptAssetHandle{15}});

  liquid::editor::EntitySetScriptVariable action(entity, "test",
                                                 liquid::String("Hello world"));
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfVariableDoesNotExistInScriptAsset) {
  liquid::AssetData<liquid::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1",
      liquid::LuaScriptVariable{liquid::LuaScriptVariableType::String, "var1"});
  auto handle = state.assetRegistry.getLuaScripts().addAsset(asset);

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(entity, {handle});

  liquid::editor::EntitySetScriptVariable action(entity, "var2",
                                                 liquid::String("Hello world"));
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfVariableTypeDoesNotMatchTypeInScriptAsset) {
  liquid::AssetData<liquid::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1", liquid::LuaScriptVariable{
                  liquid::LuaScriptVariableType::AssetPrefab, "var1"});
  auto handle = state.assetRegistry.getLuaScripts().addAsset(asset);

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(entity, {handle});

  liquid::editor::EntitySetScriptVariable action(entity, "var1",
                                                 liquid::String("Test value"));
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsFalseIfPrefabVariableDoesNotExistInRegistry) {
  liquid::AssetData<liquid::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1", liquid::LuaScriptVariable{
                  liquid::LuaScriptVariableType::AssetPrefab, "var1"});
  auto handle = state.assetRegistry.getLuaScripts().addAsset(asset);

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(entity, {handle});

  liquid::editor::EntitySetScriptVariable action(entity, "var1",
                                                 liquid::PrefabAssetHandle{25});
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntitySetScriptVariableActionTest,
       PredicateReturnsTrueIfValidVariableTypeAndValud) {
  liquid::AssetData<liquid::LuaScriptAsset> asset{};
  asset.data.variables.insert_or_assign(
      "var1", liquid::LuaScriptVariable{
                  liquid::LuaScriptVariableType::AssetPrefab, "var1"});
  auto handle = state.assetRegistry.getLuaScripts().addAsset(asset);

  auto prefabHandle = state.assetRegistry.getPrefabs().addAsset({});

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(entity, {handle});

  liquid::editor::EntitySetScriptVariable action(entity, "var1", prefabHandle);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntitySetScriptVariableActionTest);

using EntityDeleteScriptActionTest = ActionTestBase;

TEST_P(EntityDeleteScriptActionTest, ExecutorDeletesScriptComponentFromEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(entity, {});

  liquid::editor::EntityDeleteScript action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Script>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityDeleteScriptActionTest,
       PredicateReturnsTrueIfEntityHasScriptComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Script>(entity, {});

  liquid::editor::EntityDeleteScript action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntityDeleteScriptActionTest,
       PredicateReturnsFalseIfEntityHasNoScriptComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityDeleteScript action(entity);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteScriptActionTest);
