#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/window/WindowSignals.h"
#include "ScriptLoop.h"

namespace quoll {

/**
 * @brief Global variables that are needed for scripts
 */
struct ScriptGlobals {
  /**
   * Window signals
   */
  WindowSignals &windowSignals;

  /**
   * Entity database
   */
  EntityDatabase &entityDatabase;

  /**
   * Physics system
   */
  PhysicsSystem &physicsSystem;

  /**
   * Asset registry
   */
  AssetRegistry &assetRegistry;

  /**
   * Script loop
   */
  lua::ScriptLoop &scriptLoop;
};

} // namespace quoll
