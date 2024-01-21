#pragma once

#include "ScriptLoop.h"

namespace quoll {

class WindowSignals;
class EntityDatabase;
class PhysicsSystem;
class AssetRegistry;

struct ScriptGlobals {
  WindowSignals &windowSignals;

  EntityDatabase &entityDatabase;

  PhysicsSystem &physicsSystem;

  AssetRegistry &assetRegistry;

  lua::ScriptLoop &scriptLoop;
};

} // namespace quoll
