#pragma once

#include "liquid/entity/Entity.h"

namespace quoll {

/**
 * @brief Lua table
 */
class LuaTable {
public:
  /**
   * @brief Create lua table wrapper
   *
   * @param scope Lua table wrapper
   * @param index Stack index
   */
  LuaTable(void *scope, int32_t index);

  /**
   * @brief Set number table value
   *
   * @param key Key column
   * @param value Value column
   */
  void set(const char *key, uint32_t value);

  /**
   * @brief Set entity table value
   *
   * @param key Key column
   * @param value Value column
   */
  void set(const char *key, Entity value);

  /**
   * @brief Set function table value
   *
   * @param key Key column
   * @param value Value column
   */
  void set(const char *key, int (*value)(void *));

  /**
   * @brief Set table value
   *
   * @param key Key column
   * @param value Value column
   */
  void set(const char *key, const LuaTable &value);

  /**
   * @brief Get field
   *
   * @param key Field key
   */
  void get(const char *key);

private:
  uint32_t mSize = 0;
  int32_t mIndex = 0;
  void *mScope = nullptr;
};

} // namespace quoll
