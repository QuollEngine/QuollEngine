#pragma once

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/actions/Action.h"

#include "liquidator-tests/Testing.h"

class ActionTestBase : public ::testing::Test {
public:
  liquid::AssetRegistry registry;
  liquid::editor::WorkspaceState state{{}, registry};
};
