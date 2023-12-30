#include "quoll/core/Base.h"
#include "quoll/text/Text.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class TextLuaTableTest : public LuaScriptingInterfaceTestBase {};

TEST_F(TextLuaTableTest, GetTextReturnsEmptyStringIfTextDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "textGetText");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  EXPECT_TRUE(state["text"].is<sol::nil_t>());
}

TEST_F(TextLuaTableTest, GetTextReturnsTextDataIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name"});

  auto state = call(entity, "textGetText");

  auto name = state["text"].get<quoll::String>();
  EXPECT_EQ(name, "Test name");
}

TEST_F(TextLuaTableTest, SetTextDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  call(entity, "textSetText");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
}

TEST_F(TextLuaTableTest, SetTextUpdatesExistingTextIfValid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name"});

  call(entity, "textSetText");

  EXPECT_EQ(entityDatabase.get<quoll::Text>(entity).text, "Hello World");
}

TEST_F(TextLuaTableTest, GetLineHeightReturnsZeroIfTextDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "textGetLineHeight");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  EXPECT_TRUE(state["textLineHeight"].is<sol::nil_t>());
}

TEST_F(TextLuaTableTest, GetLineHeightReturnsLineHeightIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name", 25.0f});

  auto state = call(entity, "textGetLineHeight");

  auto name = state["textLineHeight"].get<f32>();
  EXPECT_EQ(name, 25.0f);
}

TEST_F(TextLuaTableTest, SetLineHeightDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  call(entity, "textSetLineHeight");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
}

TEST_F(TextLuaTableTest, SetLineHeightUpdatesExistingLineHeightIfValid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name"});

  call(entity, "textSetLineHeight");

  EXPECT_EQ(entityDatabase.get<quoll::Text>(entity).lineHeight, 12.0f);
}

TEST_F(TextLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "textDelete");
  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
}

TEST_F(TextLuaTableTest, DeleteRemovesTextComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {});

  call(entity, "textDelete");
  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
}
