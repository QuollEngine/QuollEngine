#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid-tests/test-utils/ScriptingInterfaceTestBase.h"

class RigidBodyLuaScriptingInterfaceTest
    : public LuaScriptingInterfaceTestBase {};

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       ApplyForceDoesNothingIfProvidedArgumentsAreInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<liquid::ForceComponent>(entity));
  call(entity, "rigid_body_apply_force_invalid");
  EXPECT_FALSE(entityDatabase.has<liquid::ForceComponent>(entity));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest, ApplyForceSetsForceComponent) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<liquid::ForceComponent>(entity));

  call(entity, "rigid_body_apply_force");
  EXPECT_TRUE(entityDatabase.has<liquid::ForceComponent>(entity));

  const auto &force = entityDatabase.get<liquid::ForceComponent>(entity).force;

  EXPECT_EQ(force, glm::vec3(10.0f, 0.2f, 5.0f));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       ApplyTorqueDoesNothingIfProvidedArgumentsAreInvalid) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<liquid::TorqueComponent>(entity));

  call(entity, "rigid_body_apply_torque_invalid");
  EXPECT_FALSE(entityDatabase.has<liquid::TorqueComponent>(entity));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest, ApplyTorqueSetsTorqueComponent) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<liquid::TorqueComponent>(entity));

  call(entity, "rigid_body_apply_torque");
  EXPECT_TRUE(entityDatabase.has<liquid::TorqueComponent>(entity));

  const auto &torque =
      entityDatabase.get<liquid::TorqueComponent>(entity).torque;

  EXPECT_EQ(torque, glm::vec3(2.5f, 3.5f, 1.2f));
}
