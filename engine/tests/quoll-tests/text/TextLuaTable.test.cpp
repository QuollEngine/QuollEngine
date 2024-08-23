#include "quoll/core/Base.h"
#include "quoll/text/Text.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class TextLuaTableTest : public LuaScriptingInterfaceTestBase {};

TEST_F(TextLuaTableTest, GetTextReturnsEmptyStringIfTextDoesNotExist) {
  auto entity = entityDatabase.entity();

  auto state = call(entity, "textGetTextNil");

  EXPECT_FALSE(entity.has<quoll::Text>());
  EXPECT_TRUE(state["text"].is<sol::nil_t>());
}

TEST_F(TextLuaTableTest, GetTextReturnsTextDataIfComponentExists) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::Text>({"Test content"});

  call(entity, "textGetText");
}

TEST_F(TextLuaTableTest, SetTextDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.entity();

  EXPECT_FALSE(entity.has<quoll::Text>());
  call(entity, "textSetText");

  EXPECT_FALSE(entity.has<quoll::Text>());
}

TEST_F(TextLuaTableTest, SetTextUpdatesExistingTextIfValid) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::Text>({"Test name"});

  call(entity, "textSetText");
}

TEST_F(TextLuaTableTest, GetLineHeightReturnsZeroIfTextDoesNotExist) {
  auto entity = entityDatabase.entity();
  call(entity, "textGetLineHeightNil");
}

TEST_F(TextLuaTableTest, GetLineHeightReturnsLineHeightIfComponentExists) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::Text>({"Test name", 25.0f});

  call(entity, "textGetLineHeight");
}

TEST_F(TextLuaTableTest, SetLineHeightDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.entity();

  EXPECT_FALSE(entity.has<quoll::Text>());
  call(entity, "textSetLineHeight");

  EXPECT_FALSE(entity.has<quoll::Text>());
}

TEST_F(TextLuaTableTest, SetLineHeightUpdatesExistingLineHeightIfValid) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::Text>({"Test name"});

  call(entity, "textSetLineHeight");

  EXPECT_EQ(entity.get_ref<quoll::Text>()->lineHeight, 12.0f);
}

TEST_F(TextLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.entity();

  call(entity, "textDelete");
  EXPECT_FALSE(entity.has<quoll::Text>());
}

TEST_F(TextLuaTableTest, DeleteRemovesTextComponentFromEntity) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::Text>({});

  call(entity, "textDelete");
  EXPECT_FALSE(entity.has<quoll::Text>());
}
