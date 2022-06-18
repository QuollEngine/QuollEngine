#pragma once

#include "LuaScope.h"

namespace liquid {

/**
 * @brief Lua interpreter
 */
class LuaInterpreter {
public:
  /**
   * @brief Create scope for Lua script
   *
   * @return Lua scope
   */
  LuaScope createScope();

  /**
   * @brief Destroy Lua scope
   *
   * @param scope Lua scope
   */
  void destroyScope(LuaScope &scope);

  /**
   * @brief Evaluate Lua script
   *
   * Loads the script and stores everything in Lua scope
   *
   * @param bytes Script data
   * @param scope Lua scope
   */
  void evaluate(const std::vector<char> &bytes, LuaScope &scope);
};

} // namespace liquid
