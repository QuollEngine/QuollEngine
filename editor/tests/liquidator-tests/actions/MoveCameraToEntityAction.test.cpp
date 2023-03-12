#include "liquid/core/Base.h"
#include "liquidator/actions/MoveCameraToEntityAction.h"

#include "liquidator-tests/Testing.h"

class MoveCameraToEntityActionTest : public ::testing::Test {
public:
  MoveCameraToEntityActionTest() {
    state.scene.entityDatabase.reg<liquid::editor::CameraLookAt>();
  }

  liquid::AssetRegistry registry;
  liquid::editor::WorkspaceState state{registry};
};

TEST_F(MoveCameraToEntityActionTest,
       ExecuteFailsIfProvidedArgumentIsNotEntity) {
  EXPECT_THROW(
      liquid::editor::MoveCameraToEntityAction.onExecute(state, "test"),
      std::bad_any_cast);
}

TEST_F(MoveCameraToEntityActionTest,
       ExecuteFailsIfProvidedEntityDoesNotHaveWorldTransform) {
  auto entity = state.scene.entityDatabase.create();
  EXPECT_DEATH(
      liquid::editor::MoveCameraToEntityAction.onExecute(state, entity), ".*");
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

  liquid::editor::MoveCameraToEntityAction.onExecute(state, entity);

  const auto &lookAt =
      state.scene.entityDatabase.get<liquid::editor::CameraLookAt>(
          state.camera);

  EXPECT_EQ(lookAt.up, glm::vec3(0.0f, 1.0f, 0.0));
  EXPECT_EQ(lookAt.center, glm::vec3(world[3]));
  EXPECT_EQ(lookAt.eye, glm::vec3(world[3]) - glm::vec3(0.0f, 0.0f, 10.0f));
}
