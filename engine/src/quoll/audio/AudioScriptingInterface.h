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
class AudioScriptingInterface::LuaInterface
    : public ComponentLuaInterface<AudioScriptingInterface::LuaInterface> {
public:
  /**
   * @brief Play audio
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int play(void *state);

  /**
   * @brief Check if audio is still playing
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int isPlaying(void *state);

  /**
   * @brief Delete component
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int deleteThis(void *state);

  /**
   * @brief Interface fields
   */
  static constexpr std::array<InterfaceField, 3> Fields{
      InterfaceField{"play", play}, InterfaceField{"is_playing", isPlaying},
      InterfaceField{"delete", deleteThis}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "audio"; }
};

} // namespace quoll
