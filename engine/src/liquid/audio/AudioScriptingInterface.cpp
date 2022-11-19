#include "liquid/core/Base.h"
#include "AudioScriptingInterface.h"

#include "liquid/scripting/LuaScope.h"
#include "liquid/entity/EntityDatabase.h"

namespace liquid {

int AudioScriptingInterface::LuaInterface::play(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    // TODO: Print error
    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<AudioSource>(entity)) {
    return 0;
  }

  entityDatabase.set<AudioStart>(entity, {});
  return 0;
}

int AudioScriptingInterface::LuaInterface::isPlaying(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    // TODO: Print error
    scope.set(false);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<uint32_t>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  bool isPlaying = entityDatabase.has<AudioStatus>(entity);
  scope.set(isPlaying);
  return 1;
}

} // namespace liquid
