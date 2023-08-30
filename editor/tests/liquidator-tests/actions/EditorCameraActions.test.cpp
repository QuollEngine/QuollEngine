#include "liquid/core/Base.h"
#include "liquidator/actions/EditorCameraActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

class MoveCameraToEntityActionTest : public ActionTestBase {
public:
  MoveCameraToEntityActionTest() {
    state.scene.entityDatabase.reg<quoll::editor::CameraLookAt>();
  }
};

TEST_F(MoveCameraToEntityActionTest,
       ExecuteFailsIfProvidedEntityDoesNotHaveWorldTransform) {
  auto entity = state.scene.entityDatabase.create();

  quoll::editor::MoveCameraToEntity action(entity);

  EXPECT_DEATH(action.onExecute(state, assetRegistry), ".*");
}

TEST_F(MoveCameraToEntityActionTest,
       ExecuteSetsCameraPositionToEntityPosition) {
  state.camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::editor::CameraLookAt>(state.camera, {});

  glm::mat4 world{};
  world[3] = glm::vec4{2.5f, 1.5f, 3.5f, -10.0f};

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::WorldTransform>(entity, {world});

  quoll::editor::MoveCameraToEntity action(entity);
  action.onExecute(state, assetRegistry);

  const auto &lookAt =
      state.scene.entityDatabase.get<quoll::editor::CameraLookAt>(state.camera);

  EXPECT_EQ(lookAt.up, glm::vec3(0.0f, 1.0f, 0.0));
  EXPECT_EQ(lookAt.center, glm::vec3(world[3]));
  EXPECT_EQ(lookAt.eye, glm::vec3(world[3]) - glm::vec3(0.0f, 0.0f, 10.0f));
}
