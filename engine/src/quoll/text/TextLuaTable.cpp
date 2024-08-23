#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/text/Text.h"
#include "TextLuaTable.h"

namespace quoll {

TextLuaTable::TextLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol_maybe<String> TextLuaTable::getContent() {
  if (mEntity.has<Text>()) {
    return mEntity.get_ref<Text>()->content;
  }

  return sol::nil;
}

void TextLuaTable::setContent(String content) {
  // Text needs to exist in order to change it
  if (!mEntity.has<Text>()) {
    return;
  }

  mEntity.get_ref<Text>()->content = content;
}

sol_maybe<f32> TextLuaTable::getLineHeight() {
  if (mEntity.has<Text>()) {
    return mEntity.get_ref<Text>()->lineHeight;
  }

  return sol::nil;
}

void TextLuaTable::setLineHeight(f32 lineHeight) {
  // Text needs to exist in order to change it
  if (!mEntity.has<Text>()) {
    return;
  }

  mEntity.get_ref<Text>()->lineHeight = lineHeight;
};

void TextLuaTable::deleteThis() {
  if (mEntity.has<Text>()) {
    mEntity.remove<Text>();
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
