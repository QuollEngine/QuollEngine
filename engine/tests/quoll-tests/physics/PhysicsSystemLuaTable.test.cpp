#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/entity/EntityLuaTable.h"
#include "quoll/physics/CollisionEvent.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class PhysicsSystemLuaTable : public LuaScriptingInterfaceTestBase {
public:
  PhysicsSystemLuaTable()
      : LuaScriptingInterfaceTestBase("physics-system-service.lua") {}
};

TEST_F(PhysicsSystemLuaTable, RegistersCollisionStartSignal) {
  auto entity = entityDatabase.entity();
  auto other = entityDatabase.entity();

  auto state = start(entity);

  physicsSystem.getSignals().onCollisionStart().notify(
      quoll::CollisionEvent{.a = entity, .b = other});

  EXPECT_TRUE(state["event"].is<quoll::String>());
  EXPECT_TRUE(state["a"].is<quoll::EntityLuaTable>());
  EXPECT_TRUE(state["b"].is<quoll::EntityLuaTable>());

  EXPECT_EQ(state["event"].get<quoll::String>(), "start");
  EXPECT_EQ(state["a"].get<quoll::EntityLuaTable>().getEntity(), entity);
  EXPECT_EQ(state["b"].get<quoll::EntityLuaTable>().getEntity(), other);
}

TEST_F(PhysicsSystemLuaTable, RegistersCollisionEndSignal) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::Name>({"A"});
  auto other = entityDatabase.entity();
  entity.set<quoll::Name>({"B"});

  auto state = start(entity);

  physicsSystem.getSignals().onCollisionEnd().notify(
      quoll::CollisionEvent{.a = entity, .b = other});

  EXPECT_TRUE(state["event"].is<quoll::String>());
  EXPECT_TRUE(state["a"].is<quoll::EntityLuaTable>());
  EXPECT_TRUE(state["b"].is<quoll::EntityLuaTable>());

  EXPECT_EQ(state["event"].get<quoll::String>(), "end");
  EXPECT_EQ(state["a"].get<quoll::EntityLuaTable>().getEntity(), entity);
  EXPECT_EQ(state["b"].get<quoll::EntityLuaTable>().getEntity(), other);
}
