#include "liquid/core/Base.h"
#include "liquidator/actions/ActionExecutor.h"

#include "liquidator-tests/Testing.h"

static const liquid::Path ScenePath{std::filesystem::current_path() / "scene"};

class ActionExecutorTest : public ::testing::Test {
public:
  void SetUp() override {
    std::filesystem::create_directories(ScenePath / "entities");

    YAML::Node scene;
    scene["persistentZone"] = 0;
    scene["zones"][0]["entities"] = (ScenePath / "entities").string();
    std::ofstream stream(ScenePath / "main.lqscene");
    stream << scene;
    stream.close();

    executor.getSceneIO().loadScene(ScenePath / "main.lqscene");
  }

  void TearDown() override { std::filesystem::remove_all(ScenePath); }

public:
  liquid::AssetRegistry registry;
  liquid::editor::WorkspaceState state{{}, registry};
  liquid::editor::ActionExecutor executor{state, ScenePath / "main.lqscene"};
};

TEST_F(ActionExecutorTest, ExecuteFailsIfActionHasNoExecutor) {
  liquid::editor::Action TestAction{"TestAction"};

  EXPECT_DEATH(executor.execute(TestAction, liquid::String("Hello world")),
               ".*");
}

TEST_F(ActionExecutorTest, ExecuteCallsActionExecutorWithStateAndData) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  bool called = false;
  liquid::editor::Action TestAction{
      "TestAction",
      [&called](liquid::editor::WorkspaceState &state, std::any data) mutable {
        called = true;
        EXPECT_EQ(std::any_cast<liquid::String>(data), "Hello world");
        EXPECT_EQ(state.mode, liquid::editor::WorkspaceMode::Simulation);
        return liquid::editor::ActionExecutorResult{};
      }};

  executor.execute(TestAction, liquid::String("Hello world"));

  EXPECT_TRUE(called);
}

TEST_F(ActionExecutorTest,
       ExecuteCreatesEntityFilesIfActionReturnsEntitiesToSaveAndModeIsEdit) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Name>(entity, {"My name"});

  auto entityPath = ScenePath / "entities" / "1.lqnode";
  EXPECT_FALSE(std::filesystem::exists(entityPath));

  liquid::editor::Action TestAction{
      "TestAction",
      [entity](liquid::editor::WorkspaceState &state, std::any data) {
        liquid::editor::ActionExecutorResult result{};
        result.entitiesToSave.push_back(entity);
        return result;
      }};

  executor.execute(TestAction);
  EXPECT_TRUE(std::filesystem::exists(entityPath));
}

TEST_F(
    ActionExecutorTest,
    ExecuteDoesNotCreateEntityFilesIfActionReturnsEntitiesToSaveAndModeIsSimulation) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Name>(entity, {"My name"});

  auto entityPath = ScenePath / "entities" / "1.lqnode";
  EXPECT_FALSE(std::filesystem::exists(entityPath));

  liquid::editor::Action TestAction{
      "TestAction",
      [entity](liquid::editor::WorkspaceState &state, std::any data) {
        liquid::editor::ActionExecutorResult result{};
        result.entitiesToSave.push_back(entity);
        return result;
      }};

  executor.execute(TestAction);
  EXPECT_FALSE(std::filesystem::exists(entityPath));
}

TEST_F(ActionExecutorTest,
       ExecuteDeletesEntityFilesIfActionReturnsEntitiesToDeleteAndModeIsEdit) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Name>(entity, {"My name"});
  state.scene.entityDatabase.set<liquid::Id>(entity, {15});

  executor.getSceneIO().saveEntity(entity, ScenePath / "main.lqscene");

  auto entityPath = ScenePath / "entities" / "15.lqnode";
  EXPECT_TRUE(std::filesystem::exists(entityPath));

  liquid::editor::Action TestAction{
      "TestAction",
      [entity](liquid::editor::WorkspaceState &state, std::any data) {
        liquid::editor::ActionExecutorResult result{};
        result.entitiesToDelete.push_back(entity);
        return result;
      }};

  executor.execute(TestAction);
  EXPECT_FALSE(std::filesystem::exists(entityPath));
}

TEST_F(
    ActionExecutorTest,
    ExecuteDoesNotDeleteEntityFilesIfActionReturnsEntitiesToDeleteAndModeIsSimulation) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Name>(entity, {"My name"});
  state.scene.entityDatabase.set<liquid::Id>(entity, {15});

  executor.getSceneIO().saveEntity(entity, ScenePath / "main.lqscene");

  auto entityPath = ScenePath / "entities" / "15.lqnode";
  EXPECT_TRUE(std::filesystem::exists(entityPath));

  liquid::editor::Action TestAction{
      "TestAction",
      [entity](liquid::editor::WorkspaceState &state, std::any data) {
        liquid::editor::ActionExecutorResult result{};
        result.entitiesToDelete.push_back(entity);
        return result;
      }};

  executor.execute(TestAction);
  EXPECT_TRUE(std::filesystem::exists(entityPath));
}
