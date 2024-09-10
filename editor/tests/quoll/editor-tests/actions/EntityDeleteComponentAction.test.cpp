#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
#include "quoll/core/Name.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/editor/actions/EntityDeleteComponentAction.h"
#include "quoll-tests/Testing.h"
#include "ActionTestBase.h"

using EntityDeleteComponentActionTest = ActionTestBase;

TEST_F(EntityDeleteComponentActionTest,
       PredicateReturnsTrueIfEntityHasProvidedComponent) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  EXPECT_TRUE(action.predicate(state, assetCache));
}

TEST_F(EntityDeleteComponentActionTest,
       PredicateReturnsTrueIfEntityHasProvidedComponentButNoOtherComponents) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name, quoll::Id> action(entity);

  EXPECT_TRUE(action.predicate(state, assetCache));
}

TEST_F(EntityDeleteComponentActionTest,
       PredicateReturnsFalseIfEntityDoesNotHaveProvidedComponent) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(
    EntityDeleteComponentActionTest,
    PredicateReturnsFalseIfEntityDoesNotHaveProvidedComponentButHasOtherComponents) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Id>(entity, {});
  quoll::editor::EntityDeleteComponent<quoll::Name, quoll::Id> action(entity);

  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntityDeleteComponentActionTest, PredicateReturnsFalse) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name, quoll::Id> action(entity);

  EXPECT_TRUE(action.predicate(state, assetCache));
}

TEST_F(EntityDeleteComponentActionTest, ExecutorDeletesComponentFromEntity) {
  state.scene.entityDatabase.create();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  auto res = action.onExecute(state, assetCache);
  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Name>(entity));
}

TEST_F(EntityDeleteComponentActionTest,
       ExecutorDeletesOtherComponentsFromEntity) {
  state.scene.entityDatabase.create();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Hello"});
  state.scene.entityDatabase.set<quoll::Id>(entity, {});
  state.scene.entityDatabase.set<quoll::LocalTransform>(entity, {});

  quoll::editor::EntityDeleteComponent<quoll::Name, quoll::Id,
                                       quoll::LocalTransform>
      action(entity);

  auto res = action.onExecute(state, assetCache);
  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Name>(entity));
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Id>(entity));
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::LocalTransform>(entity));
}

TEST_F(EntityDeleteComponentActionTest, UndoAddsPreviousComponentToEntity) {
  state.scene.entityDatabase.create();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  action.onExecute(state, assetCache);
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Name>(entity));

  auto res = action.onUndo(state, assetCache);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Name>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Name>(entity).name, "Hello");
}

TEST_F(EntityDeleteComponentActionTest,
       UndoDoesNotAddPreviousOtherComponentsToEntity) {
  state.scene.entityDatabase.create();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Hello"});
  state.scene.entityDatabase.set<quoll::Id>(entity, {});
  state.scene.entityDatabase.set<quoll::LocalTransform>(entity, {});
  quoll::editor::EntityDeleteComponent<quoll::Name, quoll::Id,
                                       quoll::LocalTransform>
      action(entity);

  action.onExecute(state, assetCache);
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Name>(entity));
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Id>(entity));
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::LocalTransform>(entity));

  auto res = action.onUndo(state, assetCache);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Name>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Name>(entity).name, "Hello");
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Id>(entity));
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::LocalTransform>(entity));
}
