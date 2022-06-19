#pragma once

namespace liquid {

template <class TComponentInterface> struct ComponentLuaInterface {
public:
  /**
   * @brief Interface field used in table
   */
  struct InterfaceField {
    /**
     * @brief Field key
     */
    const char *key;

    /**
     * @brief Field function
     */
    int (*fn)(void *);
  };
};

} // namespace liquid
