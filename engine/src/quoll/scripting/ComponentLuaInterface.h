#pragma once

namespace quoll {

template <class TComponentInterface> struct ComponentLuaInterface {
public:
  /**
   * @brief Interface field used in table
   */
  struct InterfaceField {
    /**
     * Field key
     */
    const char *key;

    /**
     * Field function
     */
    int (*fn)(void *);
  };
};

} // namespace quoll