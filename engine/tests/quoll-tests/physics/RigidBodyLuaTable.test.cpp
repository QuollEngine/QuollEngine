#include "quoll/core/Base.h"
#include "quoll/physics/Force.h"
#include "quoll/physics/Impulse.h"
#include "quoll/physics/RigidBody.h"
#include "quoll/physics/RigidBodyClear.h"
#include "quoll/physics/Torque.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class RigidBodyLuaTableTest : public LuaScriptingInterfaceTestBase {};

TEST_F(RigidBodyLuaTableTest, SetDefaultParamsSetsNewRigidBody) {
  auto entity = entityDatabase.entity();

  call(entity, "rigidBodySetDefaultParams");
  EXPECT_TRUE(entity.has<quoll::RigidBody>());
  EXPECT_EQ(entity.get_ref<quoll::RigidBody>()->dynamicDesc.mass,
            quoll::RigidBody{}.dynamicDesc.mass);
}

TEST_F(RigidBodyLuaTableTest,
       GetRigidBodyTypeReturnsNilIfRigidBodyDoesNotExist) {
  auto entity = entityDatabase.entity();

  auto state = call(entity, "rigidBodyGetType");
  EXPECT_TRUE(state["rbType"].is<sol::nil_t>());
}

TEST_F(RigidBodyLuaTableTest, GetRigidBodyTypeReturnsRigidBodyType) {
  auto entity = entityDatabase.entity();
  quoll::RigidBody rigidBody{};

  {
    rigidBody.type = quoll::RigidBodyType::Dynamic;
    entity.set(rigidBody);
    auto state = call(entity, "rigidBodyGetType");
    EXPECT_EQ(state["rbType"].get<u32>(),
              static_cast<u32>(quoll::RigidBodyType::Dynamic));
  }

  {
    rigidBody.type = quoll::RigidBodyType::Kinematic;
    entity.set(rigidBody);

    auto state = call(entity, "rigidBodyGetType");
    EXPECT_EQ(state["rbType"].get<u32>(),
              static_cast<u32>(quoll::RigidBodyType::Kinematic));
  }
}

TEST_F(RigidBodyLuaTableTest, GetRigidBodyTypeEnumValues) {
  auto entity = entityDatabase.entity();
  call(entity, "rigidBodyCheckTypeEnumValues");
}

TEST_F(RigidBodyLuaTableTest, GetMassReturnsNullIfRigidBodyDoesNotExist) {
  auto entity = entityDatabase.entity();

  auto state = call(entity, "rigidBodyGetMass");
  EXPECT_TRUE(state["mass"].is<sol::nil_t>());
}

TEST_F(RigidBodyLuaTableTest, GetMassReturnsRigidBodyMass) {
  auto entity = entityDatabase.entity();
  quoll::RigidBody rigidBody;
  rigidBody.dynamicDesc.mass = 2.5f;
  entity.set(rigidBody);

  auto state = call(entity, "rigidBodyGetMass");
  EXPECT_EQ(state["mass"].get<f32>(), 2.5f);
}

TEST_F(RigidBodyLuaTableTest, SetMassCreatesRigidBodyIfItDoesNotExist) {
  auto entity = entityDatabase.entity();

  call(entity, "rigidBodySetMass");
  EXPECT_TRUE(entity.has<quoll::RigidBody>());
  EXPECT_EQ(entity.get_ref<quoll::RigidBody>()->dynamicDesc.mass, 2.5);
}

TEST_F(RigidBodyLuaTableTest, SetMassUpdatesMassPropertyOfRigidBodyIfExists) {
  auto entity = entityDatabase.entity();

  quoll::RigidBody rigidBody{};
  rigidBody.dynamicDesc.inertia = glm::vec3(7.5f);
  entity.set(rigidBody);

  call(entity, "rigidBodySetMass");
  EXPECT_TRUE(entity.has<quoll::RigidBody>());
  EXPECT_EQ(entity.get_ref<quoll::RigidBody>()->dynamicDesc.mass, 2.5);
  EXPECT_NE(entity.get_ref<quoll::RigidBody>()->dynamicDesc.mass,
            rigidBody.dynamicDesc.mass);
  EXPECT_EQ(entity.get_ref<quoll::RigidBody>()->dynamicDesc.inertia,
            rigidBody.dynamicDesc.inertia);
}

TEST_F(RigidBodyLuaTableTest, GetInertiaReturnsNullIfRigidBodyDoesNotExist) {
  auto entity = entityDatabase.entity();

  auto state = call(entity, "rigidBodyGetInertia");
  EXPECT_TRUE(state["rigidBodyInertia"].is<sol::nil_t>());
}

TEST_F(RigidBodyLuaTableTest, GetInertiaReturnsRigidBodyInertia) {
  auto entity = entityDatabase.entity();
  quoll::RigidBody rigidBody;
  rigidBody.dynamicDesc.inertia = glm::vec3(2.5f, 3.5, 5.5f);
  entity.set(rigidBody);

  auto state = call(entity, "rigidBodyGetInertia");
  EXPECT_EQ(state["rigidBodyInertia"].get<glm::vec3>(),
            glm::vec3(2.5f, 3.5, 5.5f));
}

TEST_F(RigidBodyLuaTableTest, SetInertiaCreatesRigidBodyIfItDoesNotExist) {
  auto entity = entityDatabase.entity();

  call(entity, "rigidBodySetInertia");
  EXPECT_TRUE(entity.has<quoll::RigidBody>());
  EXPECT_EQ(entity.get_ref<quoll::RigidBody>()->dynamicDesc.inertia,
            glm::vec3(2.5f, 3.5f, 4.5f));
}

TEST_F(RigidBodyLuaTableTest,
       SetInertiaUpdatesInertiaPropertyOfRigidBodyIfExists) {
  auto entity = entityDatabase.entity();

  quoll::RigidBody rigidBody{};
  rigidBody.dynamicDesc.mass = 7.5f;
  entity.set(rigidBody);

  call(entity, "rigidBodySetInertia");
  EXPECT_TRUE(entity.has<quoll::RigidBody>());
  EXPECT_EQ(entity.get_ref<quoll::RigidBody>()->dynamicDesc.mass,
            rigidBody.dynamicDesc.mass);
  EXPECT_NE(entity.get_ref<quoll::RigidBody>()->dynamicDesc.inertia,
            rigidBody.dynamicDesc.inertia);
  EXPECT_EQ(entity.get_ref<quoll::RigidBody>()->dynamicDesc.inertia,
            glm::vec3(2.5f, 3.5f, 4.5f));
}

TEST_F(RigidBodyLuaTableTest, SetIndividualComponentsOfInertia) {
  auto entity = entityDatabase.entity();

  quoll::RigidBody rigidBody{};
  rigidBody.dynamicDesc.mass = 7.5f;
  entity.set(rigidBody);

  call(entity, "rigidBodySetInertiaIndividual");
  EXPECT_TRUE(entity.has<quoll::RigidBody>());
  EXPECT_EQ(entity.get_ref<quoll::RigidBody>()->dynamicDesc.inertia,
            glm::vec3(2.5f, 3.5f, 4.5f));
}

TEST_F(RigidBodyLuaTableTest,
       IsGravityAppliedReturnsNullIfRigidBodyDoesNotExist) {
  auto entity = entityDatabase.entity();

  auto state = call(entity, "rigidBodyIsGravityApplied");
  EXPECT_TRUE(state["isGravityApplied"].is<sol::nil_t>());
}

TEST_F(RigidBodyLuaTableTest, IsGravityAppliedReturnsRigidBodyGravityApplied) {
  auto entity = entityDatabase.entity();
  quoll::RigidBody rigidBody;
  rigidBody.dynamicDesc.applyGravity = false;
  entity.set(rigidBody);

  auto state = call(entity, "rigidBodyIsGravityApplied");
  EXPECT_EQ(state["isGravityApplied"].get<bool>(), false);
}

TEST_F(RigidBodyLuaTableTest, ApplyGravityCreatesRigidBodyIfItDoesNotExist) {
  auto entity = entityDatabase.entity();

  call(entity, "rigidBodyApplyGravity");
  EXPECT_TRUE(entity.has<quoll::RigidBody>());
  EXPECT_EQ(entity.get_ref<quoll::RigidBody>()->dynamicDesc.applyGravity,
            false);
}

TEST_F(RigidBodyLuaTableTest,
       ApplyGravityUpdatesInertiaPropertyOfRigidBodyIfExists) {
  auto entity = entityDatabase.entity();

  quoll::RigidBody rigidBody{};
  rigidBody.dynamicDesc.mass = 7.5f;
  entity.set(rigidBody);

  call(entity, "rigidBodyApplyGravity");
  EXPECT_TRUE(entity.has<quoll::RigidBody>());
  EXPECT_EQ(entity.get_ref<quoll::RigidBody>()->dynamicDesc.mass,
            rigidBody.dynamicDesc.mass);
  EXPECT_NE(entity.get_ref<quoll::RigidBody>()->dynamicDesc.applyGravity,
            rigidBody.dynamicDesc.applyGravity);
  EXPECT_EQ(entity.get_ref<quoll::RigidBody>()->dynamicDesc.applyGravity,
            false);
}

TEST_F(RigidBodyLuaTableTest, ApplyForceSetsForceComponent) {
  auto entity = entityDatabase.entity();
  EXPECT_FALSE(entity.has<quoll::Force>());

  call(entity, "rigidBodyApplyForce");
  EXPECT_TRUE(entity.has<quoll::Force>());

  const auto &force = entity.get_ref<quoll::Force>()->force;

  EXPECT_EQ(force, glm::vec3(10.0f, 0.2f, 5.0f));
}

TEST_F(RigidBodyLuaTableTest, ApplyImpulseSetsImpulseComponent) {
  auto entity = entityDatabase.entity();
  EXPECT_FALSE(entity.has<quoll::Impulse>());

  call(entity, "rigidBodyApplyImpulse");
  EXPECT_TRUE(entity.has<quoll::Impulse>());

  const auto &impulse = entity.get_ref<quoll::Impulse>()->impulse;

  EXPECT_EQ(impulse, glm::vec3(10.0f, 0.2f, 5.0f));
}

TEST_F(RigidBodyLuaTableTest, ApplyTorqueSetsTorqueComponent) {
  auto entity = entityDatabase.entity();
  EXPECT_FALSE(entity.has<quoll::Torque>());

  call(entity, "rigidBodyApplyTorque");
  EXPECT_TRUE(entity.has<quoll::Torque>());

  const auto &torque = entity.get_ref<quoll::Torque>()->torque;

  EXPECT_EQ(torque, glm::vec3(2.5f, 3.5f, 1.2f));
}

TEST_F(RigidBodyLuaTableTest, ClearSetsRigidBodyClearComponent) {
  auto entity = entityDatabase.entity();
  EXPECT_FALSE(entity.has<quoll::RigidBodyClear>());

  call(entity, "rigidBodyClear");
  EXPECT_TRUE(entity.has<quoll::RigidBodyClear>());
}

TEST_F(RigidBodyLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.entity();

  call(entity, "rigidBodyDelete");
  EXPECT_FALSE(entity.has<quoll::RigidBody>());
}

TEST_F(RigidBodyLuaTableTest, DeleteRemovesRigidBodyComponentFromEntity) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::RigidBody>({});

  call(entity, "rigidBodyDelete");
  EXPECT_FALSE(entity.has<quoll::RigidBody>());
}
