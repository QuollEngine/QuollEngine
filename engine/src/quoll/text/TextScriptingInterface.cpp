#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "TextScriptingInterface.h"

namespace quoll {

TextScriptingInterface::LuaInterface::LuaInterface(Entity entity,
                                                   ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol_maybe<String> TextScriptingInterface::LuaInterface::getText() {
  if (mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    return mScriptGlobals.entityDatabase.get<Text>(mEntity).text;
  }

  return sol::nil;
}

void TextScriptingInterface::LuaInterface::setText(String text) {
  // Text needs to exist in order to change it
  if (!mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    return;
  }

  mScriptGlobals.entityDatabase.get<Text>(mEntity).text = text;
}

sol_maybe<float> TextScriptingInterface::LuaInterface::getLineHeight() {
  if (mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    return mScriptGlobals.entityDatabase.get<Text>(mEntity).lineHeight;
  }

  return sol::nil;
}

void TextScriptingInterface::LuaInterface::setLineHeight(float lineHeight) {
  // Text needs to exist in order to change it
  if (!mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    return;
  }

  mScriptGlobals.entityDatabase.get<Text>(mEntity).lineHeight = lineHeight;
};

void TextScriptingInterface::LuaInterface::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<Text>(mEntity);
  }
}

void TextScriptingInterface::LuaInterface::create(
    sol::usertype<TextScriptingInterface::LuaInterface> usertype) {
  usertype["get_text"] = &LuaInterface::getText;
  usertype["set_text"] = &LuaInterface::setText;
  usertype["get_line_height"] = &LuaInterface::getLineHeight;
  usertype["set_line_height"] = &LuaInterface::setLineHeight;
  usertype["delete"] = &LuaInterface::deleteThis;
}

} // namespace quoll
