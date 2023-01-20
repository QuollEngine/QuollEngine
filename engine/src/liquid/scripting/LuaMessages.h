#pragma once

namespace liquid {

/**
 * @brief Messages used in Lua scripts
 */
class LuaMessages {
public:
  /**
   * @brief Message for no entity table
   *
   * @param interfaceName Interface name
   * @param functionName Function name
   * @return Error message
   */
  static String noEntityTable(const String &interfaceName,
                              const String &functionName);

  /**
   * @brief Message for invalid arguments
   *
   * @tparam ...TArgs Template arguments
   * @param interfaceName Interface name
   * @param functionName Function name
   * @return Error message
   */
  template <class... TArgs>
  static inline String invalidArguments(const String &interfaceName,
                                        const String &functionName) {
    auto name = interfaceName + ":" + functionName;

    return "Invalid arguments provided for `" + name +
           "`. The function accepts the following arguments: " +
           getArguments<TArgs...>();
  }

private:
  template <class TFirst, class... TRest> static inline String getArguments() {
    std::stringstream ss;
    ss << "(" << getTypename<TFirst>();
    ((ss << ", " << getTypename<TRest>()), ...);
    ss << ")";
    return ss.str();
  }

  template <class T> static inline String getTypename() { return "[unknown]"; }
};

template <> inline String LuaMessages::getTypename<float>() { return "number"; }

template <> inline String LuaMessages::getTypename<String>() {
  return "string";
}

} // namespace liquid
