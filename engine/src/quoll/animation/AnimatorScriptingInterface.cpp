#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaScope.h"
#include "quoll/scripting/LuaMessages.h"
#include "quoll/scripting/ComponentLuaInterfaceCommon.h"

#include "AnimatorScriptingInterface.h"

namespace quoll {

int AnimatorScriptingInterface::LuaInterface::trigger(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "trigger");

    return 0;
  }

  if (!scope.is<String>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<String>(getName(), "trigger");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  auto eventName = scope.get<String>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  entityDatabase.set<AnimatorEvent>(entity, {eventName});
  return 0;
}

int AnimatorScriptingInterface::LuaInterface::deleteThis(void *state) {
  return ComponentLuaInterfaceCommon::deleteComponent<Animator>(getName(),
                                                                state);
}

} // namespace quoll
