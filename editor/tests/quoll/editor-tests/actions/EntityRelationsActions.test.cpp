#include "quoll/core/Base.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/Parent.h"
#include "quoll/editor/actions/EntityRelationActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetParentTest = ActionTestBase;

TEST_F(EntitySetParentTest, ExecutorSetsParentForEntityAndChildrenForParent) {
  auto entity = state.scene.entityDatabase.entity();
  auto parent = state.scene.entityDatabase.entity();

  quoll::editor::EntitySetParent action(entity, parent);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 2);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), parent);

  EXPECT_TRUE(entity.has<quoll::Parent>());
  EXPECT_EQ(entity.get_ref<quoll::Parent>()->parent, parent);

  EXPECT_TRUE(parent.has<quoll::Children>());
  EXPECT_EQ(parent.get_ref<quoll::Children>()->children.size(), 1);
  EXPECT_EQ(parent.get_ref<quoll::Children>()->children.at(0), entity);
}

TEST_F(
    EntitySetParentTest,
    ExecutorChildrenOfPreviousParentOnExecuteIfPreviousParentOnlyHasEntityAsChildren) {
  auto entity = state.scene.entityDatabase.entity();
  auto p1 = state.scene.entityDatabase.entity();
  auto p2 = state.scene.entityDatabase.entity();

  entity.set<quoll::Parent>({p1});
  p1.set<quoll::Children>({{entity}});

  quoll::editor::EntitySetParent action(entity, p2);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(entity.get_ref<quoll::Parent>()->parent, p2);

  // Entity removed from previous parent
  EXPECT_FALSE(p1.has<quoll::Children>());

  // Entity added to new parent
  EXPECT_TRUE(p2.has<quoll::Children>());
  EXPECT_EQ(p2.get_ref<quoll::Children>()->children.size(), 1);
  EXPECT_EQ(p2.get_ref<quoll::Children>()->children.at(0), entity);
}

TEST_F(
    EntitySetParentTest,
    ExecutorRemovesEntityFromChildrenOfParentIfEntityIsOneOfChildrenOfParent) {
  auto entity = state.scene.entityDatabase.entity();
  auto child0 = state.scene.entityDatabase.entity();

  auto p1 = state.scene.entityDatabase.entity();
  auto p2 = state.scene.entityDatabase.entity();

  entity.set<quoll::Parent>({p1});
  p1.set<quoll::Children>({{entity, child0}});
  p2.set<quoll::Children>({{}});

  quoll::editor::EntitySetParent action(entity, p2);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(entity.get_ref<quoll::Parent>()->parent, p2);

  // Entity removed from previous parent
  EXPECT_TRUE(p1.has<quoll::Children>());
  EXPECT_EQ(p1.get_ref<quoll::Children>()->children.size(), 1);
  EXPECT_EQ(p1.get_ref<quoll::Children>()->children.at(0), child0);

  // Entity added to new parent
  EXPECT_TRUE(p2.has<quoll::Children>());
  EXPECT_EQ(p2.get_ref<quoll::Children>()->children.size(), 1);
  EXPECT_EQ(p2.get_ref<quoll::Children>()->children.at(0), entity);
}

TEST_F(EntitySetParentTest, UndoRemovesEntityParentIfEntityDidNotHaveParent) {
  auto entity = state.scene.entityDatabase.entity();
  auto parent = state.scene.entityDatabase.entity();

  quoll::editor::EntitySetParent action(entity, parent);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 2);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), parent);

  EXPECT_FALSE(entity.has<quoll::Parent>());
  EXPECT_FALSE(parent.has<quoll::Children>());
}

TEST_F(
    EntitySetParentTest,
    UndoSetsPreviousParentAndRemovesParentChildrenIfEntityIsTheOnlyChildOfCurrentParent) {
  auto entity = state.scene.entityDatabase.entity();
  auto p1 = state.scene.entityDatabase.entity();
  auto p2 = state.scene.entityDatabase.entity();

  entity.set<quoll::Parent>({p1});
  p1.set<quoll::Children>({{entity}});

  quoll::editor::EntitySetParent action(entity, p2);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(entity.get_ref<quoll::Parent>()->parent, p1);

  // Entity removed from previous parent
  EXPECT_FALSE(p2.has<quoll::Children>());

  // Entity added to new parent
  EXPECT_TRUE(p1.has<quoll::Children>());
  EXPECT_EQ(p1.get_ref<quoll::Children>()->children.size(), 1);
  EXPECT_EQ(p1.get_ref<quoll::Children>()->children.at(0), entity);
}

TEST_F(
    EntitySetParentTest,
    UndoSetsPreviousParentAndRemovesEntityFromCurrentParentIfEntityIsOneOfTheChildrenOfCurrentParent) {
  auto entity = state.scene.entityDatabase.entity();
  auto child0 = state.scene.entityDatabase.entity();

  auto p1 = state.scene.entityDatabase.entity();
  auto p2 = state.scene.entityDatabase.entity();

  entity.set<quoll::Parent>({p1});
  p1.set<quoll::Children>({{entity}});
  p2.set<quoll::Children>({{child0}});

  quoll::editor::EntitySetParent action(entity, p2);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_EQ(res.entitiesToSave.at(1), p2);
  EXPECT_EQ(res.entitiesToSave.at(2), p1);

  // Entity parent updated
  EXPECT_EQ(entity.get_ref<quoll::Parent>()->parent, p1);

  // Entity removed from previous parent
  EXPECT_TRUE(p2.has<quoll::Children>());
  EXPECT_EQ(p2.get_ref<quoll::Children>()->children.size(), 1);
  EXPECT_EQ(p2.get_ref<quoll::Children>()->children.at(0), child0);

  // Entity added to new parent
  EXPECT_TRUE(p1.has<quoll::Children>());
  EXPECT_EQ(p1.get_ref<quoll::Children>()->children.size(), 1);
  EXPECT_EQ(p1.get_ref<quoll::Children>()->children.at(0), entity);
}

TEST_F(EntitySetParentTest, PredicateReturnsFalseIfParentDoesNotExist) {
  auto entity = state.scene.entityDatabase.entity();

  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, quoll::Entity{25})
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetParentTest, PredicateReturnsFalseIfParentIsAChildOfEntity) {
  auto entity = state.scene.entityDatabase.entity();
  auto child0 = state.scene.entityDatabase.entity();

  child0.set<quoll::Parent>({entity});

  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, child0)
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetParentTest,
       PredicateReturnsFalseIfParentIsADescendantOfEntity) {
  auto entity = state.scene.entityDatabase.entity();
  auto child0 = state.scene.entityDatabase.entity();
  auto child1 = state.scene.entityDatabase.entity();

  child1.set<quoll::Parent>({child0});
  child0.set<quoll::Parent>({entity});

  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, child1)
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetParentTest,
       PredicateReturnsFalseIfParentIsAlreadyParentOfEntity) {
  auto entity = state.scene.entityDatabase.entity();
  auto parent = state.scene.entityDatabase.entity();

  entity.set<quoll::Parent>({parent});
  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, parent)
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetParentTest, PredicateReturnsFalseIfParentIsEntityItself) {
  auto entity = state.scene.entityDatabase.entity();

  EXPECT_FALSE(quoll::editor::EntitySetParent(entity, entity)
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetParentTest, PredicateReturnsTrueIfEntityParentIsValid) {
  auto entity = state.scene.entityDatabase.entity();
  auto parent = state.scene.entityDatabase.entity();

  EXPECT_TRUE(quoll::editor::EntitySetParent(entity, parent)
                  .predicate(state, assetRegistry));
}

using EntityRemoveParentTest = ActionTestBase;

TEST_F(EntityRemoveParentTest,
       ExecutorRemovesParentFromEntityAndChildFromParent) {
  auto entity = state.scene.entityDatabase.entity();
  auto parent = state.scene.entityDatabase.entity();
  auto c1 = state.scene.entityDatabase.entity();
  auto c2 = state.scene.entityDatabase.entity();
  auto c3 = state.scene.entityDatabase.entity();

  entity.set<quoll::Parent>({parent});
  parent.set<quoll::Children>({{c1, entity, c2, c3}});

  quoll::editor::EntityRemoveParent action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);

  EXPECT_FALSE(entity.has<quoll::Parent>());
  EXPECT_TRUE(parent.has<quoll::Children>());

  const auto &children = parent.get_ref<quoll::Children>()->children;

  EXPECT_EQ(children.size(), 3);
  EXPECT_EQ(children.at(0), c1);
  EXPECT_EQ(children.at(1), c2);
  EXPECT_EQ(children.at(2), c3);
}

TEST_F(
    EntityRemoveParentTest,
    ExecutorRemovesParentFromEntityAndChildrenFromParentIfEntityWasTheOnlyChild) {
  auto entity = state.scene.entityDatabase.entity();
  auto parent = state.scene.entityDatabase.entity();

  entity.set<quoll::Parent>({parent});
  parent.set<quoll::Children>({{entity}});

  quoll::editor::EntityRemoveParent action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);

  EXPECT_FALSE(entity.has<quoll::Parent>());
  EXPECT_FALSE(parent.has<quoll::Children>());
}

TEST_F(EntityRemoveParentTest,
       UndoAddsParentToEntityAndAddsEntityAsChildOfParentInTheSameSpot) {
  auto entity = state.scene.entityDatabase.entity();
  auto parent = state.scene.entityDatabase.entity();
  auto c1 = state.scene.entityDatabase.entity();
  auto c2 = state.scene.entityDatabase.entity();
  auto c3 = state.scene.entityDatabase.entity();

  entity.set<quoll::Parent>({parent});
  parent.set<quoll::Children>({{c1, entity, c2, c3}});

  quoll::editor::EntityRemoveParent action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);

  EXPECT_TRUE(entity.has<quoll::Parent>());
  EXPECT_EQ(entity.get_ref<quoll::Parent>()->parent, parent);

  EXPECT_TRUE(parent.has<quoll::Children>());

  const auto &children = parent.get_ref<quoll::Children>()->children;

  EXPECT_EQ(children.size(), 4);
  EXPECT_EQ(children.at(0), c1);
  EXPECT_EQ(children.at(1), entity);
  EXPECT_EQ(children.at(2), c2);
  EXPECT_EQ(children.at(3), c3);
}

TEST_F(EntityRemoveParentTest,
       UndoAddsParentToEntityAndAddsEntityAsChildOfParentIfItWasTheOnlyChild) {
  auto entity = state.scene.entityDatabase.entity();
  auto parent = state.scene.entityDatabase.entity();

  entity.set<quoll::Parent>({parent});
  parent.set<quoll::Children>({{entity}});

  quoll::editor::EntityRemoveParent action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);

  EXPECT_TRUE(entity.has<quoll::Parent>());
  EXPECT_EQ(entity.get_ref<quoll::Parent>()->parent, parent);

  EXPECT_TRUE(parent.has<quoll::Children>());

  const auto &children = parent.get_ref<quoll::Children>()->children;

  EXPECT_EQ(children.size(), 1);
  EXPECT_EQ(children.at(0), entity);
}

TEST_F(EntityRemoveParentTest, PredicateReturnsTrueIfEntityHasParent) {
  auto entity = state.scene.entityDatabase.entity();
  auto parent = state.scene.entityDatabase.entity();
  entity.set<quoll::Parent>({parent});

  EXPECT_TRUE(quoll::editor::EntityRemoveParent(entity).predicate(
      state, assetRegistry));
}

TEST_F(EntityRemoveParentTest, PredicateReturnsFalseIfEntityDoesNotHaveParent) {
  auto entity = state.scene.entityDatabase.entity();

  EXPECT_FALSE(quoll::editor::EntityRemoveParent(entity).predicate(
      state, assetRegistry));
}
