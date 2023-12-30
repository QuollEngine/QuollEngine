#include "quoll/core/Base.h"
#include "quoll/core/Name.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class NameLuaTableTest : public LuaScriptingInterfaceTestBase {};

using NameLuaTableDeathTest = NameLuaTableTest;

TEST_F(NameLuaTableTest, ReturnsEmptyStringIfNameComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "nameGet");

  EXPECT_FALSE(entityDatabase.has<quoll::Name>(entity));
  auto name = state["name"].get<quoll::String>();
  EXPECT_EQ(name, "");
}

TEST_F(NameLuaTableTest, ReturnsNameIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {"Test name"});

  auto state = call(entity, "nameGet");

  auto name = state["name"].get<quoll::String>();
  EXPECT_EQ(name, "Test name");
}

TEST_F(NameLuaTableTest, CreatesNameComponentOnSet) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Name>(entity));
  call(entity, "nameSet");

  EXPECT_TRUE(entityDatabase.has<quoll::Name>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::Name>(entity).name, "Hello World");
}

TEST_F(NameLuaTableTest, UpdatesExistingNameComponentOnSet) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {"Test name"});

  call(entity, "nameSet");

  EXPECT_EQ(entityDatabase.get<quoll::Name>(entity).name, "Hello World");
}

TEST_F(NameLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "nameDelete");
  EXPECT_FALSE(entityDatabase.has<quoll::Name>(entity));
}

TEST_F(NameLuaTableTest, DeleteRemovesNameComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {});

  call(entity, "nameDelete");
  EXPECT_FALSE(entityDatabase.has<quoll::Name>(entity));
}
