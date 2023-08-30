#include "quoll/core/Base.h"
#include "liquidator/actions/EntityRelationActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetParentTest = ActionTestBase;

TEST_P(EntitySetParentTest, ExecutorSetsParentForEntityAndChildrenForParent) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();

  quoll::editor::EntitySetParent action(entity, parent);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 2);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), parent);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Parent>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Parent>(entity).parent,
            parent);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Children>(parent));
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(parent).children.size(),
      1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(parent).children.at(0),
      entity);
}

TEST_P(
    EntitySetParentTest,
    ExecutorChildrenOfPreviousParentOnExecuteIfPreviousParentOnlyHasEntityAsChildren) {
  auto entity = activeScene().entityDatabase.create();
  auto p1 = activeScene().entityDatabase.create();
  auto p2 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::Parent>(entity, {p1});
  activeScene().entityDatabase.set<quoll::Children>(p1, {{entity}});

  quoll::editor::EntitySetParent action(entity, p2);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Parent>(entity).parent, p2);

  // Entity removed from previous parent
  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Children>(p1));

  // Entity added to new parent
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Children>(p2));
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(p2).children.size(), 1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(p2).children.at(0),
      entity);
}

TEST_P(
    EntitySetParentTest,
    ExecutorRemovesEntityFromChildrenOfParentIfEntityIsOneOfChildrenOfParent) {
  auto entity = activeScene().entityDatabase.create();
  auto child0 = activeScene().entityDatabase.create();

  auto p1 = activeScene().entityDatabase.create();
  auto p2 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::Parent>(entity, {p1});
  activeScene().entityDatabase.set<quoll::Children>(p1, {{entity, child0}});
  activeScene().entityDatabase.set<quoll::Children>(p2, {{}});

  quoll::editor::EntitySetParent action(entity, p2);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Parent>(entity).parent, p2);

  // Entity removed from previous parent
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Children>(p1));
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(p1).children.size(), 1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(p1).children.at(0),
      child0);

  // Entity added to new parent
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Children>(p2));
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(p2).children.size(), 1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(p2).children.at(0),
      entity);
}

TEST_P(EntitySetParentTest, UndoRemovesEntityParentIfEntityDidNotHaveParent) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();

  quoll::editor::EntitySetParent action(entity, parent);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 2);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), parent);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Parent>(entity));
  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Children>(parent));
}

TEST_P(
    EntitySetParentTest,
    UndoSetsPreviousParentAndRemovesParentChildrenIfEntityIsTheOnlyChildOfCurrentParent) {
  auto entity = activeScene().entityDatabase.create();
  auto p1 = activeScene().entityDatabase.create();
  auto p2 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::Parent>(entity, {p1});
  activeScene().entityDatabase.set<quoll::Children>(p1, {{entity}});

  quoll::editor::EntitySetParent action(entity, p2);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Parent>(entity).parent, p1);

  // Entity removed from previous parent
  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Children>(p2));

  // Entity added to new parent
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Children>(p1));
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(p1).children.size(), 1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(p1).children.at(0),
      entity);
}

TEST_P(
    EntitySetParentTest,
    UndoSetsPreviousParentAndRemovesEntityFromCurrentParentIfEntityIsOneOfTheChildrenOfCurrentParent) {
  auto entity = activeScene().entityDatabase.create();
  auto child0 = activeScene().entityDatabase.create();

  auto p1 = activeScene().entityDatabase.create();
  auto p2 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::Parent>(entity, {p1});
  activeScene().entityDatabase.set<quoll::Children>(p1, {{entity}});
  activeScene().entityDatabase.set<quoll::Children>(p2, {{child0}});

  quoll::editor::EntitySetParent action(entity, p2);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Parent>(entity).parent, p1);

  // Entity removed from previous parent
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Children>(p2));
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(p2).children.size(), 1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(p2).children.at(0),
      child0);

  // Entity added to new parent
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Children>(p1));
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(p1).children.size(), 1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<quoll::Children>(p1).children.at(0),
      entity);
}

TEST_P(EntitySetParentTest, PredicateReturnsFalseIfParentDoesNotExist) {
  auto entity = activeScene().entityDatabase.create();

  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, quoll::Entity{25})
                   .predicate(state, assetRegistry));
}

TEST_P(EntitySetParentTest, PredicateReturnsFalseIfParentIsAChildOfEntity) {
  auto entity = activeScene().entityDatabase.create();
  auto child0 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::Parent>(child0, {entity});

  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, child0)
                   .predicate(state, assetRegistry));
}

TEST_P(EntitySetParentTest,
       PredicateReturnsFalseIfParentIsADescendantOfEntity) {
  auto entity = activeScene().entityDatabase.create();
  auto child0 = activeScene().entityDatabase.create();
  auto child1 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::Parent>(child1, {child0});
  activeScene().entityDatabase.set<quoll::Parent>(child0, {entity});

  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, child1)
                   .predicate(state, assetRegistry));
}

TEST_P(EntitySetParentTest,
       PredicateReturnsFalseIfParentIsAlreadyParentOfEntity) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::Parent>(entity, {parent});
  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, parent)
                   .predicate(state, assetRegistry));
}

TEST_P(EntitySetParentTest, PredicateReturnsFalseIfParentIsEntityItself) {
  auto entity = activeScene().entityDatabase.create();

  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, entity)
                   .predicate(state, assetRegistry));
}

TEST_P(EntitySetParentTest, PredicateReturnsTrueIfEntityParentIsValid) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();

  EXPECT_TRUE(quoll::editor::EntitySetParent(entity, parent)
                  .predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntitySetParentTest);

using EntityRemoveParentTest = ActionTestBase;

TEST_P(EntityRemoveParentTest,
       ExecutorRemovesParentFromEntityAndChildFromParent) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();
  auto c1 = activeScene().entityDatabase.create();
  auto c2 = activeScene().entityDatabase.create();
  auto c3 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::Parent>(entity, {parent});
  activeScene().entityDatabase.set<quoll::Children>(parent,
                                                    {{c1, entity, c2, c3}});

  quoll::editor::EntityRemoveParent action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Parent>(entity));
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Children>(parent));

  const auto &children =
      activeScene().entityDatabase.get<quoll::Children>(parent).children;

  EXPECT_EQ(children.size(), 3);
  EXPECT_EQ(children.at(0), c1);
  EXPECT_EQ(children.at(1), c2);
  EXPECT_EQ(children.at(2), c3);
}

TEST_P(
    EntityRemoveParentTest,
    ExecutorRemovesParentFromEntityAndChildrenFromParentIfEntityWasTheOnlyChild) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::Parent>(entity, {parent});
  activeScene().entityDatabase.set<quoll::Children>(parent, {{entity}});

  quoll::editor::EntityRemoveParent action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Parent>(entity));
  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Children>(parent));
}

TEST_P(EntityRemoveParentTest,
       UndoAddsParentToEntityAndAddsEntityAsChildOfParentInTheSameSpot) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();
  auto c1 = activeScene().entityDatabase.create();
  auto c2 = activeScene().entityDatabase.create();
  auto c3 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::Parent>(entity, {parent});
  activeScene().entityDatabase.set<quoll::Children>(parent,
                                                    {{c1, entity, c2, c3}});

  quoll::editor::EntityRemoveParent action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Parent>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Parent>(entity).parent,
            parent);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Children>(parent));

  const auto &children =
      activeScene().entityDatabase.get<quoll::Children>(parent).children;

  EXPECT_EQ(children.size(), 4);
  EXPECT_EQ(children.at(0), c1);
  EXPECT_EQ(children.at(1), entity);
  EXPECT_EQ(children.at(2), c2);
  EXPECT_EQ(children.at(3), c3);
}

TEST_P(EntityRemoveParentTest,
       UndoAddsParentToEntityAndAddsEntityAsChildOfParentIfItWasTheOnlyChild) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::Parent>(entity, {parent});
  activeScene().entityDatabase.set<quoll::Children>(parent, {{entity}});

  quoll::editor::EntityRemoveParent action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Parent>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Parent>(entity).parent,
            parent);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Children>(parent));

  const auto &children =
      activeScene().entityDatabase.get<quoll::Children>(parent).children;

  EXPECT_EQ(children.size(), 1);
  EXPECT_EQ(children.at(0), entity);
}

TEST_P(EntityRemoveParentTest, PredicateReturnsTrueIfEntityHasParent) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Parent>(entity, {parent});

  EXPECT_TRUE(quoll::editor::EntityRemoveParent(entity).predicate(
      state, assetRegistry));
}

TEST_P(EntityRemoveParentTest, PredicateReturnsFalseIfEntityDoesNotHaveParent) {
  auto entity = activeScene().entityDatabase.create();

  EXPECT_FALSE(quoll::editor::EntityRemoveParent(entity).predicate(
      state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntityRemoveParentTest);
