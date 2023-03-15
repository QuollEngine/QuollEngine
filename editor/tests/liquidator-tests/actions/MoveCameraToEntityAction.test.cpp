#include "liquid/core/Base.h"
#include "liquidator/actions/MoveCameraToEntityAction.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

class MoveCameraToEntityActionTest : public ActionTestBase {
public:
  MoveCameraToEntityActionTest() {
    state.scene.entityDatabase.reg<liquid::editor::CameraLookAt>();
  }
};

TEST_F(MoveCameraToEntityActionTest,
       ExecuteFailsIfProvidedEntityDoesNotHaveWorldTransform) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::MoveCameraToEntityAction action(entity);

  EXPECT_DEATH(action.onExecute(state), ".*");
}

TEST_F(MoveCameraToEntityActionTest,
       ExecuteSetsCameraPositionToEntityPosition) {
  state.camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::editor::CameraLookAt>(state.camera,
                                                               {});

  glm::mat4 world{};
  world[3] = glm::vec4{2.5f, 1.5f, 3.5f, -10.0f};

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::WorldTransform>(entity, {world});

  liquid::editor::MoveCameraToEntityAction action(entity);
  action.onExecute(state);

  const auto &lookAt =
      state.scene.entityDatabase.get<liquid::editor::CameraLookAt>(
          state.camera);

  EXPECT_EQ(lookAt.up, glm::vec3(0.0f, 1.0f, 0.0));
  EXPECT_EQ(lookAt.center, glm::vec3(world[3]));
  EXPECT_EQ(lookAt.eye, glm::vec3(world[3]) - glm::vec3(0.0f, 0.0f, 10.0f));
}
