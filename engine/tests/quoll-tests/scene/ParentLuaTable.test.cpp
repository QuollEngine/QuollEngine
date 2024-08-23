#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/Parent.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class ParentLuaTableTest : public LuaScriptingInterfaceTestBase {};

// NOTE: Children are not asserted here because they are already tested

TEST_F(ParentLuaTableTest, GetReturnsNilIfEntityHasNoParent) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::Name>({"Child"});

  call(entity, "parentGetNil");
}

TEST_F(ParentLuaTableTest, GetReturnsParentOfEntityIfEntityHasParent) {
  auto parent = entityDatabase.entity();
  parent.set<quoll::Name>({"Parent"});
  auto entity = entityDatabase.entity();
  entity.set<quoll::Name>({"Entity"});
  entity.set<quoll::Parent>({parent});

  call(entity, "parentGet");
}

TEST_F(ParentLuaTableTest, SetAssignsParentToEntityAndChildToNewParent) {
  auto parent1 = entityDatabase.entity();
  parent1.set<quoll::Name>({"Parent 1"});

  auto parent2 = entityDatabase.entity();
  parent2.set<quoll::Name>({"Parent 2"});

  auto entity = entityDatabase.entity();
  entity.set<quoll::Name>({"Entity"});

  entity.set<quoll::Parent>({parent1});
  parent1.set<quoll::Children>({{entity}});

  auto entity2 = entityDatabase.entity();
  entity2.set<quoll::Parent>({parent2});
  parent2.set<quoll::Children>({{entity2}});

  call(entity, "parentSet");

  EXPECT_EQ(entity.get_ref<quoll::Parent>()->parent, parent2);
}

TEST_F(
    ParentLuaTableTest,
    SetAssignsParentToEntityAndCreatesChildrenComponentToParentIfParentHasNoChildren) {
  auto parent1 = entityDatabase.entity();
  parent1.set<quoll::Name>({"Parent 1"});

  auto parent2 = entityDatabase.entity();
  parent2.set<quoll::Name>({"Parent 2"});

  auto entity = entityDatabase.entity();
  entity.set<quoll::Name>({"Child"});

  entity.set<quoll::Parent>({parent1});
  parent1.set<quoll::Children>({{entity}});

  call(entity, "parentSet");

  EXPECT_EQ(entity.get_ref<quoll::Parent>()->parent, parent2);
}

TEST_F(ParentLuaTableTest,
       SetNilRemovesParentFromEntityAndRemovesEntityFromParentChildren) {
  auto parent = entityDatabase.entity();
  parent.set<quoll::Name>({"Parent 1"});

  auto entity = entityDatabase.entity();
  entity.set<quoll::Name>({"Child"});
  entity.set<quoll::Parent>({parent});
  parent.set<quoll::Children>({{entity}});

  call(entity, "parentSetNil");

  EXPECT_FALSE(entity.has<quoll::Parent>());
}