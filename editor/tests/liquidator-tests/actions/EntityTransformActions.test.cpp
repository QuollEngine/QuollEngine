#include "liquid/core/Base.h"
#include "liquidator/actions/EntityTransformActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetLocalTransformActionTest = ActionTestBase;

TEST_P(EntitySetLocalTransformActionTest, ExecutorSetsLocalTransformForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::WorldTransform>(entity,
                                                           {glm::mat4{5.0f}});

  liquid::editor::EntitySetLocalTransform action(
      entity, liquid::LocalTransform{glm::vec3{2.5f}});

  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::LocalTransform>(entity)
                .localPosition,
            glm::vec3(2.5f));
  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::WorldTransform>(entity)
                .worldTransform,
            glm::mat4(5.0f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetLocalTransformActionTest,
       ExecutorCreatesWorldTransformForEntityIfItDoesNotHaveOne) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetLocalTransform action(
      entity, liquid::LocalTransform{glm::vec3{2.5f}});

  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::LocalTransform>(entity)
                .localPosition,
            glm::vec3(2.5f));
  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::WorldTransform>(entity)
                .worldTransform,
            glm::mat4(1.0f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntitySetLocalTransformActionTest);
