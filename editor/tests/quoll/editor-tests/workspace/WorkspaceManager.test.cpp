#include "quoll/core/Base.h"
#include "quoll/editor/workspace/WorkspaceManager.h"
#include "quoll/editor-tests/Testing.h"

class WorkspaceManagerTest : public ::testing::Test {
public:
  quoll::editor::WorkspaceManager workspaceManager;
};

class TestWorkspace : public quoll::editor::Workspace {
public:
  TestWorkspace(u32 asset) : mAsset(asset) {}

  void prepare() override {}

  void update(f32 dt) override {}

  void fixedUpdate(f32 dt) override {}

  void render() override {}

  void processShortcuts(int key, int mods) override {}

  void updateFrameData(quoll::rhi::RenderCommandList &commandList,
                       u32 frameIndex) override {}

  quoll::editor::WorkspaceMatchParams getMatchParams() const override {
    return {.type = "TestWorkspace",
            .asset = mAsset,
            .assetType = quoll::AssetType::None};
  }

  /**
   * Reload workspace
   */
  void reload() override {}

private:
  u32 mAsset = 0;
};

TEST_F(WorkspaceManagerTest,
       AddingWorkspaceAddsItToTheEndIfNoExistingWorkspaceWithMatchingParams) {

  workspaceManager.add(new TestWorkspace(10));
  workspaceManager.add(new TestWorkspace(50));

  EXPECT_EQ(workspaceManager.getWorkspaces().size(), 2);

  EXPECT_EQ(workspaceManager.getWorkspaces().at(0)->getMatchParams().asset, 10);
  EXPECT_EQ(workspaceManager.getWorkspaces().at(1)->getMatchParams().asset, 50);
  EXPECT_EQ(workspaceManager.getCurrentWorkspace()->getMatchParams().asset, 50);
}

TEST_F(WorkspaceManagerTest,
       AddingWorkspaceWithMatchingParamsUpdatesExistingWorkspace) {

  workspaceManager.add(new TestWorkspace(10));
  workspaceManager.add(new TestWorkspace(10));

  EXPECT_EQ(workspaceManager.getWorkspaces().size(), 1);
  EXPECT_EQ(workspaceManager.getWorkspaces().at(0)->getMatchParams().asset, 10);
  EXPECT_EQ(workspaceManager.getCurrentWorkspace()->getMatchParams().asset, 10);
}

TEST_F(WorkspaceManagerTest,
       SwitchingWorkspaceSwitchesCurrentWorkspaceToProvidedOne) {
  workspaceManager.add(new TestWorkspace(10));
  workspaceManager.add(new TestWorkspace(50));

  EXPECT_EQ(workspaceManager.getWorkspaces().size(), 2);
  EXPECT_EQ(workspaceManager.getCurrentWorkspace()->getMatchParams().asset, 50);

  workspaceManager.switchWorkspace(0);

  EXPECT_EQ(workspaceManager.getWorkspaces().size(), 2);
  EXPECT_EQ(workspaceManager.getCurrentWorkspace()->getMatchParams().asset, 10);
}
