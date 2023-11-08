#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "UserLoggerLuaTable.h"

namespace quoll {

void UserLoggerLuaTable::create(sol::state_view state) {
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
