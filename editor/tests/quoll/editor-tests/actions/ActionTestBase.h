#pragma once

#include "quoll/editor/actions/Action.h"
#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/editor-tests/Testing.h"

class ActionTestBase : public ::testing::Test {
public:
  quoll::AssetRegistry assetRegistry;
  quoll::editor::WorkspaceState state{};
};
