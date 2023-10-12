#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "LuaHeaders.h"
#include "LuaMessages.h"
#include "ScriptLogger.h"

namespace quoll {

void createScriptLogger(sol::state_view state) {
  auto logger = state.create_named_table("logger");
  logger["debug"] = [](String message) {
    Engine::getUserLogger().debug() << message;
  };

  logger["info"] = [](String message) {
    Engine::getUserLogger().info() << message;
  };

  logger["warning"] = [](String message) {
    Engine::getUserLogger().warning() << message;
  };

  logger["error"] = [](String message) {
    Engine::getUserLogger().error() << message;
  };

  logger["fatal"] = [](String message) {
    Engine::getUserLogger().fatal() << message;
  };
}

} // namespace quoll
