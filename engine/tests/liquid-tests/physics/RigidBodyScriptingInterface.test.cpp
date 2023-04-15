#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid-tests/test-utils/ScriptingInterfaceTestBase.h"

class RigidBodyLuaScriptingInterfaceTest
    : public LuaScriptingInterfaceTestBase {};

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       ApplyForceDoesNothingIfProvidedArgumentsAreInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<liquid::Force>(entity));
  call(entity, "rigid_body_apply_force_invalid");
  EXPECT_FALSE(entityDatabase.has<liquid::Force>(entity));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest, ApplyForceSetsForceComponent) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<liquid::Force>(entity));

  call(entity, "rigid_body_apply_force");
  EXPECT_TRUE(entityDatabase.has<liquid::Force>(entity));

  const auto &force = entityDatabase.get<liquid::Force>(entity).force;

  EXPECT_EQ(force, glm::vec3(10.0f, 0.2f, 5.0f));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       ApplyTorqueDoesNothingIfProvidedArgumentsAreInvalid) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<liquid::Torque>(entity));

  call(entity, "rigid_body_apply_torque_invalid");
  EXPECT_FALSE(entityDatabase.has<liquid::Torque>(entity));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest, ApplyTorqueSetsTorqueComponent) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<liquid::Torque>(entity));

  call(entity, "rigid_body_apply_torque");
  EXPECT_TRUE(entityDatabase.has<liquid::Torque>(entity));

  const auto &torque = entityDatabase.get<liquid::Torque>(entity).torque;

  EXPECT_EQ(torque, glm::vec3(2.5f, 3.5f, 1.2f));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       ClearDoesNothingIfProvidedArgumentsAreInvalid) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<liquid::RigidBodyClear>(entity));

  call(entity, "rigid_body_clear_invalid");
  EXPECT_FALSE(entityDatabase.has<liquid::RigidBodyClear>(entity));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest, ClearSetsRigidBodyClearComponent) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<liquid::RigidBodyClear>(entity));

  call(entity, "rigid_body_clear");
  EXPECT_TRUE(entityDatabase.has<liquid::RigidBodyClear>(entity));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       DeleteDoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::RigidBody>(entity, {});

  call(entity, "rigid_body_delete_invalid");
  EXPECT_TRUE(entityDatabase.has<liquid::RigidBody>(entity));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "rigid_body_delete");
  EXPECT_FALSE(entityDatabase.has<liquid::RigidBody>(entity));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       DeleteRemovesRigidBodyComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::RigidBody>(entity, {});

  call(entity, "rigid_body_delete");
  EXPECT_FALSE(entityDatabase.has<liquid::RigidBody>(entity));
}
