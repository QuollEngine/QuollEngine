#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"

#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class RigidBodyLuaScriptingInterfaceTest
    : public LuaScriptingInterfaceTestBase {};

TEST_F(RigidBodyLuaScriptingInterfaceTest, SetDefaultParamsSetsNewRigidBody) {
  auto entity = entityDatabase.create();

  call(entity, "rigid_body_set_default_params");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.mass,
            quoll::RigidBody{}.dynamicDesc.mass);
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       GetMassReturnsNullIfRigidBodyDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "rigid_body_get_mass");
  EXPECT_TRUE(state["mass"].is<sol::nil_t>());
}

TEST_F(RigidBodyLuaScriptingInterfaceTest, GetMassReturnsRigidBodyMass) {
  auto entity = entityDatabase.create();
  quoll::RigidBody rigidBody;
  rigidBody.dynamicDesc.mass = 2.5f;
  entityDatabase.set(entity, rigidBody);

  auto state = call(entity, "rigid_body_get_mass");
  EXPECT_EQ(state["mass"].get<float>(), 2.5f);
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       SetMassCreatesRigidBodyIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "rigid_body_set_mass");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.mass, 2.5);
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       SetMassUpdatesMassPropertyOfRigidBodyIfExists) {
  auto entity = entityDatabase.create();

  quoll::RigidBody rigidBody{};
  rigidBody.dynamicDesc.inertia = glm::vec3(7.5f);
  entityDatabase.set(entity, rigidBody);

  call(entity, "rigid_body_set_mass");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.mass, 2.5);
  EXPECT_NE(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.mass,
            rigidBody.dynamicDesc.mass);
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.inertia,
            rigidBody.dynamicDesc.inertia);
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       GetInertiaReturnsNullIfRigidBodyDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "rigid_body_get_inertia");
  EXPECT_TRUE(state["inertia_x"].is<sol::nil_t>());
  EXPECT_TRUE(state["inertia_y"].is<sol::nil_t>());
  EXPECT_TRUE(state["inertia_z"].is<sol::nil_t>());
}

TEST_F(RigidBodyLuaScriptingInterfaceTest, GetInertiaReturnsRigidBodyInertia) {
  auto entity = entityDatabase.create();
  quoll::RigidBody rigidBody;
  rigidBody.dynamicDesc.inertia = glm::vec3(2.5f);
  entityDatabase.set(entity, rigidBody);

  auto state = call(entity, "rigid_body_get_inertia");
  EXPECT_EQ(state["inertia_x"].get<float>(), 2.5f);
  EXPECT_EQ(state["inertia_y"].get<float>(), 2.5f);
  EXPECT_EQ(state["inertia_z"].get<float>(), 2.5f);
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       SetInertiaCreatesRigidBodyIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "rigid_body_set_inertia");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.inertia,
            glm::vec3(2.5));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       SetInertiaUpdatesInertiaPropertyOfRigidBodyIfExists) {
  auto entity = entityDatabase.create();

  quoll::RigidBody rigidBody{};
  rigidBody.dynamicDesc.mass = 7.5f;
  entityDatabase.set(entity, rigidBody);

  call(entity, "rigid_body_set_inertia");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.mass,
            rigidBody.dynamicDesc.mass);
  EXPECT_NE(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.inertia,
            rigidBody.dynamicDesc.inertia);
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.inertia,
            glm::vec3{2.5f});
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       IsGravityAppliedReturnsNullIfRigidBodyDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "rigid_body_is_gravity_applied");
  EXPECT_TRUE(state["is_gravity_applied"].is<sol::nil_t>());
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       IsGravityAppliedReturnsRigidBodyGravityApplied) {
  auto entity = entityDatabase.create();
  quoll::RigidBody rigidBody;
  rigidBody.dynamicDesc.applyGravity = false;
  entityDatabase.set(entity, rigidBody);

  auto state = call(entity, "rigid_body_is_gravity_applied");
  EXPECT_EQ(state["is_gravity_applied"].get<bool>(), false);
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       ApplyGravityCreatesRigidBodyIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "rigid_body_apply_gravity");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(
      entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.applyGravity,
      false);
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       ApplyGravityUpdatesInertiaPropertyOfRigidBodyIfExists) {
  auto entity = entityDatabase.create();

  quoll::RigidBody rigidBody{};
  rigidBody.dynamicDesc.mass = 7.5f;
  entityDatabase.set(entity, rigidBody);

  call(entity, "rigid_body_apply_gravity");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.mass,
            rigidBody.dynamicDesc.mass);
  EXPECT_NE(
      entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.applyGravity,
      rigidBody.dynamicDesc.applyGravity);
  EXPECT_EQ(
      entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.applyGravity,
      false);
}

TEST_F(RigidBodyLuaScriptingInterfaceTest, ApplyForceSetsForceComponent) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::Force>(entity));

  call(entity, "rigid_body_apply_force");
  EXPECT_TRUE(entityDatabase.has<quoll::Force>(entity));

  const auto &force = entityDatabase.get<quoll::Force>(entity).force;

  EXPECT_EQ(force, glm::vec3(10.0f, 0.2f, 5.0f));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest, ApplyTorqueSetsTorqueComponent) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::Torque>(entity));

  call(entity, "rigid_body_apply_torque");
  EXPECT_TRUE(entityDatabase.has<quoll::Torque>(entity));

  const auto &torque = entityDatabase.get<quoll::Torque>(entity).torque;

  EXPECT_EQ(torque, glm::vec3(2.5f, 3.5f, 1.2f));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest, ClearSetsRigidBodyClearComponent) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::RigidBodyClear>(entity));

  call(entity, "rigid_body_clear");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBodyClear>(entity));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "rigid_body_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::RigidBody>(entity));
}

TEST_F(RigidBodyLuaScriptingInterfaceTest,
       DeleteRemovesRigidBodyComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::RigidBody>(entity, {});

  call(entity, "rigid_body_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::RigidBody>(entity));
}
