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
