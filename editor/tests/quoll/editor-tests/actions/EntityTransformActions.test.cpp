#include "quoll/core/Base.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/WorldTransform.h"

#include "quoll/editor/actions/EntityTransformActions.h"

#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetLocalTransformContinousActionTest = ActionTestBase;

TEST_P(EntitySetLocalTransformContinousActionTest,
       ExecutorSetsFinalTransformForEntity) {
  auto entity = activeScene().entityDatabase.create();

  quoll::LocalTransform startTransform{glm::vec3{2.5f}};
  quoll::LocalTransform finalTransform{glm::vec3{5.5f}};

  quoll::editor::EntitySetLocalTransformContinuous action(entity,
                                                          startTransform);
  action.setNewComponent(finalTransform);

  auto res = action.onExecute(state, assetRegistry);
  EXPECT_EQ(activeScene()
                .entityDatabase.get<quoll::LocalTransform>(entity)
                .localPosition,
            glm::vec3(5.5f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetLocalTransformContinousActionTest,
       UndoSetsStartTransformForEntityWhereStartTransformIsDefined) {
  auto entity = activeScene().entityDatabase.create();

  quoll::LocalTransform startTransform{glm::vec3{2.5f}};
  quoll::LocalTransform finalTransform{glm::vec3{5.5f}};

  quoll::editor::EntitySetLocalTransformContinuous action(entity,
                                                          startTransform);

  auto res = action.onUndo(state, assetRegistry);
  EXPECT_EQ(activeScene()
                .entityDatabase.get<quoll::LocalTransform>(entity)
                .localPosition,
            glm::vec3(2.5f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetLocalTransformContinousActionTest,
       UndoRemovesTransformComponentFromEntityIfStartTransformIsNotDefined) {
  auto entity = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::LocalTransform>(entity, {});
  quoll::editor::EntitySetLocalTransformContinuous action(entity, std::nullopt);

  auto res = action.onUndo(state, assetRegistry);
  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::LocalTransform>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetLocalTransformContinousActionTest,
       UndoRemovesWorldTransfromFromEntityOnUndoIfStartTransformIsNotDefined) {
  auto entity = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<quoll::LocalTransform>(entity, {});
  activeScene().entityDatabase.set<quoll::WorldTransform>(entity, {});
  quoll::editor::EntitySetLocalTransformContinuous action(entity, std::nullopt);

  auto res = action.onUndo(state, assetRegistry);
  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::LocalTransform>(entity));
  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::WorldTransform>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetLocalTransformContinousActionTest,
       PredicateReturnsTrueIfActionHasFinalTransform) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntitySetLocalTransformContinuous action(
      entity, {}, quoll::LocalTransform{glm::vec3{5.5f}});

  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetLocalTransformContinousActionTest,
       PredicateReturnsFalseIfActionDoesNotHaveFinalTransform) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntitySetLocalTransformContinuous action(entity, {});

  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest,
                     EntitySetLocalTransformContinousActionTest);
