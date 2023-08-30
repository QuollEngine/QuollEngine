#include "quoll/core/Base.h"
#include "LuaMessages.h"

namespace quoll {

String LuaMessages::noEntityTable(const String &interfaceName,
                                  const String &functionName) {
  String correctName = interfaceName + ":" + functionName;
  String invalidName = interfaceName + "." + functionName;

  return "Entity does not exist. Make sure you are using "
         "` " +
         correctName +
         "` instead "
         "of `" +
         invalidName + "`";
}

String LuaMessages::entityDoesNotExist(const String &interfaceName,
                                       const String &functionName,
                                       Entity entity) {
  return "Entity " + std::to_string(static_cast<uint32_t>(entity)) +
         " does not exist";
}

String LuaMessages::componentDoesNotExist(const String &componentName,
                                          Entity entity) {
  return "Entity " + std::to_string(static_cast<uint32_t>(entity)) +
         " does not have " + componentName + " component";
}

String
LuaMessages::nothingSpawnedBecauseEmptyPrefab(const String &interfaceName,
                                              const String &functionName,
                                              const String &prefabName) {
  return "Nothing is spawned because prefab " + prefabName + " is empty";
}

String LuaMessages::assetNotFound(const String &interfaceName,
                                  const String &functionName,
                                  const String &type) {
  return "Asset of type " + type + " is not found";
}

} // namespace quoll
