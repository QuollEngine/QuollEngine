#include "liquid/core/Base.h"
#include "liquidator/actions/EntityTransformActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetLocalTransformContinousActionTest = ActionTestBase;

TEST_P(EntitySetLocalTransformContinousActionTest,
       ExecutorSetsFinalTransformForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::LocalTransform startTransform{glm::vec3{2.5f}};
  liquid::LocalTransform finalTransform{glm::vec3{5.5f}};

  liquid::editor::EntitySetLocalTransformContinuous action(entity,
                                                           startTransform);
  action.setNewComponent(finalTransform);

  auto res = action.onExecute(state);
  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::LocalTransform>(entity)
                .localPosition,
            glm::vec3(5.5f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetLocalTransformContinousActionTest,
       UndoSetsStartTransformForEntityWhereStartTransformIsDefined) {
  auto entity = activeScene().entityDatabase.create();

  liquid::LocalTransform startTransform{glm::vec3{2.5f}};
  liquid::LocalTransform finalTransform{glm::vec3{5.5f}};

  liquid::editor::EntitySetLocalTransformContinuous action(entity,
                                                           startTransform);

  auto res = action.onUndo(state);
  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::LocalTransform>(entity)
                .localPosition,
            glm::vec3(2.5f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetLocalTransformContinousActionTest,
       UndoRemovesTransformComponentFromEntityIfStartTransformIsNotDefined) {
  auto entity = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<liquid::LocalTransform>(entity, {});
  liquid::editor::EntitySetLocalTransformContinuous action(entity,
                                                           std::nullopt);

  auto res = action.onUndo(state);
  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::LocalTransform>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetLocalTransformContinousActionTest,
       UndoRemovesWorldTransfromFromEntityOnUndoIfStartTransformIsNotDefined) {
  auto entity = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<liquid::LocalTransform>(entity, {});
  activeScene().entityDatabase.set<liquid::WorldTransform>(entity, {});
  liquid::editor::EntitySetLocalTransformContinuous action(entity,
                                                           std::nullopt);

  auto res = action.onUndo(state);
  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::LocalTransform>(entity));
  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::WorldTransform>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetLocalTransformContinousActionTest,
       PredicateReturnsTrueIfActionHasFinalTransform) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntitySetLocalTransformContinuous action(
      entity, {}, liquid::LocalTransform{glm::vec3{5.5f}});

  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntitySetLocalTransformContinousActionTest,
       PredicateReturnsFalseIfActionDoesNotHaveFinalTransform) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntitySetLocalTransformContinuous action(entity, {});

  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest,
                     EntitySetLocalTransformContinousActionTest);
