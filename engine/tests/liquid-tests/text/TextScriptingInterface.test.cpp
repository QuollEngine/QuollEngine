#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid-tests/test-utils/ScriptingInterfaceTestBase.h"

class TextLuaScriptingInterfaceTest : public LuaScriptingInterfaceTestBase {};

TEST_F(TextLuaScriptingInterfaceTest,
       GetTextReturnsEmptyStringIfTextDoesNotExist) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "text_get_text");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  auto name = scope.getGlobal<quoll::String>("text");
  EXPECT_EQ(name, "");
}

TEST_F(TextLuaScriptingInterfaceTest, GetTextReturnsEmptyStringIfNoSelf) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "text_get_text_invalid");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  auto name = scope.getGlobal<quoll::String>("text");
  EXPECT_EQ(name, "");
}

TEST_F(TextLuaScriptingInterfaceTest, GetTextReturnsTextDataIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name"});

  auto &scope = call(entity, "text_get_text");

  auto name = scope.getGlobal<quoll::String>("text");
  EXPECT_EQ(name, "Test name");
}

TEST_F(TextLuaScriptingInterfaceTest,
       SetTextDoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  call(entity, "text_set_text_invalid");
  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
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

  auto &scope = call(entity, "text_get_line_height");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  auto name = scope.getGlobal<float>("text_line_height");
  EXPECT_EQ(name, 0.0f);
}

TEST_F(TextLuaScriptingInterfaceTest, GetLineHeightReturnsZeroIfNoSelf) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "text_get_line_height_invalid");

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  auto name = scope.getGlobal<float>("text_line_height");
  EXPECT_EQ(name, 0.0f);
}

TEST_F(TextLuaScriptingInterfaceTest,
       GetLineHeightReturnsLineHeightIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {"Test name", 25.0f});

  auto &scope = call(entity, "text_get_line_height");

  auto name = scope.getGlobal<float>("text_line_height");
  EXPECT_EQ(name, 25.0f);
}

TEST_F(TextLuaScriptingInterfaceTest,
       SetLineHeightDoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
  call(entity, "text_set_line_height_invalid");
  EXPECT_FALSE(entityDatabase.has<quoll::Text>(entity));
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
       DeleteDoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Text>(entity, {});

  call(entity, "text_delete_invalid");
  EXPECT_TRUE(entityDatabase.has<quoll::Text>(entity));
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
