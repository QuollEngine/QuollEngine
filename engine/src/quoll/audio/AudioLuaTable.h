#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

/**
 * @brief Lua scripting interface for audio
 */
class AudioLuaTable {
public:
  /**
   * @brief Create audio table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  AudioLuaTable(Entity entity, ScriptGlobals scriptGlobals);

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
  static void create(sol::usertype<AudioLuaTable> usertype);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
