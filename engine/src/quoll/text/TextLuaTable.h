#pragma once

#include "quoll/scripting/ComponentLuaInterface.h"

namespace quoll {

/**
 * @brief Lua interface for text component
 */
class TextLuaTable {
public:
  /**
   * @brief Create text table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  TextLuaTable(Entity entity, ScriptGlobals scriptGlobals);

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
  sol_maybe<f32> getLineHeight();

  /**
   * @brief Set line height
   *
   * @param lineHeight Line height
   */
  void setLineHeight(f32 lineHeight);

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
  static void create(sol::usertype<TextLuaTable> usertype);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
