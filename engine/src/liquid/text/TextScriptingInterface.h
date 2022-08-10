#pragma once

#include "liquid/scripting/ComponentLuaInterface.h"

namespace liquid {

/**
 * @brief Scripting interfaces for text component
 */
struct TextScriptingInterface {
  class LuaInterface;
};

/**
 * @brief Lua interface for text component
 */
class TextScriptingInterface::LuaInterface
    : public ComponentLuaInterface<TextScriptingInterface::LuaInterface> {
public:
  /**
   * @brief Get text contents
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int getText(void *state);

  /**
   * @brief Set text contents
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int setText(void *state);

  /**
   * @brief Interface fields
   */
  static constexpr std::array<InterfaceField, 2> Fields{
      InterfaceField{"get_text", getText}, InterfaceField{"set_text", setText}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "text"; }
};

} // namespace liquid
