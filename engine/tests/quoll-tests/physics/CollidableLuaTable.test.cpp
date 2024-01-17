#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/physics/Collidable.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class CollidableLuaTableTest : public LuaScriptingInterfaceTestBase {};

TEST_F(CollidableLuaTableTest,
       SetDefaultMaterialCreatesCollidableIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
  call(entity, "collidableSetDefaultMaterial");
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaTableTest,
       SetDefaultMaterialUpdatesExistingCollidableMaterialIfExists) {
  auto entity = entityDatabase.create();

  quoll::Collidable collidable{};
  collidable.materialDesc.restitution = 2.5f;
  collidable.geometryDesc.type = quoll::PhysicsGeometryType::Sphere;
  entityDatabase.set(entity, collidable);

  call(entity, "collidableSetDefaultMaterial");

  const auto &updatedCollidable = entityDatabase.get<quoll::Collidable>(entity);
  EXPECT_EQ(updatedCollidable.geometryDesc.type, collidable.geometryDesc.type);
  EXPECT_NE(updatedCollidable.materialDesc.restitution,
            collidable.materialDesc.restitution);
  EXPECT_EQ(updatedCollidable.materialDesc.restitution,
            quoll::Collidable{}.materialDesc.restitution);
}

// Get static friction
TEST_F(CollidableLuaTableTest,
       GetStaticFrictionReturnsNullIfCollidableDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "collidableGetStaticFriction");
  EXPECT_TRUE(state["staticFriction"].is<sol::nil_t>());
}

TEST_F(CollidableLuaTableTest,
       GetStaticFrictionReturnsCollidableStaticFriction) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.staticFriction = 2.5f;
  entityDatabase.set(entity, collidable);

  auto state = call(entity, "collidableGetStaticFriction");
  EXPECT_EQ(state["staticFriction"].get<f32>(), 2.5f);
}

// Set static friction
TEST_F(CollidableLuaTableTest,
       SetStaticFrictionCreatesCollidableIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidableSetStaticFriction");
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  EXPECT_EQ(
      entityDatabase.get<quoll::Collidable>(entity).materialDesc.staticFriction,
      2.5);
}

TEST_F(CollidableLuaTableTest,
       SetStaticFrictionUpdatesExistingCollidableIfExists) {
  auto entity = entityDatabase.create();

  quoll::Collidable collidable{};
  collidable.materialDesc.restitution = 2.5f;
  entityDatabase.set(entity, collidable);

  call(entity, "collidableSetStaticFriction");

  const auto &updatedCollidable = entityDatabase.get<quoll::Collidable>(entity);
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  EXPECT_EQ(updatedCollidable.materialDesc.staticFriction, 2.5);
  EXPECT_NE(updatedCollidable.materialDesc.staticFriction,
            collidable.materialDesc.staticFriction);
  EXPECT_EQ(updatedCollidable.materialDesc.restitution,
            collidable.materialDesc.restitution);
}

// Get dynamic friction
TEST_F(CollidableLuaTableTest,
       GetDynamicFrictionReturnsNullIfCollidableDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "collidableGetDynamicFriction");
  EXPECT_TRUE(state["dynamicFriction"].is<sol::nil_t>());
}

TEST_F(CollidableLuaTableTest,
       GetDynamicFrictionReturnsCollidableDynamicFriction) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.dynamicFriction = 2.5f;
  entityDatabase.set(entity, collidable);

  auto state = call(entity, "collidableGetDynamicFriction");
  EXPECT_EQ(state["dynamicFriction"].get<f32>(), 2.5f);
}

// Set dynamic friction
TEST_F(CollidableLuaTableTest,
       SetDynamicFrictionCreatesCollidableIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidableSetDynamicFriction");
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::Collidable>(entity)
                .materialDesc.dynamicFriction,
            2.5);
}

TEST_F(CollidableLuaTableTest,
       SetDynamicFrictionUpdatesExistingCollidableIfExists) {
  auto entity = entityDatabase.create();

  quoll::Collidable collidable{};
  collidable.materialDesc.restitution = 2.5f;
  entityDatabase.set(entity, collidable);

  call(entity, "collidableSetDynamicFriction");

  const auto &updatedCollidable = entityDatabase.get<quoll::Collidable>(entity);
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  EXPECT_EQ(updatedCollidable.materialDesc.dynamicFriction, 2.5);
  EXPECT_NE(updatedCollidable.materialDesc.dynamicFriction,
            collidable.materialDesc.dynamicFriction);
  EXPECT_EQ(updatedCollidable.materialDesc.restitution,
            collidable.materialDesc.restitution);
}

// Get restitution friction
TEST_F(CollidableLuaTableTest,
       GetRestitutionReturnsNullIfCollidableDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "collidableGetRestitution");
  EXPECT_TRUE(state["restitution"].is<sol::nil_t>());
}

TEST_F(CollidableLuaTableTest, GetRestitutionReturnsCollidableRestitution) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.restitution = 2.5f;
  entityDatabase.set(entity, collidable);

  auto state = call(entity, "collidableGetRestitution");
  EXPECT_EQ(state["restitution"].get<f32>(), 2.5f);
}

// Set restitution
TEST_F(CollidableLuaTableTest,
       SetRestitutionCreatesCollidableIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidableSetRestitution");
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  EXPECT_EQ(
      entityDatabase.get<quoll::Collidable>(entity).materialDesc.restitution,
      2.5);
}

TEST_F(CollidableLuaTableTest,
       SetBoxGeometryCreatesCollidableIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidableSetBoxGeometry");

  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::Collidable>(entity).geometryDesc.type,
            quoll::PhysicsGeometryType::Box);
  EXPECT_EQ(
      std::get<quoll::PhysicsGeometryBox>(
          entityDatabase.get<quoll::Collidable>(entity).geometryDesc.params)
          .halfExtents,
      glm::vec3{2.5f});
}

TEST_F(CollidableLuaTableTest,
       SetBoxGeometryUpdatesExistingCollidableShapeToBoxIfExists) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.restitution = 2.5f;
  collidable.geometryDesc.type = quoll::PhysicsGeometryType::Plane;
  entityDatabase.set(entity, collidable);

  call(entity, "collidableSetBoxGeometry");

  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  auto &updatedCollidable = entityDatabase.get<quoll::Collidable>(entity);
  EXPECT_EQ(updatedCollidable.geometryDesc.type,
            quoll::PhysicsGeometryType::Box);
  EXPECT_EQ(
      std::get<quoll::PhysicsGeometryBox>(updatedCollidable.geometryDesc.params)
          .halfExtents,
      glm::vec3{2.5f});
  EXPECT_EQ(updatedCollidable.materialDesc.restitution,
            collidable.materialDesc.restitution);
}

// Set sphere geometry
TEST_F(CollidableLuaTableTest,
       SetSphereGeometryCreatesCollidableIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidableSetSphereGeometry");

  EXPECT_EQ(entityDatabase.get<quoll::Collidable>(entity).geometryDesc.type,
            quoll::PhysicsGeometryType::Sphere);
  EXPECT_EQ(
      std::get<quoll::PhysicsGeometrySphere>(
          entityDatabase.get<quoll::Collidable>(entity).geometryDesc.params)
          .radius,
      2.5f);
}

TEST_F(CollidableLuaTableTest,
       SetSphereGeometryUpdatesExistingCollidableShapeToSphereIfExists) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.restitution = 2.5f;
  collidable.geometryDesc.type = quoll::PhysicsGeometryType::Plane;
  entityDatabase.set(entity, collidable);

  call(entity, "collidableSetSphereGeometry");

  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  auto &updatedCollidable = entityDatabase.get<quoll::Collidable>(entity);
  EXPECT_EQ(updatedCollidable.geometryDesc.type,
            quoll::PhysicsGeometryType::Sphere);
  EXPECT_EQ(std::get<quoll::PhysicsGeometrySphere>(
                updatedCollidable.geometryDesc.params)
                .radius,
            2.5f);
  EXPECT_EQ(updatedCollidable.materialDesc.restitution,
            collidable.materialDesc.restitution);
}

// Set capsule geometry
TEST_F(CollidableLuaTableTest,
       SetCapsuleGeometryCreatesCollidableIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidableSetCapsuleGeometry");

  EXPECT_EQ(entityDatabase.get<quoll::Collidable>(entity).geometryDesc.type,
            quoll::PhysicsGeometryType::Capsule);
  EXPECT_EQ(
      std::get<quoll::PhysicsGeometryCapsule>(
          entityDatabase.get<quoll::Collidable>(entity).geometryDesc.params)
          .radius,
      2.5f);
  EXPECT_EQ(
      std::get<quoll::PhysicsGeometryCapsule>(
          entityDatabase.get<quoll::Collidable>(entity).geometryDesc.params)
          .halfHeight,
      3.5f);
}

TEST_F(CollidableLuaTableTest,
       SetCapsuleGeometryUpdatesExistingCollidableShapeToCapsuleIfExists) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.restitution = 2.5f;
  collidable.geometryDesc.type = quoll::PhysicsGeometryType::Plane;
  entityDatabase.set(entity, collidable);

  call(entity, "collidableSetCapsuleGeometry");

  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  auto &updatedCollidable = entityDatabase.get<quoll::Collidable>(entity);

  EXPECT_EQ(updatedCollidable.geometryDesc.type,
            quoll::PhysicsGeometryType::Capsule);
  EXPECT_EQ(std::get<quoll::PhysicsGeometryCapsule>(
                updatedCollidable.geometryDesc.params)
                .radius,
            2.5f);
  EXPECT_EQ(std::get<quoll::PhysicsGeometryCapsule>(
                updatedCollidable.geometryDesc.params)
                .halfHeight,
            3.5f);
  EXPECT_EQ(updatedCollidable.materialDesc.restitution,
            collidable.materialDesc.restitution);
}

// Set plane geometry
TEST_F(CollidableLuaTableTest,
       SetPlaneGeometryCreatesCollidableIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidableSetPlaneGeometry");

  EXPECT_EQ(entityDatabase.get<quoll::Collidable>(entity).geometryDesc.type,
            quoll::PhysicsGeometryType::Plane);
  EXPECT_TRUE(std::holds_alternative<quoll::PhysicsGeometryPlane>(
      entityDatabase.get<quoll::Collidable>(entity).geometryDesc.params));
}

TEST_F(CollidableLuaTableTest,
       SetPlaneGeometryUpdatesExistingCollidableShapeToPlaneIfExists) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.restitution = 2.5f;
  collidable.geometryDesc.type = quoll::PhysicsGeometryType::Box;
  entityDatabase.set(entity, collidable);

  call(entity, "collidableSetPlaneGeometry");

  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  auto &updatedCollidable = entityDatabase.get<quoll::Collidable>(entity);

  EXPECT_EQ(updatedCollidable.geometryDesc.type,
            quoll::PhysicsGeometryType::Plane);
  EXPECT_TRUE(std::holds_alternative<quoll::PhysicsGeometryPlane>(
      updatedCollidable.geometryDesc.params));
  EXPECT_EQ(updatedCollidable.materialDesc.restitution,
            collidable.materialDesc.restitution);
}

// sweep
TEST_F(CollidableLuaTableTest, SweepReturnsFalseIfNoCollidable) {
  physicsBackend->setSweepValue(false);

  auto entity = entityDatabase.create();
  auto state = call(entity, "collidableSweep");
  EXPECT_TRUE(state["sweepOutput"].is<bool>());
  EXPECT_FALSE(state["sweepOutput"].get<bool>());
  EXPECT_TRUE(state["sweepData"].is<sol::nil_t>());
}

TEST_F(CollidableLuaTableTest, SweepReturnsFalseIfSweepTestFails) {
  physicsBackend->setSweepValue(false);

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Collidable>(entity, {});
  auto state = call(entity, "collidableSweep");
  EXPECT_TRUE(state["sweepOutput"].is<bool>());
  EXPECT_FALSE(state["sweepOutput"].get<bool>());
  EXPECT_TRUE(state["sweepData"].is<sol::nil_t>());
}

TEST_F(CollidableLuaTableTest, SweepReturnsTrueIfSweepTestSucceeds) {
  auto entity2 = entityDatabase.create();
  entityDatabase.set<quoll::Name>(entity2, {"Other entity"});

  physicsBackend->setSweepValue(true);
  physicsBackend->setSweepHitData({.normal = glm::vec3{2.5f, 3.5f, 4.5f},
                                   .distance = 0.5f,
                                   .entity = entity2});

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Collidable>(entity, {});

  auto state = call(entity, "collidableSweep");
  EXPECT_TRUE(state["sweepOutput"].is<bool>());
  EXPECT_TRUE(state["sweepOutput"].get<bool>());

  EXPECT_FALSE(state["sweepNormal"].is<sol::nil_t>());
  EXPECT_FALSE(state["sweepDistance"].is<sol::nil_t>());
  EXPECT_FALSE(state["sweepEntityName"].is<sol::nil_t>());

  auto normal = state["sweepNormal"].get<glm::vec3>();
  EXPECT_EQ(normal, glm::vec3(2.5f, 3.5f, 4.5f));

  auto distance = state["sweepDistance"].get<f32>();
  EXPECT_EQ(distance, 0.5f);

  auto entityName = state["sweepEntityName"].get<quoll::String>();
  EXPECT_EQ(entityName, "Other entity");
}

// Delete
TEST_F(CollidableLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidableDelete");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaTableTest, DeleteRemovesCollidableComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Collidable>(entity, {});

  call(entity, "collidableDelete");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}
