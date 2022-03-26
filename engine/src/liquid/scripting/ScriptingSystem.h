#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/events/EventSystem.h"

#include "LuaInterpreter.h"

namespace liquid {

enum class ScriptType { Lua };

struct Script {
  ScriptType type = ScriptType::Lua;
  String name;
  std::vector<char> bytes;
};

class ScriptingSystem {
public:
  /**
   * @brief Create scripting system
   *
   * @param entityContext Entity context
   * @param eventSystem Event system
   */
  ScriptingSystem(EntityContext &entityContext, EventSystem &eventSystem);

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
   * @brief Add script from file
   *
   * Sets script filename as script name
   * internally
   *
   * @param fileName File name
   * @return New script handle
   */
  ScriptHandle addScript(const String &fileName);

  /**
   * @brief Add script from buffer
   *
   * @param name Script name
   * @param bytes Buffer data
   * @return New script handle
   */
  ScriptHandle addScript(const String &name, const std::vector<char> &bytes);

  /**
   * @brief Get all scripts
   *
   * @return List of scripts
   */
  inline const std::unordered_map<ScriptHandle, Script> &getScripts() const {
    return mScripts;
  }

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
  ScriptHandle mLastHandle{0};
  std::unordered_map<ScriptHandle, Script> mScripts;
  EntityContext &mEntityContext;
  EventSystem &mEventSystem;
  LuaInterpreter mLuaInterpreter;
};

} // namespace liquid
