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
   * @brief Get variable value from stack
   *
   * Assertion fails if a type is not
   * implemented
   *
   * @tparam T Variable type
   * @param index Stack index
   * @return Variable value
   */
  template <class T> T get(int32_t index = -1) {
    LIQUID_ASSERT(false, "Not implemented");
  }

  /**
   * @brief Check if variable is not a correct type
   *
   * Assertion fails if a type is not
   * implemented
   *
   * @tparam T Variable type to check for
   * @param index Stack index
   * @retval true Type matches
   * @retval false Type does not match
   */
  template <class T> bool is(int32_t index = -1) {
    LIQUID_ASSERT(false, "Not implemented");
    return false;
  }

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
   * @brief Pops last value from stack
   *
   * @tparam T Variable type
   * @return Variable value
   */
  template <class T> T popLast() {
    const auto &value = get<T>(-1);
    pop(-1);
    return value;
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
    return popLast<T>();
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

  /**
   * @brief Pop items from the stack
   *
   * @param count Number of elements to pop
   */
  void pop(int count);

private:
  /**
   * @brief Convert stack value to boolean
   *
   * @param index Stack index
   * @return Boolean value
   */
  bool luaGetBoolean(int index);

  /**
   * @brief Convert stack value to integer
   *
   * @param index Stack index
   * @return Integer value
   */
  int32_t luaGetInteger(int index);

  /**
   * @brief Convert stack value to decimal
   *
   * @param index Stack index
   * @return Decimal
   */
  float luaGetNumber(int index);

  /**
   * @brief Convert stack value to string
   *
   * @param index Stack index
   * @return String value
   */
  StringView luaGetString(int index);

  /**
   * @brief Convert stack value to user data
   *
   * @param index Stack index
   * @return User data value
   */
  void *luaGetUserData(int index);

  /**
   * @brief Check if stack value is a number
   *
   * @param index Stack index
   * @retval true Value is number
   * @retval false Value is not a number
   */
  bool luaIsNumber(int index);

  /**
   * @brief Check if stack value is a string
   *
   * @param index Stack index
   * @retval true Value is string
   * @retval false Value is not a string
   */
  bool luaIsString(int index);

  /**
   * @brief Push boolean to stack
   *
   * @param value Boolean value
   */
  void luaSetBoolean(bool value);

  /**
   * @brief Push floating point to stack
   *
   * @param value Floating point value
   */
  void luaSetNumber(float value);

  /**
   * @brief Push string to stack
   *
   * @param value String value
   */
  void luaSetString(const String &value);

  /**
   * @brief Push user data to the stack
   *
   * @param value User data value
   */
  void luaSetUserData(void *value);

private:
  lua_State *mScope = nullptr;
};

/**
 * @brief Get integer
 *
 * @param index Stack index
 * @return Integer value
 */
template <> inline bool LuaScope::get<bool>(int32_t index) {
  return luaGetBoolean(index);
}

/**
 * @brief Get integer
 *
 * @param index Stack index
 * @return Integer value
 */
template <> inline int32_t LuaScope::get<int32_t>(int32_t index) {
  return luaGetInteger(index);
}

/**
 * @brief Get unsigned integer
 *
 * @param index Stack index
 * @return Unsigned integer
 */
template <> inline uint32_t LuaScope::get<uint32_t>(int32_t index) {
  return static_cast<uint32_t>(luaGetInteger(index));
}

/**
 * @brief Get floating point
 *
 * @param index Stack index
 * @return Floating point value
 */
template <> inline float LuaScope::get<float>(int32_t index) {
  return luaGetNumber(index);
}

/**
 * @brief Get string view
 *
 * @param index Stack index
 * @return String view
 */
template <> inline StringView LuaScope::get<StringView>(int32_t index) {
  return luaGetString(index);
}

/**
 * @brief Get string
 *
 * @param index Stack index
 * @return String
 */
template <> inline String LuaScope::get<String>(int32_t index) {
  return String(luaGetString(index));
}

/**
 * @brief Get table
 *
 * @param index Stack index
 * @return Lua table
 */
template <> inline LuaTable LuaScope::get<LuaTable>(int32_t index) {
  return LuaTable(static_cast<void *>(mScope));
}

/**
 * @brief Get user data
 *
 * @param index Stack index
 * @return User data
 */
template <> inline LuaUserData LuaScope::get<LuaUserData>(int32_t index) {
  return {luaGetUserData(index)};
}

/**
 * @brief Check if variable is a string
 *
 * @param index Stack index
 * @retval true Type matches
 * @retval false Type does not match
 */
template <> inline bool LuaScope::is<String>(int32_t index) {
  return luaIsString(index);
}

/**
 * @brief Check if variable is a floating point
 *
 * @param index Stack index
 * @retval true Type matches
 * @retval false Type does not match
 */
template <> inline bool LuaScope::is<float>(int32_t index) {
  return luaIsNumber(index);
}

/**
 * @brief Set boolean
 *
 * @param value Boolean value
 */
template <> inline void LuaScope::set<bool>(const bool &value) {
  luaSetBoolean(value);
}

/**
 * @brief Set floating point
 *
 * @param value Floating point value
 */
template <> inline void LuaScope::set<float>(const float &value) {
  luaSetNumber(value);
}

/**
 * @brief Set string
 *
 * @param value String value
 */
template <> inline void LuaScope::set<String>(const String &value) {
  luaSetString(value);
}

/**
 * @brief Set user data
 *
 * @param value User data value
 */
template <> inline void LuaScope::set<LuaUserData>(const LuaUserData &value) {
  luaSetUserData(value.pointer);
}

/**
 * @brief Get global table variable
 *
 * @param name Variable global name
 * @return Table value
 */
template <> inline LuaTable LuaScope::getGlobal<LuaTable>(const String &name) {
  luaGetGlobal(name);
  return get<LuaTable>();
}

} // namespace liquid
