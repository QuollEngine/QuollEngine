#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaScope.h"
#include "quoll/scripting/LuaMessages.h"
#include "quoll/scripting/ComponentLuaInterfaceCommon.h"

#include "AudioScriptingInterface.h"

namespace quoll {

int AudioScriptingInterface::LuaInterface::play(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "play");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
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
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "is_playing");
    scope.set(false);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  bool isPlaying = entityDatabase.has<AudioStatus>(entity);
  scope.set(isPlaying);
  return 1;
}

int AudioScriptingInterface::LuaInterface::deleteThis(void *state) {
  return ComponentLuaInterfaceCommon::deleteComponent<AudioSource>(getName(),
                                                                   state);
}

} // namespace quoll
