#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/events/EventSystem.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/physics/PhysicsSystem.h"

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
   * @param eventSystem Event system
   * @param assetRegistry Asset registry
   */
  LuaScriptingSystem(EventSystem &eventSystem, AssetRegistry &assetRegistry);

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
   */
  void start(EntityDatabase &entityDatabase, PhysicsSystem &physicsSystem);

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
  EventSystem &mEventSystem;
  AssetRegistry &mAssetRegistry;
  lua::Interpreter mLuaInterpreter;
  lua::ScriptLoop mScriptLoop;

  EntityDatabaseObserver<LuaScript> mScriptRemoveObserver;
};

} // namespace quoll
