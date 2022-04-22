#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/events/EventSystem.h"
#include "liquid/asset/AssetManager.h"

#include "LuaInterpreter.h"

namespace liquid {

class ScriptingSystem {
public:
  /**
   * @brief Create scripting system
   *
   * @param entityContext Entity context
   * @param eventSystem Event system
   * @param assetManager Asset manager
   */
  ScriptingSystem(EntityContext &entityContext, EventSystem &eventSystem,
                  AssetManager &assetManager);

  /**
   * @brief Destroy scripting system
   *
   * Destroy all the scripts and their handles
   */
  ~ScriptingSystem();

  ScriptingSystem(const ScriptingSystem &) = delete;
  ScriptingSystem &operator=(const ScriptingSystem &) = delete;
  ScriptingSystem(ScriptingSystem &&) = delete;
  ScriptingSystem &operator=(ScriptingSystem &&) = delete;

  /**
   * @brief Call script start on new scripts
   */
  void start();

  /**
   * @brief Call script update on new scripts
   */
  void update();

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
  EntityContext &mEntityContext;
  EventSystem &mEventSystem;
  AssetManager &mAssetManager;
  LuaInterpreter mLuaInterpreter;
};

} // namespace liquid
