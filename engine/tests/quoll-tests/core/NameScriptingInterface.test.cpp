#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"

#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class NameLuaScriptingInterfaceTest : public LuaScriptingInterfaceTestBase {};

using NameLuaScriptingInterfaceDeathTest = NameLuaScriptingInterfaceTest;

TEST_F(NameLuaScriptingInterfaceTest,
       ReturnsEmptyStringIfNameComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "name_get");

  EXPECT_FALSE(entityDatabase.has<quoll::Name>(entity));
  auto name = state["name"].get<quoll::String>();
  EXPECT_EQ(name, "");
}

TEST_F(NameLuaScriptingInterfaceTest, ReturnsNameIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {"Test name"});

  auto state = call(entity, "name_get");

  auto name = state["name"].get<quoll::String>();
  EXPECT_EQ(name, "Test name");
}

TEST_F(NameLuaScriptingInterfaceTest, CreatesNameComponentOnSet) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Name>(entity));
  call(entity, "name_set");

  EXPECT_TRUE(entityDatabase.has<quoll::Name>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::Name>(entity).name, "Hello World");
}

TEST_F(NameLuaScriptingInterfaceTest, UpdatesExistingNameComponentOnSet) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {"Test name"});

  call(entity, "name_set");

  EXPECT_EQ(entityDatabase.get<quoll::Name>(entity).name, "Hello World");
}

TEST_F(NameLuaScriptingInterfaceTest,
       DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "name_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Name>(entity));
}

TEST_F(NameLuaScriptingInterfaceTest, DeleteRemovesNameComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {});

  call(entity, "name_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Name>(entity));
}
