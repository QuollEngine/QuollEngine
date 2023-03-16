#pragma once

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/actions/Action.h"

#include "liquidator-tests/Testing.h"

class ActionTestBase
    : public ::testing::Test,
      public ::testing::WithParamInterface<liquid::editor::WorkspaceMode> {
public:
  void SetUp() override {
    if (::testing::UnitTest::GetInstance()
            ->current_test_info()
            ->value_param()) {
      auto mode = GetParam();
      state.mode = mode;
    }
  }

  liquid::Scene &activeScene() {
    return state.mode == liquid::editor::WorkspaceMode::Simulation
               ? state.simulationScene
               : state.scene;
  }

  liquid::AssetRegistry registry;
  liquid::editor::WorkspaceState state{{}, registry};
};

#define InitActionsTestSuite(InstantiationName, TestSuiteName)                 \
  INSTANTIATE_TEST_SUITE_P(                                                    \
      InstantiationName, TestSuiteName,                                        \
      ::testing::Values(liquid::editor::WorkspaceMode::Edit,                   \
                        liquid::editor::WorkspaceMode::Simulation),            \
      [](const ::testing::TestParamInfo<ActionTestBase::ParamType> &info) {    \
        auto mode = info.param;                                                \
        return mode == liquid::editor::WorkspaceMode::Simulation               \
                   ? "simulation"                                              \
                   : "edit";                                                   \
      });
