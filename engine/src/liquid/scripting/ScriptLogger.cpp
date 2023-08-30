#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "LuaScope.h"
#include "LuaMessages.h"
#include "ScriptLogger.h"

namespace quoll {

void createScriptLogger(LuaScope &scope) {
  auto logTable = scope.createTable(1);
  logTable.set("debug", [](void *state) {
    LuaScope scope(state);
    if (!scope.is<String>(1)) {
      Engine::getUserLogger().error()
          << LuaMessages::invalidArguments<String>("logger", "debug");

      return 0;
    }

    Engine::getUserLogger().debug() << scope.get<String>(1);

    return 0;
  });

  logTable.set("info", [](void *state) {
    LuaScope scope(state);
    if (!scope.is<String>(1)) {
      Engine::getUserLogger().error()
          << LuaMessages::invalidArguments<String>("logger", "info");

      return 0;
    }

    Engine::getUserLogger().info() << scope.get<String>(1);

    return 0;
  });

  logTable.set("warning", [](void *state) {
    LuaScope scope(state);
    if (!scope.is<String>(1)) {
      Engine::getUserLogger().error()
          << LuaMessages::invalidArguments<String>("logger", "warning");

      return 0;
    }

    Engine::getUserLogger().warning() << scope.get<String>(1);

    return 0;
  });

  logTable.set("error", [](void *state) {
    LuaScope scope(state);
    if (!scope.is<String>(1)) {
      Engine::getUserLogger().error()
          << LuaMessages::invalidArguments<String>("logger", "error");
      return 0;
    }

    Engine::getUserLogger().error() << scope.get<String>(1);

    return 0;
  });

  logTable.set("fatal", [](void *state) {
    LuaScope scope(state);
    if (!scope.is<String>(1)) {
      Engine::getUserLogger().error()
          << LuaMessages::invalidArguments<String>("logger", "fatal");
      return 0;
    }

    Engine::getUserLogger().fatal() << scope.get<String>(1);

    return 0;
  });
}

} // namespace quoll
