#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/window/Window.h"

#include "LuaScript.h"
#include "ScriptLoop.h"
#include "Interpreter.h"

namespace quoll {

/**
 * @brief Scripting system
 */
class LuaScriptingSystem {
public:
  /**
   * @brief Create scripting system
   *
   * @param assetRegistry Asset registry
   */
  LuaScriptingSystem(AssetRegistry &assetRegistry);

  ~LuaScriptingSystem() = default;

  LuaScriptingSystem(const LuaScriptingSystem &) = delete;
  LuaScriptingSystem &operator=(const LuaScriptingSystem &) = delete;
  LuaScriptingSystem(LuaScriptingSystem &&) = delete;
  LuaScriptingSystem &operator=(LuaScriptingSystem &&) = delete;

  /**
   * @brief Call script start on new scripts
   *
   * @param entityDatabase Entity database
   * @param physicsSystem Physics system
   * @param windowSignals Window signals
   */
  void start(EntityDatabase &entityDatabase, PhysicsSystem &physicsSystem,
             WindowSignals &windowSignals);

  /**
   * @brief Call script update on new scripts
   *
   * @param dt Delta time
   * @param entityDatabase Entity database
   */
  void update(f32 dt, EntityDatabase &entityDatabase);

  /**
   * @brief Cleanup components
   *
   * Destroys all scripting data and
   * removes event observers
   *
   * @param entityDatabase Entity database
   */
  void cleanup(EntityDatabase &entityDatabase);

  /**
   * @brief Observer changes in entities
   *
   * @param entityDatabase Entity database
   */
  void observeChanges(EntityDatabase &entityDatabase);

private:
  /**
   * @brief Destroy scripting data
   *
   * @param component Scripting component
   */
  void destroyScriptingData(LuaScript &component);

  /**
   * @brief Create scripting data
   *
   * @param component Scripting component
   * @param entity Entity
   */
  void createScriptingData(LuaScript &component, Entity event);

private:
  AssetRegistry &mAssetRegistry;
  lua::Interpreter mLuaInterpreter;
  lua::ScriptLoop mScriptLoop;

  EntityDatabaseObserver<LuaScript> mScriptRemoveObserver;
};

} // namespace quoll
