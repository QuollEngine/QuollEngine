#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/events/EventSystem.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/physics/PhysicsSystem.h"

#include "LuaInterpreter.h"

namespace quoll {

/**
 * @brief Scripting system
 */
class ScriptingSystem {
public:
  /**
   * @brief Create scripting system
   *
   * @param eventSystem Event system
   * @param assetRegistry Asset registry
   */
  ScriptingSystem(EventSystem &eventSystem, AssetRegistry &assetRegistry);

  ~ScriptingSystem() = default;

  ScriptingSystem(const ScriptingSystem &) = delete;
  ScriptingSystem &operator=(const ScriptingSystem &) = delete;
  ScriptingSystem(ScriptingSystem &&) = delete;
  ScriptingSystem &operator=(ScriptingSystem &&) = delete;

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
  void update(float dt, EntityDatabase &entityDatabase);

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
  void destroyScriptingData(Script &component);

  /**
   * @brief Create scripting data
   *
   * @param component Scripting component
   * @param entity Entity
   */
  void createScriptingData(Script &component, Entity event);

private:
  EventSystem &mEventSystem;
  AssetRegistry &mAssetRegistry;
  LuaInterpreter mLuaInterpreter;

  EntityDatabaseObserver<Script> mScriptRemoveObserver;
};

} // namespace quoll
