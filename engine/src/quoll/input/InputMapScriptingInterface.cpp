#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "InputMapScriptingInterface.h"

namespace quoll {

InputMapScriptingInterface::LuaInterface::LuaInterface(
    Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol_maybe<size_t>
InputMapScriptingInterface::LuaInterface::getCommand(String name) {
  if (!mScriptGlobals.entityDatabase.has<InputMap>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  const auto &inputMap = mScriptGlobals.entityDatabase.get<InputMap>(mEntity);
  if (!inputMap.commandNameMap.contains(name)) {
    Engine::getUserLogger().error()
        << "Input command \"" << name << "\" does not exist";
    return sol::nil;
  }

  return inputMap.commandNameMap.at(name);
}

sol_maybe<bool>
InputMapScriptingInterface::LuaInterface::getCommandValueBoolean(
    size_t command) {
  if (!mScriptGlobals.entityDatabase.has<InputMap>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  const auto &inputMap = mScriptGlobals.entityDatabase.get<InputMap>(mEntity);
  if (command >= inputMap.commandValues.size()) {
    Engine::getUserLogger().error() << "Invalid command provided";
    return sol::nil;
  }

  const auto &value = inputMap.commandValues.at(command);
  if (auto *v = std::get_if<bool>(&value)) {
    return *v;
  }

  if (auto *v = std::get_if<glm::vec2>(&value)) {
    return v->x != 0.0f || v->y != 0.0f;
  }

  return sol::nil;
}

std::tuple<sol_maybe<float>, sol_maybe<float>>
InputMapScriptingInterface::LuaInterface::getCommandValueAxis2d(
    size_t command) {
  if (!mScriptGlobals.entityDatabase.has<InputMap>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return {sol::nil, sol::nil};
  }

  const auto &inputMap = mScriptGlobals.entityDatabase.get<InputMap>(mEntity);
  if (command >= inputMap.commandValues.size()) {
    Engine::getUserLogger().error() << "Invalid command provided";
    return {sol::nil, sol::nil};
  }

  const auto &value = inputMap.commandValues.at(command);

  if (auto *v = std::get_if<bool>(&value)) {
    auto value = *v == true ? 1.0f : 0.0f;
    return {value, value};
  }

  if (auto *v = std::get_if<glm::vec2>(&value)) {
    return {v->x, v->y};
  }

  return {sol::nil, sol::nil};
}

void InputMapScriptingInterface::LuaInterface::setScheme(String name) {
  if (!mScriptGlobals.entityDatabase.has<InputMap>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return;
  }

  auto &inputMap = mScriptGlobals.entityDatabase.get<InputMap>(mEntity);
  if (!inputMap.schemeNameMap.contains(name)) {
    Engine::getUserLogger().error()
        << "Input scheme \"" << name << "\" does not exist";
    return;
  }

  inputMap.activeScheme = inputMap.schemeNameMap.at(name);
}

void InputMapScriptingInterface::LuaInterface::create(
    sol::usertype<InputMapScriptingInterface::LuaInterface> usertype) {
  usertype["get_command"] = &LuaInterface::getCommand;
  usertype["get_value_boolean"] = &LuaInterface::getCommandValueBoolean;
  usertype["is_pressed"] = &LuaInterface::getCommandValueBoolean;
  usertype["get_value_axis_2d"] = &LuaInterface::getCommandValueAxis2d;
  usertype["set_scheme"] = &LuaInterface::setScheme;
}

} // namespace quoll
