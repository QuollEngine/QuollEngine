#pragma once

#include "LuaHeaders.h"

namespace quoll {

/**
 * @brief Lua interpreter
 */
class LuaInterpreter {
public:
  /**
   * @brief Create state for Lua script
   *
   * @return Lua state
   */
  lua_State *createState();

  /**
   * @brief Destroy Lua state
   *
   * @param state Lua state
   */
  void destroyState(lua_State *state);

  /**
   * @brief Evaluate Lua script
   *
   * Loads the script and stores everything in Lua scope
   *
   * @param bytes Script data
   * @param state Lua state
   * @retval true Script evaluated sucessfully
   * @retval false Script failed to evaluate
   */
  bool evaluate(const std::vector<uint8_t> &bytes, lua_State *state);
};

} // namespace quoll
