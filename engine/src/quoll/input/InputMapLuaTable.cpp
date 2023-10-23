#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/input/InputMap.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "InputMapLuaTable.h"

namespace quoll {

InputMapLuaTable::InputMapLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol_maybe<usize> InputMapLuaTable::getCommand(String name) {
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

sol_maybe<bool> InputMapLuaTable::getCommandValueBoolean(usize command) {
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

std::tuple<sol_maybe<f32>, sol_maybe<f32>>
InputMapLuaTable::getCommandValueAxis2d(usize command) {
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

void InputMapLuaTable::setScheme(String name) {
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

void InputMapLuaTable::create(sol::usertype<InputMapLuaTable> usertype) {
  usertype["get_command"] = &InputMapLuaTable::getCommand;
  usertype["get_value_boolean"] = &InputMapLuaTable::getCommandValueBoolean;
  usertype["is_pressed"] = &InputMapLuaTable::getCommandValueBoolean;
  usertype["get_value_axis_2d"] = &InputMapLuaTable::getCommandValueAxis2d;
  usertype["set_scheme"] = &InputMapLuaTable::setScheme;
}

} // namespace quoll
