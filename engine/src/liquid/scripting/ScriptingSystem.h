#pragma once

#include "liquid/entity/EntityDatabase.h"
#include "liquid/events/EventSystem.h"
#include "liquid/asset/AssetRegistry.h"

#include "LuaInterpreter.h"

namespace liquid {

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
   */
  void start(EntityDatabase &entityDatabase);

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
};

} // namespace liquid
