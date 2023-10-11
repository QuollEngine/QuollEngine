#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Scripting interfaces for audio
 */
struct AudioScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua scripting interface for Audio
 */
class AudioScriptingInterface::LuaInterface {
public:
  /**
   * @brief Create audio table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  LuaInterface(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Play audio
   */
  void play();

  /**
   * @brief Check if audio is still playing
   *
   * @retval true Audio still playing
   * @retval false Audio is not playing
   */
  bool isPlaying();

  /**
   * @brief Delete component
   */
  void deleteThis();

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "audio"; }

  /**
   * @brief Create user type
   *
   * @param usertype User type
   */
  static void
  create(sol::usertype<AudioScriptingInterface::LuaInterface> usertype);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
