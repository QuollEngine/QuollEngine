#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "NameScriptingInterface.h"

namespace quoll {

NameScriptingInterface::LuaInterface::LuaInterface(Entity entity,
                                                   ScriptGlobals scriptGlobals)
    : mScriptGlobals(scriptGlobals), mEntity(entity) {}

String NameScriptingInterface::LuaInterface::get() {
  if (mScriptGlobals.entityDatabase.has<Name>(mEntity)) {
    return mScriptGlobals.entityDatabase.get<Name>(mEntity).name;
  }

  return "";
}

void NameScriptingInterface::LuaInterface::set(String name) {
  mScriptGlobals.entityDatabase.set<Name>(mEntity, {name});
}

void NameScriptingInterface::LuaInterface::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<Name>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<Name>(mEntity);
  }
}

void NameScriptingInterface::LuaInterface::create(
    sol::usertype<NameScriptingInterface::LuaInterface> usertype) {
  usertype["get"] = &LuaInterface::get;
  usertype["set"] = &LuaInterface::set;
  usertype["delete"] = &LuaInterface::deleteThis;
}

} // namespace quoll
