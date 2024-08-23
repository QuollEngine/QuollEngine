#include "quoll/core/Base.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll/editor/actions/EditorCameraActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

class MoveCameraToEntityActionTest : public ActionTestBase {
public:
  MoveCameraToEntityActionTest() {
    state.scene.entityDatabase.component<quoll::editor::CameraLookAt>();
  }
};

TEST_F(MoveCameraToEntityActionTest,
       ExecuteFailsIfProvidedEntityDoesNotHaveWorldTransform) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::editor::MoveCameraToEntity action(entity);

  EXPECT_DEATH(action.onExecute(state, assetRegistry), ".*");
}

TEST_F(MoveCameraToEntityActionTest,
       ExecuteSetsCameraPositionToEntityPosition) {
  state.camera = state.scene.entityDatabase.entity();
  state.camera.set<quoll::editor::CameraLookAt>({});

  glm::mat4 world{};
  world[3] = glm::vec4{2.5f, 1.5f, 3.5f, -10.0f};

  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::WorldTransform>({world});

  quoll::editor::MoveCameraToEntity action(entity);
  action.onExecute(state, assetRegistry);

  auto lookAt = state.camera.get_ref<quoll::editor::CameraLookAt>();

  EXPECT_EQ(lookAt->up, glm::vec3(0.0f, 1.0f, 0.0));
  EXPECT_EQ(lookAt->center, glm::vec3(world[3]));
  EXPECT_EQ(lookAt->eye, glm::vec3(world[3]) - glm::vec3(0.0f, 0.0f, 10.0f));
}
