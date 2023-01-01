#include "liquid/core/Base.h"
#include "liquid/scripting/ScriptDecorator.h"

#include "EntityQuery.h"
#include "EntityQueryScriptingInterface.h"

namespace liquid {

int EntityQueryScriptingInterface::LuaInterface::getFirstEntityByName(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<String>(1)) {
    scope.set(nullptr);
    // TODO: Show logs here
    return 1;
  }

  auto arg = scope.get<StringView>(1);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  EntityQuery query(entityDatabase);

  auto entity = query.getFirstEntityByName(arg);
  if (entity == EntityNull) {
    scope.set(nullptr);
  } else {
    ScriptDecorator::createEntityTable(scope, entity);
  }

  return 1;
}

} // namespace liquid
