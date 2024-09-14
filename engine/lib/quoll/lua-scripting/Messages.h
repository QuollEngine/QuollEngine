#pragma once

#include "quoll/entity/Entity.h"

namespace quoll::lua {

class Messages {
public:
  static String noEntityTable(const String &interfaceName,
                              const String &functionName);

  static String entityDoesNotExist(const String &interfaceName,
                                   const String &functionName, Entity entity);

  static String componentDoesNotExist(const String &componentName,
                                      Entity entity);

  static String nothingSpawnedBecauseEmptyPrefab(const String &interfaceName,
                                                 const String &functionName,
                                                 const String &prefabName);

  static String assetNotFound(const String &interfaceName,
                              const String &functionName, const String &type);

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
