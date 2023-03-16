#include "liquid/core/Base.h"
#include "liquidator/actions/EntityCameraActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetPerspectiveLensActionTest = ActionTestBase;

TEST_P(EntitySetPerspectiveLensActionTest,
       ExecutorSetsPerspectiveLensAndCameraComponentForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetPerspectiveLens action(entity, {45.0f});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::PerspectiveLens>(entity).fovY,
      45.0f);
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Camera>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(
    EntitySetPerspectiveLensActionTest,
    ExecutorSetsPerspectiveLensDoesNotCreateNewCameraComponentIfAlreadyExists) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(entity, {glm::mat4{0.2f}});

  liquid::editor::EntitySetPerspectiveLens action(entity, {45.0f});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::PerspectiveLens>(entity).fovY,
      45.0f);
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::Camera>(entity).projectionMatrix,
      glm::mat4{0.2f});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntitySetPerspectiveLensActionTest);

using SetEntityPerspectiveLensAutoAspectRatioTest = ActionTestBase;

TEST_P(SetEntityPerspectiveLensAutoAspectRatioTest,
       ExecutorSetPerspectiveLensAutoAspectRatioComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetCameraAutoAspectRatio action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(
      activeScene().entityDatabase.has<liquid::AutoAspectRatio>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(SetEntityPerspectiveLensAutoAspectRatioTest,
       PredicateReturnsTrueIfEntityHasNoAutoAspectRatioComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetCameraAutoAspectRatio action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(SetEntityPerspectiveLensAutoAspectRatioTest,
       PredicateReturnsFalseIfEntityHasAutoAspectRatioComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  liquid::editor::EntitySetCameraAutoAspectRatio action(entity);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest,
                     SetEntityPerspectiveLensAutoAspectRatioTest);

using SetEntityPerspectiveLensCustomAspectRatioTest = ActionTestBase;

TEST_P(SetEntityPerspectiveLensCustomAspectRatioTest,
       ExecutorRemovesAutoAspectRatioComponentForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  liquid::editor::EntitySetCameraCustomAspectRatio action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::AutoAspectRatio>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(SetEntityPerspectiveLensCustomAspectRatioTest,
       PredicateReturnsTrueIfEntityHasAutoAspectRatioComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  liquid::editor::EntitySetCameraCustomAspectRatio action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(SetEntityPerspectiveLensCustomAspectRatioTest,
       PredicateReturnsFalseIfEntityHasNoAutoAspectRatioComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetCameraCustomAspectRatio action(entity);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest,
                     SetEntityPerspectiveLensCustomAspectRatioTest);
