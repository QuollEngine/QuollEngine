#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid-tests/test-utils/ScriptingInterfaceTestBase.h"

class TextLuaScriptingInterfaceTest : public LuaScriptingInterfaceTestBase {};

TEST_F(TextLuaScriptingInterfaceTest,
       ReturnsEmptyStringIfTextComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "text_get_text");

  EXPECT_FALSE(entityDatabase.has<liquid::TextComponent>(entity));
  auto name = scope.getGlobal<liquid::String>("text");
  EXPECT_EQ(name, "");
}

TEST_F(TextLuaScriptingInterfaceTest, ReturnsEmptyStringIfNoSelf) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "text_get_text_invalid");

  EXPECT_FALSE(entityDatabase.has<liquid::TextComponent>(entity));
  auto name = scope.getGlobal<liquid::String>("text");
  EXPECT_EQ(name, "");
}

TEST_F(TextLuaScriptingInterfaceTest,
       ReturnsTextComponentTextDataIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::TextComponent>(entity, {"Test name"});

  auto &scope = call(entity, "text_get_text");

  auto name = scope.getGlobal<liquid::String>("text");
  EXPECT_EQ(name, "Test name");
}

TEST_F(TextLuaScriptingInterfaceTest, DoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<liquid::TextComponent>(entity));
  call(entity, "text_set_text_invalid");
  EXPECT_FALSE(entityDatabase.has<liquid::TextComponent>(entity));
}

TEST_F(TextLuaScriptingInterfaceTest, DoesNothingIfCOmponentDOesNotExist) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<liquid::TextComponent>(entity));
  call(entity, "text_set_text");

  EXPECT_FALSE(entityDatabase.has<liquid::TextComponent>(entity));
}

TEST_F(TextLuaScriptingInterfaceTest, UpdatesExistingTextComponentOnSet) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::TextComponent>(entity, {"Test name"});

  call(entity, "text_set_text");

  EXPECT_EQ(entityDatabase.get<liquid::TextComponent>(entity).text,
            "Hello World");
}
