#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class NameLuaTableTest : public LuaScriptingInterfaceTestBase {};

using NameLuaTableDeathTest = NameLuaTableTest;

TEST_F(NameLuaTableTest, ReturnsEmptyStringIfNameComponentDoesNotExist) {
  auto entity = entityDatabase.entity();

  call(entity, "nameGetNil");
}

TEST_F(NameLuaTableTest, ReturnsNameIfComponentExists) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::Name>({"Test name"});

  call(entity, "nameGet");
}

TEST_F(NameLuaTableTest, CreatesNameComponentOnSet) {
  auto entity = entityDatabase.entity();

  EXPECT_FALSE(entity.has<quoll::Name>());
  call(entity, "nameSet");

  EXPECT_TRUE(entity.has<quoll::Name>());
  EXPECT_EQ(entity.get_ref<quoll::Name>()->name, "Hello World");
}

TEST_F(NameLuaTableTest, UpdatesExistingNameComponentOnSet) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::Name>({"Test name"});

  call(entity, "nameSet");

  EXPECT_EQ(entity.get_ref<quoll::Name>()->name, "Hello World");
}
