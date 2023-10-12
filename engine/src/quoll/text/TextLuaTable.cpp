#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "TextLuaTable.h"

namespace quoll {

TextLuaTable::TextLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol_maybe<String> TextLuaTable::getText() {
  if (mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    return mScriptGlobals.entityDatabase.get<Text>(mEntity).text;
  }

  return sol::nil;
}

void TextLuaTable::setText(String text) {
  // Text needs to exist in order to change it
  if (!mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    return;
  }

  mScriptGlobals.entityDatabase.get<Text>(mEntity).text = text;
}

sol_maybe<float> TextLuaTable::getLineHeight() {
  if (mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    return mScriptGlobals.entityDatabase.get<Text>(mEntity).lineHeight;
  }

  return sol::nil;
}

void TextLuaTable::setLineHeight(float lineHeight) {
  // Text needs to exist in order to change it
  if (!mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    return;
  }

  mScriptGlobals.entityDatabase.get<Text>(mEntity).lineHeight = lineHeight;
};

void TextLuaTable::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<Text>(mEntity);
  }
}

void TextLuaTable::create(sol::usertype<TextLuaTable> usertype) {
  usertype["get_text"] = &TextLuaTable::getText;
  usertype["set_text"] = &TextLuaTable::setText;
  usertype["get_line_height"] = &TextLuaTable::getLineHeight;
  usertype["set_line_height"] = &TextLuaTable::setLineHeight;
  usertype["delete"] = &TextLuaTable::deleteThis;
}

} // namespace quoll
