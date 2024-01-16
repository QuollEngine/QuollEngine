#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/Parent.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class ParentLuaTableTest : public LuaScriptingInterfaceTestBase {};

// NOTE: Children are not asserted here because they are already tested

TEST_F(ParentLuaTableTest, GetReturnsNilIfEntityHasNoParent) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {"Child"});

  call(entity, "parentGetNil");
}

TEST_F(ParentLuaTableTest, GetReturnsParentOfEntityIfEntityHasParent) {
  auto parent = entityDatabase.create();
  entityDatabase.set<quoll::Name>(parent, {"Parent"});
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {"Entity"});
  entityDatabase.set<quoll::Parent>(entity, {parent});

  call(entity, "parentGet");
}

TEST_F(ParentLuaTableTest, SetAssignsParentToEntityAndChildToNewParent) {
  auto parent1 = entityDatabase.create();
  entityDatabase.set<quoll::Name>(parent1, {"Parent 1"});

  auto parent2 = entityDatabase.create();
  entityDatabase.set<quoll::Name>(parent2, {"Parent 2"});

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {"Entity"});

  entityDatabase.set<quoll::Parent>(entity, {parent1});
  entityDatabase.set<quoll::Children>(parent1, {{entity}});

  auto entity2 = entityDatabase.create();
  entityDatabase.set<quoll::Parent>(entity2, {parent2});
  entityDatabase.set<quoll::Children>(parent2, {{entity2}});

  call(entity, "parentSet");

  EXPECT_EQ(entityDatabase.get<quoll::Parent>(entity).parent, parent2);
}

TEST_F(
    ParentLuaTableTest,
    SetAssignsParentToEntityAndCreatesChildrenComponentToParentIfParentHasNoChildren) {
  auto parent1 = entityDatabase.create();
  entityDatabase.set<quoll::Name>(parent1, {"Parent 1"});

  auto parent2 = entityDatabase.create();
  entityDatabase.set<quoll::Name>(parent2, {"Parent 2"});

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {"Child"});

  entityDatabase.set<quoll::Parent>(entity, {parent1});
  entityDatabase.set<quoll::Children>(parent1, {{entity}});

  call(entity, "parentSet");

  EXPECT_EQ(entityDatabase.get<quoll::Parent>(entity).parent, parent2);
}

TEST_F(ParentLuaTableTest,
       SetNilRemovesParentFromEntityAndRemovesEntityFromParentChildren) {
  auto parent = entityDatabase.create();
  entityDatabase.set<quoll::Name>(parent, {"Parent 1"});

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity, {"Child"});
  entityDatabase.set<quoll::Parent>(entity, {parent});
  entityDatabase.set<quoll::Children>(parent, {{entity}});

  call(entity, "parentSetNil");

  EXPECT_FALSE(entityDatabase.has<quoll::Parent>(entity));
}