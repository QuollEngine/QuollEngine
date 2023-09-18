#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaScope.h"
#include "quoll/scripting/LuaMessages.h"

#include "InputMapScriptingInterface.h"

namespace quoll {

int InputMapScriptingInterface::LuaInterface::getCommand(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_command");

    scope.set(nullptr);
    return 1;
  }

  if (!scope.is<String>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<String>(getName(), "get_command");

    scope.set(nullptr);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  auto name = scope.get<String>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<InputMap>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);
    scope.set(nullptr);
    return 1;
  }

  const auto &inputMap = entityDatabase.get<InputMap>(entity);
  if (!inputMap.commandNameMap.contains(name)) {
    Engine::getUserLogger().error()
        << "Input command \"" << name << "\" does not exist";
    scope.set(nullptr);
    return 1;
  }

  scope.set(inputMap.commandNameMap.at(name));

  return 1;
}

int InputMapScriptingInterface::LuaInterface::getCommandValueBoolean(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_command_value_boolean");

    scope.set(false);
    return 1;
  }

  if (!scope.is<uint32_t>(2)) {
    Engine::getUserLogger().error() << LuaMessages::invalidArguments<uint32_t>(
        getName(), "get_command_value_boolean");

    scope.set(false);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  auto command = static_cast<size_t>(scope.get<uint32_t>(2));
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<InputMap>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);
    scope.set(false);
    return 1;
  }

  const auto &inputMap = entityDatabase.get<InputMap>(entity);
  if (command >= inputMap.commandValues.size()) {
    Engine::getUserLogger().error() << "Invalid command provided";
    scope.set(false);
    return 1;
  }

  const auto &value = inputMap.commandValues.at(command);

  if (auto *v = std::get_if<bool>(&value)) {
    scope.set(*v);
  } else if (auto *v = std::get_if<glm::vec2>(&value)) {
    if (v->x == 0.0f && v->y == 0.0f) {
      scope.set(false);
    } else {
      scope.set(true);
    }
  } else {
    scope.set(false);
  }

  return 1;
}

int InputMapScriptingInterface::LuaInterface::getCommandValueAxis2d(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_command_value_boolean");

    scope.set(0.0f);
    scope.set(0.0f);
    return 2;
  }

  if (!scope.is<uint32_t>(2)) {
    Engine::getUserLogger().error() << LuaMessages::invalidArguments<uint32_t>(
        getName(), "get_command_value_boolean");

    scope.set(0.0f);
    scope.set(0.0f);
    return 2;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  auto command = static_cast<size_t>(scope.get<uint32_t>(2));
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<InputMap>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);
    scope.set(0.0f);
    scope.set(0.0f);
    return 2;
  }

  const auto &inputMap = entityDatabase.get<InputMap>(entity);
  if (command >= inputMap.commandValues.size()) {
    Engine::getUserLogger().error() << "Invalid command provided";
    scope.set(0.0f);
    scope.set(0.0f);
    return 2;
  }

  const auto &value = inputMap.commandValues.at(command);

  if (auto *v = std::get_if<bool>(&value)) {
    scope.set(*v == true ? 1.0f : 0.0f);
    scope.set(*v == true ? 1.0f : 0.0f);
  } else if (auto *v = std::get_if<glm::vec2>(&value)) {
    scope.set(v->x);
    scope.set(v->y);
  } else {
    scope.set(0.0f);
    scope.set(0.0f);
  }

  return 2;
}

} // namespace quoll
