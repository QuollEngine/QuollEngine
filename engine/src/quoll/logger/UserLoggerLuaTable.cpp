#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "UserLoggerLuaTable.h"

namespace quoll {

static void logMessages(LogStream &stream, sol::state_view state,
                        sol::variadic_args args) {
  for (auto arg : args) {
    stream << state["tostring"](arg.get<sol::object>()).get<String>() << "\t";
  }
}

sol::table UserLoggerLuaTable::create(sol::state_view state) {
  auto logger = state.create_table();
  logger["debug"] = [state](sol::variadic_args args) {
    auto stream = Engine::getUserLogger().debug();
    logMessages(stream, state, args);
  };

  logger["info"] = [state](sol::variadic_args args) {
    auto stream = Engine::getUserLogger().info();
    logMessages(stream, state, args);
  };

  logger["warning"] = [state](sol::variadic_args args) {
    auto stream = Engine::getUserLogger().warning();
    logMessages(stream, state, args);
  };

  logger["error"] = [state](sol::variadic_args args) {
    auto stream = Engine::getUserLogger().error();
    logMessages(stream, state, args);
  };

  logger["fatal"] = [state](sol::variadic_args args) {
    auto stream = Engine::getUserLogger().fatal();
    logMessages(stream, state, args);
  };

  return logger;
}

} // namespace quoll
