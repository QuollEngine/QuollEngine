#pragma once

namespace liquid {

class LuaInterpreter {
public:
  /**
   * @brief Create scope for Lua script
   *
   * @return Lua scope
   */
  void *createScope();

  /**
   * @brief Destroy Lua scope
   *
   * @param scope Lua scope
   */
  void destroyScope(void *scope);

  /**
   * @brief Evaluate Lua script
   *
   * Loads the script and stores everything in Lua scope
   *
   * @param bytes Script data
   * @param scope Scope
   */
  void evaluate(const std::vector<char> &bytes, void *scope);
};

} // namespace liquid
