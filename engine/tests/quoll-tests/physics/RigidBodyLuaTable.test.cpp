#include "quoll/core/Base.h"
#include "quoll/physics/RigidBody.h"
#include "quoll/physics/RigidBodyClear.h"
#include "quoll/physics/Force.h"
#include "quoll/physics/Torque.h"
#include "quoll/physics/Impulse.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class RigidBodyLuaTableTest : public LuaScriptingInterfaceTestBase {};

TEST_F(RigidBodyLuaTableTest, SetDefaultParamsSetsNewRigidBody) {
  auto entity = entityDatabase.create();

  call(entity, "rigidBodySetDefaultParams");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.mass,
            quoll::RigidBody{}.dynamicDesc.mass);
}

TEST_F(RigidBodyLuaTableTest,
       GetRigidBodyTypeReturnsNilIfRigidBodyDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "rigidBodyGetType");
  EXPECT_TRUE(state["rbType"].is<sol::nil_t>());
}

TEST_F(RigidBodyLuaTableTest, GetRigidBodyTypeReturnsRigidBodyType) {
  auto entity = entityDatabase.create();
  quoll::RigidBody rigidBody{};

  {
    rigidBody.type = quoll::RigidBodyType::Dynamic;
    entityDatabase.set(entity, rigidBody);
    auto state = call(entity, "rigidBodyGetType");
    EXPECT_EQ(state["rbType"].get<u32>(),
              static_cast<u32>(quoll::RigidBodyType::Dynamic));
  }

  {
    rigidBody.type = quoll::RigidBodyType::Kinematic;
    entityDatabase.set(entity, rigidBody);

    auto state = call(entity, "rigidBodyGetType");
    EXPECT_EQ(state["rbType"].get<u32>(),
              static_cast<u32>(quoll::RigidBodyType::Kinematic));
  }
}

TEST_F(RigidBodyLuaTableTest, GetRigidBodyTypeEnumValues) {
  auto entity = entityDatabase.create();
  call(entity, "rigidBodyCheckTypeEnumValues");
}

TEST_F(RigidBodyLuaTableTest, GetMassReturnsNullIfRigidBodyDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "rigidBodyGetMass");
  EXPECT_TRUE(state["mass"].is<sol::nil_t>());
}

TEST_F(RigidBodyLuaTableTest, GetMassReturnsRigidBodyMass) {
  auto entity = entityDatabase.create();
  quoll::RigidBody rigidBody;
  rigidBody.dynamicDesc.mass = 2.5f;
  entityDatabase.set(entity, rigidBody);

  auto state = call(entity, "rigidBodyGetMass");
  EXPECT_EQ(state["mass"].get<f32>(), 2.5f);
}

TEST_F(RigidBodyLuaTableTest, SetMassCreatesRigidBodyIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "rigidBodySetMass");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.mass, 2.5);
}

TEST_F(RigidBodyLuaTableTest, SetMassUpdatesMassPropertyOfRigidBodyIfExists) {
  auto entity = entityDatabase.create();

  quoll::RigidBody rigidBody{};
  rigidBody.dynamicDesc.inertia = glm::vec3(7.5f);
  entityDatabase.set(entity, rigidBody);

  call(entity, "rigidBodySetMass");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.mass, 2.5);
  EXPECT_NE(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.mass,
            rigidBody.dynamicDesc.mass);
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.inertia,
            rigidBody.dynamicDesc.inertia);
}

TEST_F(RigidBodyLuaTableTest, GetInertiaReturnsNullIfRigidBodyDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "rigidBodyGetInertia");
  EXPECT_TRUE(state["rigidBodyInertia"].is<sol::nil_t>());
}

TEST_F(RigidBodyLuaTableTest, GetInertiaReturnsRigidBodyInertia) {
  auto entity = entityDatabase.create();
  quoll::RigidBody rigidBody;
  rigidBody.dynamicDesc.inertia = glm::vec3(2.5f, 3.5, 5.5f);
  entityDatabase.set(entity, rigidBody);

  auto state = call(entity, "rigidBodyGetInertia");
  EXPECT_EQ(state["rigidBodyInertia"].get<glm::vec3>(),
            glm::vec3(2.5f, 3.5, 5.5f));
}

TEST_F(RigidBodyLuaTableTest, SetInertiaCreatesRigidBodyIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "rigidBodySetInertia");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.inertia,
            glm::vec3(2.5f, 3.5f, 4.5f));
}

TEST_F(RigidBodyLuaTableTest,
       SetInertiaUpdatesInertiaPropertyOfRigidBodyIfExists) {
  auto entity = entityDatabase.create();

  quoll::RigidBody rigidBody{};
  rigidBody.dynamicDesc.mass = 7.5f;
  entityDatabase.set(entity, rigidBody);

  call(entity, "rigidBodySetInertia");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.mass,
            rigidBody.dynamicDesc.mass);
  EXPECT_NE(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.inertia,
            rigidBody.dynamicDesc.inertia);
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.inertia,
            glm::vec3(2.5f, 3.5f, 4.5f));
}

TEST_F(RigidBodyLuaTableTest, SetIndividualComponentsOfInertia) {
  auto entity = entityDatabase.create();

  quoll::RigidBody rigidBody{};
  rigidBody.dynamicDesc.mass = 7.5f;
  entityDatabase.set(entity, rigidBody);

  call(entity, "rigidBodySetInertiaIndividual");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.inertia,
            glm::vec3(2.5f, 3.5f, 4.5f));
}

TEST_F(RigidBodyLuaTableTest,
       IsGravityAppliedReturnsNullIfRigidBodyDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "rigidBodyIsGravityApplied");
  EXPECT_TRUE(state["isGravityApplied"].is<sol::nil_t>());
}

TEST_F(RigidBodyLuaTableTest, IsGravityAppliedReturnsRigidBodyGravityApplied) {
  auto entity = entityDatabase.create();
  quoll::RigidBody rigidBody;
  rigidBody.dynamicDesc.applyGravity = false;
  entityDatabase.set(entity, rigidBody);

  auto state = call(entity, "rigidBodyIsGravityApplied");
  EXPECT_EQ(state["isGravityApplied"].get<bool>(), false);
}

TEST_F(RigidBodyLuaTableTest, ApplyGravityCreatesRigidBodyIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "rigidBodyApplyGravity");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBody>(entity));
  EXPECT_EQ(
      entityDatabase.get<quoll::RigidBody>(entity).dynamicDesc.applyGravity,
      false);
}

TEST_F(RigidBodyLuaTableTest,
       ApplyGravityUpdatesInertiaPropertyOfRigidBodyIfExists) {
  auto entity = entityDatabase.create();

  quoll::RigidBody rigidBody{};
  rigidBody.dynamicDesc.mass = 7.5f;
  entityDatabase.set(entity, rigidBody);

  call(entity, "rigidBodyApplyGravity");
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

TEST_F(RigidBodyLuaTableTest, ApplyForceSetsForceComponent) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::Force>(entity));

  call(entity, "rigidBodyApplyForce");
  EXPECT_TRUE(entityDatabase.has<quoll::Force>(entity));

  const auto &force = entityDatabase.get<quoll::Force>(entity).force;

  EXPECT_EQ(force, glm::vec3(10.0f, 0.2f, 5.0f));
}

TEST_F(RigidBodyLuaTableTest, ApplyImpulseSetsImpulseComponent) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::Impulse>(entity));

  call(entity, "rigidBodyApplyImpulse");
  EXPECT_TRUE(entityDatabase.has<quoll::Impulse>(entity));

  const auto &impulse = entityDatabase.get<quoll::Impulse>(entity).impulse;

  EXPECT_EQ(impulse, glm::vec3(10.0f, 0.2f, 5.0f));
}

TEST_F(RigidBodyLuaTableTest, ApplyTorqueSetsTorqueComponent) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::Torque>(entity));

  call(entity, "rigidBodyApplyTorque");
  EXPECT_TRUE(entityDatabase.has<quoll::Torque>(entity));

  const auto &torque = entityDatabase.get<quoll::Torque>(entity).torque;

  EXPECT_EQ(torque, glm::vec3(2.5f, 3.5f, 1.2f));
}

TEST_F(RigidBodyLuaTableTest, ClearSetsRigidBodyClearComponent) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::RigidBodyClear>(entity));

  call(entity, "rigidBodyClear");
  EXPECT_TRUE(entityDatabase.has<quoll::RigidBodyClear>(entity));
}

TEST_F(RigidBodyLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "rigidBodyDelete");
  EXPECT_FALSE(entityDatabase.has<quoll::RigidBody>(entity));
}

TEST_F(RigidBodyLuaTableTest, DeleteRemovesRigidBodyComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::RigidBody>(entity, {});

  call(entity, "rigidBodyDelete");
  EXPECT_FALSE(entityDatabase.has<quoll::RigidBody>(entity));
}
