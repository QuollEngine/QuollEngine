#pragma once

namespace liquid {

/**
 * @brief Lua table
 */
class LuaTable {
public:
  /**
   * @brief Create Lua table
   *
   * @param scope Scope
   * @param size Table size
   */
  LuaTable(void *scope, uint32_t size);

  /**
   * @brief Set number table value
   *
   * @param key Key column
   * @param value Value column
   */
  void set(const char *key, uint32_t value);

private:
  uint32_t mSize = 0;
  void *mScope = nullptr;
};

} // namespace liquid
