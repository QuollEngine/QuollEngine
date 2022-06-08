#pragma once

#include "liquid/entity/EntityContext.h"
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
   * @param entityContext Entity context
   */
  void start(EntityContext &entityContext);

  /**
   * @brief Call script update on new scripts
   *
   * @param entityContext Entity context
   */
  void update(EntityContext &entityContext);

  /**
   * @brief Cleanup components
   *
   * Destroys all scripting data and
   * removes event observers
   *
   * @param entityContext Entity context
   */
  void cleanup(EntityContext &entityContext);

private:
  /**
   * @brief Destroy scripting data
   *
   * @param component Scripting component
   */
  void destroyScriptingData(ScriptingComponent &component);

  /**
   * @brief Create scripting data
   *
   * @param component Scripting component
   * @param entity Entity
   */
  void createScriptingData(ScriptingComponent &component, Entity event);

private:
  EventSystem &mEventSystem;
  AssetRegistry &mAssetRegistry;
  LuaInterpreter mLuaInterpreter;
};

} // namespace liquid
