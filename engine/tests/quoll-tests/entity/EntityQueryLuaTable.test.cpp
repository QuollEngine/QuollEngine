#include "quoll/core/Base.h"
#include "quoll/core/Delete.h"
#include "quoll/core/Name.h"
#include "quoll/entity/EntityLuaTable.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using EntityQueryLuaTableTest = LuaScriptingInterfaceTestBase;

TEST_F(EntityQueryLuaTableTest,
       GetEntityByNameReturnsNulllIfEntityDoesNotExist) {
  auto entity = entityDatabase.entity();

  auto state = call(entity, "entityQueryGetFirstByName");
  EXPECT_TRUE(state["foundEntity"].is<sol::nil_t>());
}

TEST_F(EntityQueryLuaTableTest,
       GetEntityByNameReturnsEntityTableIfEntityExists) {
  auto entity = entityDatabase.entity();

  auto e1 = entityDatabase.entity();
  e1.set<quoll::Name>({"Test"});

  auto state = call(entity, "entityQueryGetFirstByName");

  EXPECT_TRUE(state["foundEntity"].is<quoll::EntityLuaTable>());
  EXPECT_EQ(state["foundEntity"].get<quoll::EntityLuaTable>().getEntity(), e1);
}

TEST_F(EntityQueryLuaTableTest,
       DeleteEntityAddsDeleteComponentToExistingEntity) {
  auto entity = entityDatabase.entity();

  call(entity, "entityQueryDeleteEntity");
  EXPECT_TRUE(entity.has<quoll::Delete>());
}
