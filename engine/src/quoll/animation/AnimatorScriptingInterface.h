#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Scripting interfaces for animator
 */
struct AnimatorScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua scripting interface for animator
 */
class AnimatorScriptingInterface::LuaInterface
    : public ComponentLuaInterface<AnimatorScriptingInterface::LuaInterface> {
public:
  /**
   * @brief Play audio
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int trigger(void *state);

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
  static constexpr std::array<InterfaceField, 2> Fields{
      InterfaceField{"trigger", trigger}, InterfaceField{"delete", deleteThis}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "animator"; }
};

} // namespace quoll
