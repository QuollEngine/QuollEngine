#include "liquid/core/Base.h"
#include "AudioScriptingInterface.h"

#include "liquid/scripting/LuaScope.h"
#include "liquid/entity/EntityDatabase.h"

namespace liquid {

int AudioScriptingInterface::LuaInterface::play(void *state) {
  LuaScope scope(state);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  auto entityTable = scope.getGlobal<LuaTable>("entity");
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(2);

  if (!entityDatabase.hasComponent<AudioSourceComponent>(entity)) {
    return 0;
  }

  entityDatabase.setComponent<AudioStartComponent>(entity, {});
  return 0;
}

int AudioScriptingInterface::LuaInterface::isPlaying(void *state) {
  LuaScope scope(state);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  auto entityTable = scope.getGlobal<LuaTable>("entity");
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(2);

  bool isPlaying = entityDatabase.hasComponent<AudioStatusComponent>(entity);
  scope.set(isPlaying);
  return 1;
}

} // namespace liquid
