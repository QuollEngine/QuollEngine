#pragma once

// Default create component tests
#define InitDefaultCreateComponentTests(SuiteName, ActionName, ComponentName)  \
  TEST_P(SuiteName, ExecutorCreatesComponentForEntity) {                       \
    auto entity = activeScene().entityDatabase.create();                       \
    quoll::editor::ActionName action(entity);                                  \
    auto res = action.onExecute(state, assetRegistry);                         \
    EXPECT_TRUE(                                                               \
        activeScene().entityDatabase.has<quoll::ComponentName>(entity));       \
    EXPECT_EQ(res.entitiesToSave.at(0), entity);                               \
    EXPECT_TRUE(res.addToHistory);                                             \
  }                                                                            \
                                                                               \
  TEST_P(SuiteName, UndoDeletesComponentFromEntity) {                          \
    auto entity = activeScene().entityDatabase.create();                       \
    activeScene().entityDatabase.set<quoll::ComponentName>(entity, {});        \
    quoll::editor::ActionName action(entity);                                  \
    auto res = action.onUndo(state, assetRegistry);                            \
    EXPECT_FALSE(                                                              \
        activeScene().entityDatabase.has<quoll::ComponentName>(entity));       \
    EXPECT_EQ(res.entitiesToSave.at(0), entity);                               \
  }                                                                            \
                                                                               \
  TEST_P(SuiteName, PredicateReturnsFalseIfComponentAlreadyExists) {           \
    auto entity = activeScene().entityDatabase.create();                       \
    activeScene().entityDatabase.set<quoll::ComponentName>(entity, {});        \
    EXPECT_FALSE(                                                              \
        quoll::editor::ActionName(entity).predicate(state, assetRegistry));    \
  }                                                                            \
                                                                               \
  TEST_P(SuiteName, PredicateReturnsTrueIfComponentDoesNotExist) {             \
    auto entity = activeScene().entityDatabase.create();                       \
    EXPECT_TRUE(                                                               \
        quoll::editor::ActionName(entity).predicate(state, assetRegistry));    \
  }

// Default delete component tests
#define InitDefaultDeleteComponentTests(SuiteName, ActionName, ComponentName)  \
  TEST_P(SuiteName, ExecutorDeletesComponentFromEntity) {                      \
    auto entity = activeScene().entityDatabase.create();                       \
    activeScene().entityDatabase.set<quoll::ComponentName>(entity, {});        \
    quoll::editor::ActionName action(entity);                                  \
    auto res = action.onExecute(state, assetRegistry);                         \
    EXPECT_FALSE(                                                              \
        activeScene().entityDatabase.has<quoll::ComponentName>(entity));       \
    ASSERT_EQ(res.entitiesToSave.size(), 1);                                   \
    EXPECT_EQ(res.entitiesToSave.at(0), entity);                               \
    EXPECT_TRUE(res.addToHistory);                                             \
  }                                                                            \
                                                                               \
  TEST_P(SuiteName, PredicateReturnsTrueIfEntityHasComponent) {                \
    auto entity = activeScene().entityDatabase.create();                       \
    activeScene().entityDatabase.set<quoll::ComponentName>(entity, {});        \
    quoll::editor::ActionName action(entity);                                  \
    EXPECT_TRUE(action.predicate(state, assetRegistry));                       \
  }                                                                            \
                                                                               \
  TEST_P(SuiteName, PredicateReturnsFalseIfEntityDoesNotHaveComponent) {       \
    auto entity = activeScene().entityDatabase.create();                       \
    quoll::editor::ActionName action(entity);                                  \
    EXPECT_FALSE(action.predicate(state, assetRegistry));                      \
  }                                                                            \
                                                                               \
  TEST_P(SuiteName, UndoCreatesOldComponentForEntity) {                        \
    auto entity = activeScene().entityDatabase.create();                       \
    activeScene().entityDatabase.set<quoll::ComponentName>(entity, {});        \
    quoll::editor::ActionName action(entity);                                  \
    action.onExecute(state, assetRegistry);                                    \
    EXPECT_FALSE(                                                              \
        activeScene().entityDatabase.has<quoll::ComponentName>(entity));       \
    auto res = action.onUndo(state, assetRegistry);                            \
    EXPECT_TRUE(                                                               \
        activeScene().entityDatabase.has<quoll::ComponentName>(entity));       \
    ASSERT_EQ(res.entitiesToSave.size(), 1);                                   \
    EXPECT_EQ(res.entitiesToSave.at(0), entity);                               \
  }

// Default update component tests
#define InitDefaultUpdateComponentTests(SuiteName, ActionName, ComponentName,  \
                                        TestPropertyName, TestPropertyValue)   \
  TEST_P(SuiteName, ExecutorSetsNewComponentForEntity) {                       \
    auto entity = activeScene().entityDatabase.create();                       \
    quoll::ComponentName component{};                                          \
    component.TestPropertyName = TestPropertyValue;                            \
                                                                               \
    quoll::editor::ActionName action(entity, {});                              \
    action.setNewComponent(component);                                         \
    auto res = action.onExecute(state, assetRegistry);                         \
                                                                               \
    EXPECT_EQ(activeScene()                                                    \
                  .entityDatabase.get<quoll::ComponentName>(entity)            \
                  .TestPropertyName,                                           \
              TestPropertyValue);                                              \
    EXPECT_EQ(res.entitiesToSave.at(0), entity);                               \
    EXPECT_TRUE(res.addToHistory);                                             \
  }                                                                            \
                                                                               \
  TEST_P(SuiteName, UndoSetsOldComponentForEntity) {                           \
    auto entity = activeScene().entityDatabase.create();                       \
    quoll::ComponentName component{};                                          \
    component.TestPropertyName = TestPropertyValue;                            \
                                                                               \
    quoll::editor::ActionName action(entity, component);                       \
    auto res = action.onUndo(state, assetRegistry);                            \
                                                                               \
    EXPECT_EQ(activeScene()                                                    \
                  .entityDatabase.get<quoll::ComponentName>(entity)            \
                  .TestPropertyName,                                           \
              TestPropertyValue);                                              \
    EXPECT_EQ(res.entitiesToSave.at(0), entity);                               \
  }                                                                            \
                                                                               \
  TEST_P(SuiteName, PredicateReturnsTrueIfNewComponentIsNotEmpty) {            \
    auto entity = activeScene().entityDatabase.create();                       \
                                                                               \
    quoll::editor::ActionName action(entity, {});                              \
    action.setNewComponent({});                                                \
    EXPECT_TRUE(action.predicate(state, assetRegistry));                       \
  }                                                                            \
                                                                               \
  TEST_P(SuiteName, PredicateReturnsFalseIfNewComponentIsEmpty) {              \
    auto entity = activeScene().entityDatabase.create();                       \
                                                                               \
    quoll::editor::ActionName action(entity, {});                              \
    EXPECT_FALSE(action.predicate(state, assetRegistry));                      \
  }
