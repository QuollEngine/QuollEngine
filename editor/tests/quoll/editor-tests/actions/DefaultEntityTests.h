#pragma once

// Default delete component tests
#define InitDefaultDeleteComponentTests(SuiteName, ActionName, ComponentName)  \
  TEST_F(SuiteName, ExecutorDeletesComponentFromEntity) {                      \
    auto entity = state.scene.entityDatabase.create();                         \
    state.scene.entityDatabase.set<quoll::ComponentName>(entity, {});          \
    quoll::editor::ActionName action(entity);                                  \
    auto res = action.onExecute(state, assetRegistry);                         \
    EXPECT_FALSE(                                                              \
        state.scene.entityDatabase.has<quoll::ComponentName>(entity));         \
    ASSERT_EQ(res.entitiesToSave.size(), 1);                                   \
    EXPECT_EQ(res.entitiesToSave.at(0), entity);                               \
    EXPECT_TRUE(res.addToHistory);                                             \
  }                                                                            \
                                                                               \
  TEST_F(SuiteName, PredicateReturnsTrueIfEntityHasComponent) {                \
    auto entity = state.scene.entityDatabase.create();                         \
    state.scene.entityDatabase.set<quoll::ComponentName>(entity, {});          \
    quoll::editor::ActionName action(entity);                                  \
    EXPECT_TRUE(action.predicate(state, assetRegistry));                       \
  }                                                                            \
                                                                               \
  TEST_F(SuiteName, PredicateReturnsFalseIfEntityDoesNotHaveComponent) {       \
    auto entity = state.scene.entityDatabase.create();                         \
    quoll::editor::ActionName action(entity);                                  \
    EXPECT_FALSE(action.predicate(state, assetRegistry));                      \
  }                                                                            \
                                                                               \
  TEST_F(SuiteName, UndoCreatesOldComponentForEntity) {                        \
    auto entity = state.scene.entityDatabase.create();                         \
    state.scene.entityDatabase.set<quoll::ComponentName>(entity, {});          \
    quoll::editor::ActionName action(entity);                                  \
    action.onExecute(state, assetRegistry);                                    \
    EXPECT_FALSE(                                                              \
        state.scene.entityDatabase.has<quoll::ComponentName>(entity));         \
    auto res = action.onUndo(state, assetRegistry);                            \
    EXPECT_TRUE(state.scene.entityDatabase.has<quoll::ComponentName>(entity)); \
    ASSERT_EQ(res.entitiesToSave.size(), 1);                                   \
    EXPECT_EQ(res.entitiesToSave.at(0), entity);                               \
  }
