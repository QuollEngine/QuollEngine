#include "liquid/core/Base.h"
#include "liquidator/actions/CreateEmptyEntityAtViewAction.h"

#include "liquidator-tests/Testing.h"

class CreateEmptyEntityAtViewActionTest : public ::testing::Test {
public:
  CreateEmptyEntityAtViewActionTest() {
    auto c1 = state.simulationScene.entityDatabase.create();
    auto c2 = state.scene.entityDatabase.create();

    LIQUID_ASSERT(c1 == c2, "Camera entities have different IDs");
    state.camera = c1;

    glm::mat4 viewMatrix =
        glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                    glm::vec3{0.0f, 1.0f, 0.0f});

    liquid::Camera camera{};
    camera.viewMatrix = viewMatrix;
    state.scene.entityDatabase.set(state.camera, camera);
    state.simulationScene.entityDatabase.set(state.camera, camera);
  }

  liquid::AssetRegistry registry;
  liquid::editor::WorkspaceState state{{}, registry};
};

TEST_F(CreateEmptyEntityAtViewActionTest,
       ExecuteCreatesEmptyEntityInSceneWhenModeIsEdit) {
  liquid::editor::CreateEmptyEntityAtViewAction action;
  auto res = action.onExecute(state);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_NE(res.entitiesToSave.at(0), liquid::Entity::Null);

  auto entity = res.entitiesToSave.at(0);
  EXPECT_TRUE(state.scene.entityDatabase.exists(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<liquid::Name>(entity).name,
            "New entity");
  EXPECT_EQ(state.scene.entityDatabase.get<liquid::LocalTransform>(entity)
                .localPosition,
            glm::vec3(0.0f, 0.0f, -10.0f));

  EXPECT_EQ(state.scene.entityDatabase.get<liquid::WorldTransform>(entity)
                .worldTransform,
            glm::mat4(1.0f));
}

TEST_F(CreateEmptyEntityAtViewActionTest,
       ExecuteCreatesEmptyEntityInSimulatorSceneWhenModeIsSimulation) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  liquid::editor::CreateEmptyEntityAtViewAction action;
  auto res = action.onExecute(state);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_NE(res.entitiesToSave.at(0), liquid::Entity::Null);

  auto entity = res.entitiesToSave.at(0);

  EXPECT_TRUE(state.simulationScene.entityDatabase.exists(entity));
  EXPECT_EQ(
      state.simulationScene.entityDatabase.get<liquid::LocalTransform>(entity)
          .localPosition,
      glm::vec3(0.0f, 0.0f, -10.0f));
  EXPECT_EQ(state.simulationScene.entityDatabase.get<liquid::Name>(entity).name,
            "New entity");
  EXPECT_EQ(
      state.simulationScene.entityDatabase.get<liquid::WorldTransform>(entity)
          .worldTransform,
      glm::mat4(1.0f));
}
