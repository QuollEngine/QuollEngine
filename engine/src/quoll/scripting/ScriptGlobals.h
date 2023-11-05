#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/asset/AssetRegistry.h"
#include "ScriptLoop.h"

namespace quoll {

/**
 * @brief Global variables that are needed for scripts
 */
struct ScriptGlobals {
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
  ScriptLoop &scriptLoop;
};

} // namespace quoll
