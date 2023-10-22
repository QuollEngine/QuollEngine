#include "quoll/core/Base.h"
#include "quoll/text/Text.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class TextLuaTableTest : public LuaScriptingInterfaceTestBase {};

TEST_F(TextLuaTableTest, GetTextReturnsEmptyStringIfTextDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "text_get_text");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  EXPECT_TRUE(state["text"].is<sol::nil_t>());
}

TEST_F(TextLuaTableTest, GetTextReturnsTextDataIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name"});

  auto state = call(entity, "text_get_text");

  auto name = state["text"].get<quoll::String>();
  EXPECT_EQ(name, "Test name");
}

TEST_F(TextLuaTableTest, SetTextDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  call(entity, "text_set_text");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
}

TEST_F(TextLuaTableTest, SetTextUpdatesExistingTextIfValid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name"});

  call(entity, "text_set_text");

  EXPECT_EQ(entityDatabase.get<quoll::Text>(entity).text, "Hello World");
}

TEST_F(TextLuaTableTest, GetLineHeightReturnsZeroIfTextDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "text_get_line_height");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  EXPECT_TRUE(state["text_line_height"].is<sol::nil_t>());
}

TEST_F(TextLuaTableTest, GetLineHeightReturnsLineHeightIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name", 25.0f});

  auto state = call(entity, "text_get_line_height");

  auto name = state["text_line_height"].get<f32>();
  EXPECT_EQ(name, 25.0f);
}

TEST_F(TextLuaTableTest, SetLineHeightDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  call(entity, "text_set_line_height");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
}

TEST_F(TextLuaTableTest, SetLineHeightUpdatesExistingLineHeightIfValid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name"});

  call(entity, "text_set_line_height");

  EXPECT_EQ(entityDatabase.get<quoll::Text>(entity).lineHeight, 12.0f);
}

TEST_F(TextLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "text_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
}

TEST_F(TextLuaTableTest, DeleteRemovesTextComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {});

  call(entity, "text_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
}
