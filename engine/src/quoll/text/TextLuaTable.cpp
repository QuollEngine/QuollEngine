#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/text/Text.h"

#include "quoll/entity/EntityDatabase.h"

#include "TextLuaTable.h"

namespace quoll {

TextLuaTable::TextLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol_maybe<String> TextLuaTable::getContent() {
  if (mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    return mScriptGlobals.entityDatabase.get<Text>(mEntity).content;
  }

  return sol::nil;
}

void TextLuaTable::setContent(String content) {
  // Text needs to exist in order to change it
  if (!mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    return;
  }

  mScriptGlobals.entityDatabase.get<Text>(mEntity).content = content;
}

sol_maybe<f32> TextLuaTable::getLineHeight() {
  if (mScriptGlobals.entityDatabase.has<Text>(mEntity)) {
    return mScriptGlobals.entityDatabase.get<Text>(mEntity).lineHeight;
  }

  return sol::nil;
}

void TextLuaTable::setLineHeight(f32 lineHeight) {
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

void TextLuaTable::create(sol::usertype<TextLuaTable> usertype,
                          sol::state_view state) {
  usertype["content"] =
      sol::property(&TextLuaTable::getContent, &TextLuaTable::setContent);
  usertype["lineHeight"] =
      sol::property(&TextLuaTable::getLineHeight, &TextLuaTable::setLineHeight);
  usertype["delete"] = &TextLuaTable::deleteThis;
}

} // namespace quoll
