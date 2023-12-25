#pragma once

#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/editor/actions/Action.h"

#include "quoll/editor-tests/Testing.h"

class ActionTestBase
    : public ::testing::Test,
      public ::testing::WithParamInterface<quoll::editor::WorkspaceMode> {
public:
  void SetUp() override {
    if (::testing::UnitTest::GetInstance()
            ->current_test_info()
            ->value_param()) {
      auto mode = GetParam();
      state.mode = mode;
    }
  }

  quoll::Scene &activeScene() {
    return state.mode == quoll::editor::WorkspaceMode::Simulation
               ? state.simulationScene
               : state.scene;
  }

  quoll::AssetRegistry assetRegistry;
  quoll::editor::WorkspaceState state{};
};

#define InitActionsTestSuite(InstantiationName, TestSuiteName)                 \
  INSTANTIATE_TEST_SUITE_P(                                                    \
      InstantiationName, TestSuiteName,                                        \
      ::testing::Values(quoll::editor::WorkspaceMode::Edit,                    \
                        quoll::editor::WorkspaceMode::Simulation),             \
      [](const ::testing::TestParamInfo<ActionTestBase::ParamType> &info) {    \
        auto mode = info.param;                                                \
        return mode == quoll::editor::WorkspaceMode::Simulation ? "simulation" \
                                                                : "edit";      \
      });
