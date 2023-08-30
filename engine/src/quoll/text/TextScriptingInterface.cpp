#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"
#include "quoll/scripting/LuaScope.h"
#include "quoll/scripting/ComponentLuaInterfaceCommon.h"

#include "TextScriptingInterface.h"

namespace quoll {

int TextScriptingInterface::LuaInterface::getText(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_text");

    scope.set<String>("");
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (entityDatabase.has<Text>(entity)) {
    scope.set(entityDatabase.get<Text>(entity).text);
  } else {
    scope.set<String>("");
  }

  return 1;
}

int TextScriptingInterface::LuaInterface::setText(void *state) {
  LuaScope scope(state);
  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_text");

    return 0;
  }

  if (!scope.is<String>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<String>(getName(), "set_text");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  auto text = scope.get<String>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  // Text needs to exist in order to change it
  if (!entityDatabase.has<Text>(entity)) {
    return 0;
  }

  entityDatabase.get<Text>(entity).text = text;

  return 0;
}
int TextScriptingInterface::LuaInterface::getLineHeight(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_line_height");

    scope.set<String>("");
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (entityDatabase.has<Text>(entity)) {
    scope.set(entityDatabase.get<Text>(entity).lineHeight);
  } else {
    scope.set<float>(0.0f);
  }

  return 1;
}
int TextScriptingInterface::LuaInterface::setLineHeight(void *state) {
  LuaScope scope(state);
  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_line_height");

    return 0;
  }

  if (!scope.is<float>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float>(getName(), "set_line_height");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  auto lineHeight = scope.get<float>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  // Text needs to exist in order to change it
  if (!entityDatabase.has<Text>(entity)) {
    return 0;
  }

  entityDatabase.get<Text>(entity).lineHeight = lineHeight;

  return 0;
};

int TextScriptingInterface::LuaInterface::deleteThis(void *state) {
  return ComponentLuaInterfaceCommon::deleteComponent<Text>(getName(), state);
}

} // namespace quoll
