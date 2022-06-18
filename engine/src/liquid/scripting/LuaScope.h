#pragma once

struct lua_State;

#include "LuaTable.h"

namespace liquid {

/**
 * @brief Lua user data object
 */
struct LuaUserData {
  /**
   * @brief User data pointer
   */
  void *pointer = nullptr;
};

/**
 * @brief Wrapper around Lua scope
 *
 * Encapsulates Lua functionality
 * with a simple API
 */
class LuaScope {
public:
  /**
   * @brief Default constructor
   *
   * Does not set the Lua scope
   */
  LuaScope() = default;

  /**
   * @brief Create lua scope wrapper
   *
   * @param scope Lua scope
   */
  LuaScope(void *scope);

  /**
   * @brief Get varible value
   *
   * Assertion fails if a type is not
   * implemented
   *
   * @tparam T Variable type
   * @return Variable value
   */
  template <class T> T get() { LIQUID_ASSERT(false, "Not implemented"); }

  /**
   * @brief Generic variable setter
   *
   * Assertion fails if a type is not
   * implemented
   *
   * @tparam T Variable type
   * @param value Variable value
   */
  template <class T> void set(const T &value) {
    LIQUID_ASSERT(false, "Not implemented");
  }

  /**
   * @brief Get global variable
   *
   * @tparam T Variable type
   * @param name Variable global name
   * @return Variable value
   */
  template <class T> T getGlobal(const String &name) {
    luaGetGlobal(name);
    return get<T>();
  }

  /**
   * @brief Set global variable
   *
   * @tparam T Variable type
   * @param name Variable global name
   * @param value Variable value
   */
  template <class T> void setGlobal(const String &name, const T &value) {
    set(value);
    setPreviousValueAsGlobal(name);
  }

  /**
   * @brief Set previously stored value as global
   *
   * @param name Global variable name
   */
  void setPreviousValueAsGlobal(const String &name);

  /**
   * @brief Check if global function exists i
   *
   * @param name Function name
   * @retval true Function exists
   * @retval false Function does not exist
   */
  bool hasFunction(const String &name);

  /**
   * @brief Call global function
   *
   * @param numArgs Number of arguments
   */
  void call(uint32_t numArgs);

  /**
   * @brief Create table
   *
   * @param size Table size
   * @return Lua table
   */
  LuaTable createTable(uint32_t size);

  /**
   * @brief Get Lua state
   *
   * @return Lua state
   */
  inline lua_State *getLuaState() { return mScope; }

  /**
   * @brief Get global variable and add it to stack
   *
   * @param name Variable name
   * @return Lua result
   */
  int luaGetGlobal(const String &name);

private:
  /**
   * @brief Convert stack value to integer
   *
   * @param index Stack index
   * @return Integer value
   */
  int32_t luaGetInteger(int index);

  /**
   * @brief Convert stack value to user data
   *
   * @param index Stack index
   * @return User data value
   */
  void *luaGetUserData(int index);

  /**
   * @brief Push user data to the stack
   *
   * @param data User data
   */
  void luaSetUserData(void *data);

  /**
   * @brief Pop item from the stack
   *
   * @param Item index
   */
  void luaPop(int index);

private:
  lua_State *mScope = nullptr;
};

/**
 * @brief Get integer
 *
 * @return Integer value
 */
template <> inline int32_t LuaScope::get<int32_t>() {
  return luaGetInteger(-1);
}

/**
 * @brief Get unsigned integer

 * @return Unsigned integer
 */
template <> inline uint32_t LuaScope::get<uint32_t>() {
  return static_cast<uint32_t>(luaGetInteger(-1));
}

/**
 * @brief Get table
 *
 * @return Lua table
 */
template <> inline LuaTable LuaScope::get<LuaTable>() {
  return LuaTable(static_cast<void *>(mScope));
}

/**
 * @brief Get user data
 *
 * @return User data
 */
template <> inline LuaUserData LuaScope::get<LuaUserData>() {
  return {luaGetUserData(-1)};
}

/**
 * @brief Set user data
 *
 * @param data User data
 */
template <> inline void LuaScope::set<LuaUserData>(const LuaUserData &data) {
  luaSetUserData(data.pointer);
}

} // namespace liquid
