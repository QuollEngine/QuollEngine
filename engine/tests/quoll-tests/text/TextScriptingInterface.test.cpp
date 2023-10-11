#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"

#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class TextLuaScriptingInterfaceTest : public LuaScriptingInterfaceTestBase {};

TEST_F(TextLuaScriptingInterfaceTest,
       GetTextReturnsEmptyStringIfTextDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "text_get_text");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  EXPECT_TRUE(state["text"].is<sol::nil_t>());
}

TEST_F(TextLuaScriptingInterfaceTest, GetTextReturnsTextDataIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name"});

  auto state = call(entity, "text_get_text");

  auto name = state["text"].get<quoll::String>();
  EXPECT_EQ(name, "Test name");
}

TEST_F(TextLuaScriptingInterfaceTest,
       SetTextDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  call(entity, "text_set_text");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
}

TEST_F(TextLuaScriptingInterfaceTest, SetTextUpdatesExistingTextIfValid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name"});

  call(entity, "text_set_text");

  EXPECT_EQ(entityDatabase.get<quoll::Text>(entity).text, "Hello World");
}

TEST_F(TextLuaScriptingInterfaceTest,
       GetLineHeightReturnsZeroIfTextDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "text_get_line_height");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  EXPECT_TRUE(state["text_line_height"].is<sol::nil_t>());
}

TEST_F(TextLuaScriptingInterfaceTest,
       GetLineHeightReturnsLineHeightIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name", 25.0f});

  auto state = call(entity, "text_get_line_height");

  auto name = state["text_line_height"].get<float>();
  EXPECT_EQ(name, 25.0f);
}

TEST_F(TextLuaScriptingInterfaceTest,
       SetLineHeightDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  call(entity, "text_set_line_height");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
}

TEST_F(TextLuaScriptingInterfaceTest,
       SetLineHeightUpdatesExistingLineHeightIfValid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name"});

  call(entity, "text_set_line_height");

  EXPECT_EQ(entityDatabase.get<quoll::Text>(entity).lineHeight, 12.0f);
}

TEST_F(TextLuaScriptingInterfaceTest,
       DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "text_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
}

TEST_F(TextLuaScriptingInterfaceTest, DeleteRemovesTextComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {});

  call(entity, "text_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
}
