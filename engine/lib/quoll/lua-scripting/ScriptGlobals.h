#pragma once

#include "ScriptLoop.h"

namespace quoll {

class WindowSignals;
class EntityDatabase;
class PhysicsSystem;
class AssetCache;

struct ScriptGlobals {
  WindowSignals &windowSignals;

  EntityDatabase &entityDatabase;

  PhysicsSystem &physicsSystem;

  AssetCache &assetCache;

  lua::ScriptLoop &scriptLoop;
};

} // namespace quoll
