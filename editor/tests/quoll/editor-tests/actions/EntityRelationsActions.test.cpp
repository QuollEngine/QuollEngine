#include "quoll/core/Base.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/Parent.h"
#include "quoll/editor/actions/EntityRelationActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetParentTest = ActionTestBase;

TEST_F(EntitySetParentTest, ExecutorSetsParentForEntityAndChildrenForParent) {
  auto entity = state.scene.entityDatabase.create();
  auto parent = state.scene.entityDatabase.create();

  quoll::editor::EntitySetParent action(entity, parent);
  auto res = action.onExecute(state, assetCache);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 2);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), parent);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Parent>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Parent>(entity).parent,
            parent);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Children>(parent));
  EXPECT_EQ(
      state.scene.entityDatabase.get<quoll::Children>(parent).children.size(),
      1);
  EXPECT_EQ(
      state.scene.entityDatabase.get<quoll::Children>(parent).children.at(0),
      entity);
}

TEST_F(
    EntitySetParentTest,
    ExecutorChildrenOfPreviousParentOnExecuteIfPreviousParentOnlyHasEntityAsChildren) {
  auto entity = state.scene.entityDatabase.create();
  auto p1 = state.scene.entityDatabase.create();
  auto p2 = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<quoll::Parent>(entity, {p1});
  state.scene.entityDatabase.set<quoll::Children>(p1, {{entity}});

  quoll::editor::EntitySetParent action(entity, p2);
  auto res = action.onExecute(state, assetCache);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Parent>(entity).parent, p2);

  // Entity removed from previous parent
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Children>(p1));

  // Entity added to new parent
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Children>(p2));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Children>(p2).children.size(),
            1);
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Children>(p2).children.at(0),
            entity);
}

TEST_F(
    EntitySetParentTest,
    ExecutorRemovesEntityFromChildrenOfParentIfEntityIsOneOfChildrenOfParent) {
  auto entity = state.scene.entityDatabase.create();
  auto child0 = state.scene.entityDatabase.create();

  auto p1 = state.scene.entityDatabase.create();
  auto p2 = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<quoll::Parent>(entity, {p1});
  state.scene.entityDatabase.set<quoll::Children>(p1, {{entity, child0}});
  state.scene.entityDatabase.set<quoll::Children>(p2, {{}});

  quoll::editor::EntitySetParent action(entity, p2);
  auto res = action.onExecute(state, assetCache);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Parent>(entity).parent, p2);

  // Entity removed from previous parent
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Children>(p1));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Children>(p1).children.size(),
            1);
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Children>(p1).children.at(0),
            child0);

  // Entity added to new parent
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Children>(p2));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Children>(p2).children.size(),
            1);
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Children>(p2).children.at(0),
            entity);
}

TEST_F(EntitySetParentTest, UndoRemovesEntityParentIfEntityDidNotHaveParent) {
  auto entity = state.scene.entityDatabase.create();
  auto parent = state.scene.entityDatabase.create();

  quoll::editor::EntitySetParent action(entity, parent);
  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  EXPECT_EQ(res.entitiesToSave.size(), 2);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), parent);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Parent>(entity));
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Children>(parent));
}

TEST_F(
    EntitySetParentTest,
    UndoSetsPreviousParentAndRemovesParentChildrenIfEntityIsTheOnlyChildOfCurrentParent) {
  auto entity = state.scene.entityDatabase.create();
  auto p1 = state.scene.entityDatabase.create();
  auto p2 = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<quoll::Parent>(entity, {p1});
  state.scene.entityDatabase.set<quoll::Children>(p1, {{entity}});

  quoll::editor::EntitySetParent action(entity, p2);
  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Parent>(entity).parent, p1);

  // Entity removed from previous parent
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Children>(p2));

  // Entity added to new parent
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Children>(p1));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Children>(p1).children.size(),
            1);
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Children>(p1).children.at(0),
            entity);
}

TEST_F(
    EntitySetParentTest,
    UndoSetsPreviousParentAndRemovesEntityFromCurrentParentIfEntityIsOneOfTheChildrenOfCurrentParent) {
  auto entity = state.scene.entityDatabase.create();
  auto child0 = state.scene.entityDatabase.create();

  auto p1 = state.scene.entityDatabase.create();
  auto p2 = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<quoll::Parent>(entity, {p1});
  state.scene.entityDatabase.set<quoll::Children>(p1, {{entity}});
  state.scene.entityDatabase.set<quoll::Children>(p2, {{child0}});

  quoll::editor::EntitySetParent action(entity, p2);
  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Parent>(entity).parent, p1);

  // Entity removed from previous parent
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Children>(p2));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Children>(p2).children.size(),
            1);
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Children>(p2).children.at(0),
            child0);

  // Entity added to new parent
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Children>(p1));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Children>(p1).children.size(),
            1);
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Children>(p1).children.at(0),
            entity);
}

TEST_F(EntitySetParentTest, PredicateReturnsFalseIfParentDoesNotExist) {
  auto entity = state.scene.entityDatabase.create();

  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, quoll::Entity{25})
                   .predicate(state, assetCache));
}

TEST_F(EntitySetParentTest, PredicateReturnsFalseIfParentIsAChildOfEntity) {
  auto entity = state.scene.entityDatabase.create();
  auto child0 = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<quoll::Parent>(child0, {entity});

  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, child0)
                   .predicate(state, assetCache));
}

TEST_F(EntitySetParentTest,
       PredicateReturnsFalseIfParentIsADescendantOfEntity) {
  auto entity = state.scene.entityDatabase.create();
  auto child0 = state.scene.entityDatabase.create();
  auto child1 = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<quoll::Parent>(child1, {child0});
  state.scene.entityDatabase.set<quoll::Parent>(child0, {entity});

  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, child1)
                   .predicate(state, assetCache));
}

TEST_F(EntitySetParentTest,
       PredicateReturnsFalseIfParentIsAlreadyParentOfEntity) {
  auto entity = state.scene.entityDatabase.create();
  auto parent = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<quoll::Parent>(entity, {parent});
  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, parent)
                   .predicate(state, assetCache));
}

TEST_F(EntitySetParentTest, PredicateReturnsFalseIfParentIsEntityItself) {
  auto entity = state.scene.entityDatabase.create();

  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, entity)
                   .predicate(state, assetCache));
}

TEST_F(EntitySetParentTest, PredicateReturnsTrueIfEntityParentIsValid) {
  auto entity = state.scene.entityDatabase.create();
  auto parent = state.scene.entityDatabase.create();

  EXPECT_TRUE(quoll::editor::EntitySetParent(entity, parent)
                  .predicate(state, assetCache));
}

using EntityRemoveParentTest = ActionTestBase;

TEST_F(EntityRemoveParentTest,
       ExecutorRemovesParentFromEntityAndChildFromParent) {
  auto entity = state.scene.entityDatabase.create();
  auto parent = state.scene.entityDatabase.create();
  auto c1 = state.scene.entityDatabase.create();
  auto c2 = state.scene.entityDatabase.create();
  auto c3 = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<quoll::Parent>(entity, {parent});
  state.scene.entityDatabase.set<quoll::Children>(parent,
                                                  {{c1, entity, c2, c3}});

  quoll::editor::EntityRemoveParent action(entity);
  auto res = action.onExecute(state, assetCache);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Parent>(entity));
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Children>(parent));

  const auto &children =
      state.scene.entityDatabase.get<quoll::Children>(parent).children;

  EXPECT_EQ(children.size(), 3);
  EXPECT_EQ(children.at(0), c1);
  EXPECT_EQ(children.at(1), c2);
  EXPECT_EQ(children.at(2), c3);
}

TEST_F(
    EntityRemoveParentTest,
    ExecutorRemovesParentFromEntityAndChildrenFromParentIfEntityWasTheOnlyChild) {
  auto entity = state.scene.entityDatabase.create();
  auto parent = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<quoll::Parent>(entity, {parent});
  state.scene.entityDatabase.set<quoll::Children>(parent, {{entity}});

  quoll::editor::EntityRemoveParent action(entity);
  auto res = action.onExecute(state, assetCache);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Parent>(entity));
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Children>(parent));
}

TEST_F(EntityRemoveParentTest,
       UndoAddsParentToEntityAndAddsEntityAsChildOfParentInTheSameSpot) {
  auto entity = state.scene.entityDatabase.create();
  auto parent = state.scene.entityDatabase.create();
  auto c1 = state.scene.entityDatabase.create();
  auto c2 = state.scene.entityDatabase.create();
  auto c3 = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<quoll::Parent>(entity, {parent});
  state.scene.entityDatabase.set<quoll::Children>(parent,
                                                  {{c1, entity, c2, c3}});

  quoll::editor::EntityRemoveParent action(entity);
  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Parent>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Parent>(entity).parent,
            parent);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Children>(parent));

  const auto &children =
      state.scene.entityDatabase.get<quoll::Children>(parent).children;

  EXPECT_EQ(children.size(), 4);
  EXPECT_EQ(children.at(0), c1);
  EXPECT_EQ(children.at(1), entity);
  EXPECT_EQ(children.at(2), c2);
  EXPECT_EQ(children.at(3), c3);
}

TEST_F(EntityRemoveParentTest,
       UndoAddsParentToEntityAndAddsEntityAsChildOfParentIfItWasTheOnlyChild) {
  auto entity = state.scene.entityDatabase.create();
  auto parent = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<quoll::Parent>(entity, {parent});
  state.scene.entityDatabase.set<quoll::Children>(parent, {{entity}});

  quoll::editor::EntityRemoveParent action(entity);
  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Parent>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Parent>(entity).parent,
            parent);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Children>(parent));

  const auto &children =
      state.scene.entityDatabase.get<quoll::Children>(parent).children;

  EXPECT_EQ(children.size(), 1);
  EXPECT_EQ(children.at(0), entity);
}

TEST_F(EntityRemoveParentTest, PredicateReturnsTrueIfEntityHasParent) {
  auto entity = state.scene.entityDatabase.create();
  auto parent = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Parent>(entity, {parent});

  EXPECT_TRUE(
      quoll::editor::EntityRemoveParent(entity).predicate(state, assetCache));
}

TEST_F(EntityRemoveParentTest, PredicateReturnsFalseIfEntityDoesNotHaveParent) {
  auto entity = state.scene.entityDatabase.create();

  EXPECT_FALSE(
      quoll::editor::EntityRemoveParent(entity).predicate(state, assetCache));
}
