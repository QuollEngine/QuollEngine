#pragma once

#include "LuaScriptInputVariable.h"
#include "LuaHeaders.h"

namespace quoll {

/**
 * @brief Input variables table
 */
class InputVars {
public:
  /**
   * @brief Create input variables
   *
   * @param variables Input variables
   */
  InputVars(std::unordered_map<String, LuaScriptInputVariable> &variables);

  /**
   * @brief Register variable
   *
   * @param name Variable name
   * @param type Variable type
   * @return Variable value
   */
  std::variant<String, u32> registerVar(String name,
                                        LuaScriptVariableType type);

  /**
   * @brief Create interface
   *
   * @param state Sol state
   */
  static void create(sol::state_view state);

private:
  std::unordered_map<String, LuaScriptInputVariable> mVariables;
};

} // namespace quoll
