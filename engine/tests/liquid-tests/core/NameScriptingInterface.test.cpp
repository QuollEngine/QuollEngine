#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid-tests/test-utils/ScriptingInterfaceTestBase.h"

class NameLuaScriptingInterfaceTest : public LuaScriptingInterfaceTestBase {};

TEST_F(NameLuaScriptingInterfaceTest,
       ReturnsEmptyStringIfNameComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "name_get");

  EXPECT_FALSE(entityDatabase.has<liquid::Name>(entity));
  auto name = scope.getGlobal<liquid::String>("name");
  EXPECT_EQ(name, "");
}

TEST_F(NameLuaScriptingInterfaceTest, ReturnsEmptyStringIfNoSelf) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "name_get_invalid");

  EXPECT_FALSE(entityDatabase.has<liquid::Name>(entity));
  auto name = scope.getGlobal<liquid::String>("name");
  EXPECT_EQ(name, "");
}

TEST_F(NameLuaScriptingInterfaceTest, ReturnsNameComponentDataIfExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Name>(entity, {"Test name"});

  auto &scope = call(entity, "name_get");

  auto name = scope.getGlobal<liquid::String>("name");
  EXPECT_EQ(name, "Test name");
}

TEST_F(NameLuaScriptingInterfaceTest, DoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<liquid::Name>(entity));
  call(entity, "name_set_invalid");
  EXPECT_FALSE(entityDatabase.has<liquid::Name>(entity));
}

TEST_F(NameLuaScriptingInterfaceTest, CreatesNameComponentOnSet) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<liquid::Name>(entity));
  call(entity, "name_set");

  EXPECT_TRUE(entityDatabase.has<liquid::Name>(entity));
  EXPECT_EQ(entityDatabase.get<liquid::Name>(entity).name, "Hello World");
}

TEST_F(NameLuaScriptingInterfaceTest, UpdatesExistingNameComponentOnSet) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Name>(entity, {"Test name"});

  call(entity, "name_set");

  EXPECT_EQ(entityDatabase.get<liquid::Name>(entity).name, "Hello World");
}
