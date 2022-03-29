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

  /**
   * @brief Get function
   *
   * @param scope Lua scope
   * @param name Function name
   */
  void getFunction(void *scope, const char *name);

  /**
   * @brief Call function
   *
   * @param scope Lua scope
   * @param numArgs Number of arguments
   */
  void callFunction(void *scope, uint32_t numArgs);

  /**
   * @brief Check if scope has function
   *
   * @param scope Lua scope
   * @param name Function name
   * @retval true Scope has function
   * @retval false Scope does not have function
   */
  bool hasFunction(void *scope, const char *name);
};

} // namespace liquid
