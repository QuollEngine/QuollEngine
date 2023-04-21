#include "liquid/core/Base.h"
#include "liquidator/actions/EntityRigidBodyActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetRigidBodyActionTest = ActionTestBase;

TEST_P(EntitySetRigidBodyActionTest, ExecutorSetsRigidBodyForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::RigidBody rigidBody{};
  rigidBody.dynamicDesc.mass = 5.0f;

  liquid::editor::EntitySetRigidBody action(entity, rigidBody);
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::RigidBody>(entity)
                .dynamicDesc.mass,
            5.0f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntitySetRigidBodyActionTest);

using EntityDeleteRigidBodyActionTest = ActionTestBase;

TEST_P(EntityDeleteRigidBodyActionTest,
       ExecutorDeletesRigidBodyComponentFromEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::RigidBody>(entity, {});

  liquid::editor::EntityDeleteRigidBody action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::RigidBody>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityDeleteRigidBodyActionTest,
       PredicateReturnsTrueIfEntityHasRigidBodyComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::RigidBody>(entity, {});

  liquid::editor::EntityDeleteRigidBody action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntityDeleteRigidBodyActionTest,
       PredicateReturnsFalseIfEntityHasNoRigidBodyComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityDeleteRigidBody action(entity);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteRigidBodyActionTest);
