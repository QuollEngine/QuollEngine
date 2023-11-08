#pragma once

#include "quoll/entity/Entity.h"

namespace quoll::lua {

/**
 * @brief Messages used in Lua scripts
 */
class Messages {
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
   * @brief Message for entity does not exist
   *
   * @param interfaceName Interface name
   * @param functionName Function name
   * @param entity Entity
   * @return Error message
   */
  static String entityDoesNotExist(const String &interfaceName,
                                   const String &functionName, Entity entity);

  /**
   * @brief Message for component does not exist
   *
   * @param componentName Component name
   * @param entity Entity
   * @return Error message
   */
  static String componentDoesNotExist(const String &componentName,
                                      Entity entity);

  /**
   * @brief Messages for nothing is spawned because prefab is empty
   *
   * @param interfaceName Interface name
   * @param functionName Function name
   * @param prefabName Prefab name
   * @return Error message
   */
  static String nothingSpawnedBecauseEmptyPrefab(const String &interfaceName,
                                                 const String &functionName,
                                                 const String &prefabName);

  /**
   * @brief Message for asset not found
   *
   * @param interfaceName Interface name
   * @param functionName Function name
   * @param type Asset type
   * @return Error message
   */
  static String assetNotFound(const String &interfaceName,
                              const String &functionName, const String &type);

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

template <> inline String Messages::getTypename<f32>() { return "number"; }

template <> inline String Messages::getTypename<String>() { return "string"; }

template <> inline String Messages::getTypename<Entity>() {
  return "entity table";
}

} // namespace quoll::lua
