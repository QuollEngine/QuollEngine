#include "quoll/core/Base.h"
#include "Messages.h"

namespace quoll::lua {

String Messages::noEntityTable(const String &interfaceName,
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

String Messages::entityDoesNotExist(const String &interfaceName,
                                    const String &functionName, Entity entity) {
  return "Entity " + std::to_string(static_cast<u32>(entity)) +
         " does not exist";
}

String Messages::componentDoesNotExist(const String &componentName,
                                       Entity entity) {
  return "Entity " + std::to_string(static_cast<u32>(entity)) +
         " does not have " + componentName + " component";
}

String Messages::nothingSpawnedBecauseEmptyPrefab(const String &interfaceName,
                                                  const String &functionName,
                                                  const String &prefabName) {
  return "Nothing is spawned because prefab " + prefabName + " is empty";
}

String Messages::assetNotFound(const String &interfaceName,
                               const String &functionName, const String &type) {
  return "Asset of type " + type + " is not found";
}

} // namespace quoll::lua
