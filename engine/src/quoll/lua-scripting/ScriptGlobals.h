#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/window/WindowSignals.h"
#include "ScriptLoop.h"

namespace quoll {

struct ScriptGlobals {
  WindowSignals &windowSignals;

  EntityDatabase &entityDatabase;

  PhysicsSystem &physicsSystem;

  AssetRegistry &assetRegistry;

  lua::ScriptLoop &scriptLoop;
};

} // namespace quoll
