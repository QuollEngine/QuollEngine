#include "liquid/core/Base.h"
#include "liquidator/actions/EntityRelationActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetParentTest = ActionTestBase;

TEST_F(EntitySetParentTest, ExecutorSetsParentForEntityAndChildrenForParent) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();

  liquid::editor::EntitySetParent action(entity, parent);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 2);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), parent);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Parent>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Parent>(entity).parent,
            parent);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Children>(parent));
  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::Children>(parent)
                .children.size(),
            1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(parent).children.at(0),
      entity);
}

TEST_F(
    EntitySetParentTest,
    ExecutorChildrenOfPreviousParentOnExecuteIfPreviousParentOnlyHasEntityAsChildren) {
  auto entity = activeScene().entityDatabase.create();
  auto p1 = activeScene().entityDatabase.create();
  auto p2 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<liquid::Parent>(entity, {p1});
  activeScene().entityDatabase.set<liquid::Children>(p1, {{entity}});

  liquid::editor::EntitySetParent action(entity, p2);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Parent>(entity).parent,
            p2);

  // Entity removed from previous parent
  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Children>(p1));

  // Entity added to new parent
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Children>(p2));
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(p2).children.size(),
      1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(p2).children.at(0),
      entity);
}

TEST_F(
    EntitySetParentTest,
    ExecutorRemovesEntityFromChildrenOfParentIfEntityIsOneOfChildrenOfParent) {
  auto entity = activeScene().entityDatabase.create();
  auto child0 = activeScene().entityDatabase.create();

  auto p1 = activeScene().entityDatabase.create();
  auto p2 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<liquid::Parent>(entity, {p1});
  activeScene().entityDatabase.set<liquid::Children>(p1, {{entity, child0}});
  activeScene().entityDatabase.set<liquid::Children>(p2, {{}});

  liquid::editor::EntitySetParent action(entity, p2);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Parent>(entity).parent,
            p2);

  // Entity removed from previous parent
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Children>(p1));
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(p1).children.size(),
      1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(p1).children.at(0),
      child0);

  // Entity added to new parent
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Children>(p2));
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(p2).children.size(),
      1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(p2).children.at(0),
      entity);
}

TEST_F(EntitySetParentTest, UndoRemovesEntityParentIfEntityDidNotHaveParent) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();

  liquid::editor::EntitySetParent action(entity, parent);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 2);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), parent);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Parent>(entity));
  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Children>(parent));
}

TEST_F(
    EntitySetParentTest,
    UndoSetsPreviousParentAndRemovesParentChildrenIfEntityIsTheOnlyChildOfCurrentParent) {
  auto entity = activeScene().entityDatabase.create();
  auto p1 = activeScene().entityDatabase.create();
  auto p2 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<liquid::Parent>(entity, {p1});
  activeScene().entityDatabase.set<liquid::Children>(p1, {{entity}});

  liquid::editor::EntitySetParent action(entity, p2);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Parent>(entity).parent,
            p1);

  // Entity removed from previous parent
  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Children>(p2));

  // Entity added to new parent
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Children>(p1));
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(p1).children.size(),
      1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(p1).children.at(0),
      entity);
}

TEST_F(
    EntitySetParentTest,
    UndoSetsPreviousParentAndRemovesEntityFromCurrentParentIfEntityIsOneOfTheChildrenOfCurrentParent) {
  auto entity = activeScene().entityDatabase.create();
  auto child0 = activeScene().entityDatabase.create();

  auto p1 = activeScene().entityDatabase.create();
  auto p2 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<liquid::Parent>(entity, {p1});
  activeScene().entityDatabase.set<liquid::Children>(p1, {{entity}});
  activeScene().entityDatabase.set<liquid::Children>(p2, {{child0}});

  liquid::editor::EntitySetParent action(entity, p2);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Parent>(entity).parent,
            p1);

  // Entity removed from previous parent
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Children>(p2));
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(p2).children.size(),
      1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(p2).children.at(0),
      child0);

  // Entity added to new parent
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Children>(p1));
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(p1).children.size(),
      1);
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Children>(p1).children.at(0),
      entity);
}

TEST_F(EntitySetParentTest, PredicateReturnsFalseIfParentDoesNotExist) {
  auto entity = activeScene().entityDatabase.create();

  EXPECT_FALSE(liquid::editor::EntitySetParent(entity, liquid::Entity{25})
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetParentTest, PredicateReturnsFalseIfParentIsAChildOfEntity) {
  auto entity = activeScene().entityDatabase.create();
  auto child0 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<liquid::Parent>(child0, {entity});

  EXPECT_FALSE(liquid::editor::EntitySetParent(entity, child0)
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetParentTest,
       PredicateReturnsFalseIfParentIsADescendantOfEntity) {
  auto entity = activeScene().entityDatabase.create();
  auto child0 = activeScene().entityDatabase.create();
  auto child1 = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<liquid::Parent>(child1, {child0});
  activeScene().entityDatabase.set<liquid::Parent>(child0, {entity});

  EXPECT_FALSE(liquid::editor::EntitySetParent(entity, child1)
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetParentTest,
       PredicateReturnsFalseIfParentIsAlreadyParentOfEntity) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<liquid::Parent>(entity, {parent});
  EXPECT_FALSE(liquid::editor::EntitySetParent(entity, parent)
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetParentTest, PredicateReturnsFalseIfParentIsEntityItself) {
  auto entity = activeScene().entityDatabase.create();

  EXPECT_FALSE(liquid::editor::EntitySetParent(entity, entity)
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetParentTest, PredicateReturnsTrueIfEntityParentIsValid) {
  auto entity = activeScene().entityDatabase.create();
  auto parent = activeScene().entityDatabase.create();

  EXPECT_TRUE(liquid::editor::EntitySetParent(entity, parent)
                  .predicate(state, assetRegistry));
}
