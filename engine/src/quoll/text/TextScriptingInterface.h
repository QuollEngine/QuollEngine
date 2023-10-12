#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Scripting interfaces for text component
 */
struct TextScriptingInterface {
  class LuaInterface;
};

/**
 * @brief Lua interface for text component
 */
class TextScriptingInterface::LuaInterface {
public:
  /**
   * @brief Create text table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  LuaInterface(Entity entity, ScriptGlobals scriptGlobals);

  /**
   * @brief Get text contents
   *
   * @return Text contents
   */
  sol_maybe<String> getText();

  /**
   * @brief Set text contents
   *
   * @param text Text contents
   */
  void setText(String text);

  /**
   * @brief Get line height
   *
   * @return Line height
   */
  sol_maybe<float> getLineHeight();

  /**
   * @brief Set line height
   *
   * @param lineHeight Line height
   */
  void setLineHeight(float lineHeight);

  /**
   * @brief Delete component
   */
  void deleteThis();

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "text"; }

  /**
   * @brief Create user type
   *
   * @param usertype User type
   */
  static void
  create(sol::usertype<TextScriptingInterface::LuaInterface> usertype);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
