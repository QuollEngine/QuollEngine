#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid-tests/test-utils/ScriptingInterfaceTestBase.h"

class NameLuaScriptingInterfaceTest : public LuaScriptingInterfaceTestBase {};

TEST_F(NameLuaScriptingInterfaceTest,
       ReturnsEmptyStringIfNameComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();

  auto &scope = call(entity, "name_get");

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  auto name = scope.getGlobal<liquid::String>("name");
  EXPECT_EQ(name, "");
}

TEST_F(NameLuaScriptingInterfaceTest, ReturnsEmptyStringIfNoSelf) {
  auto entity = entityDatabase.createEntity();

  auto &scope = call(entity, "name_get_invalid");

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  auto name = scope.getGlobal<liquid::String>("name");
  EXPECT_EQ(name, "");
}

TEST_F(NameLuaScriptingInterfaceTest, ReturnsNameComponentDataIfExists) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::NameComponent>(entity, {"Test name"});

  auto &scope = call(entity, "name_get");

  auto name = scope.getGlobal<liquid::String>("name");
  EXPECT_EQ(name, "Test name");
}

TEST_F(NameLuaScriptingInterfaceTest, DoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.createEntity();

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  call(entity, "name_set_invalid");
  EXPECT_FALSE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
}

TEST_F(NameLuaScriptingInterfaceTest, CreatesNameComponentOnSet) {
  auto entity = entityDatabase.createEntity();

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  call(entity, "name_set");

  EXPECT_TRUE(entityDatabase.hasComponent<liquid::NameComponent>(entity));
  EXPECT_EQ(entityDatabase.getComponent<liquid::NameComponent>(entity).name,
            "Hello World");
}

TEST_F(NameLuaScriptingInterfaceTest, UpdatesExistingNameComponentOnSet) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::NameComponent>(entity, {"Test name"});

  call(entity, "name_set");

  EXPECT_EQ(entityDatabase.getComponent<liquid::NameComponent>(entity).name,
            "Hello World");
}
