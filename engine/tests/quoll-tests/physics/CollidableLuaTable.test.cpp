#include "quoll/core/Base.h"
#include "quoll/physics/Collidable.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class CollidableLuaTableTest : public LuaScriptingInterfaceTestBase {};

TEST_F(CollidableLuaTableTest,
       SetDefaultMaterialCreatesCollidableIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
  call(entity, "collidable_set_default_material");
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaTableTest,
       SetDefaultMaterialUpdatesExistingCollidableMaterialIfExists) {
  auto entity = entityDatabase.create();

  quoll::Collidable collidable{};
  collidable.materialDesc.restitution = 2.5f;
  collidable.geometryDesc.type = quoll::PhysicsGeometryType::Sphere;
  entityDatabase.set(entity, collidable);

  call(entity, "collidable_set_default_material");

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

  auto state = call(entity, "collidable_get_static_friction");
  EXPECT_TRUE(state["static_friction"].is<sol::nil_t>());
}

TEST_F(CollidableLuaTableTest,
       GetStaticFrictionReturnsCollidableStaticFriction) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.staticFriction = 2.5f;
  entityDatabase.set(entity, collidable);

  auto state = call(entity, "collidable_get_static_friction");
  EXPECT_EQ(state["static_friction"].get<f32>(), 2.5f);
}

// Set static friction
TEST_F(CollidableLuaTableTest,
       SetStaticFrictionCreatesCollidableIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidable_set_static_friction");
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

  call(entity, "collidable_set_static_friction");

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

  auto state = call(entity, "collidable_get_dynamic_friction");
  EXPECT_TRUE(state["dynamic_friction"].is<sol::nil_t>());
}

TEST_F(CollidableLuaTableTest,
       GetDynamicFrictionReturnsCollidableDynamicFriction) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.dynamicFriction = 2.5f;
  entityDatabase.set(entity, collidable);

  auto state = call(entity, "collidable_get_dynamic_friction");
  EXPECT_EQ(state["dynamic_friction"].get<f32>(), 2.5f);
}

// Set dynamic friction
TEST_F(CollidableLuaTableTest,
       SetDynamicFrictionCreatesCollidableIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidable_set_dynamic_friction");
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

  call(entity, "collidable_set_dynamic_friction");

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

  auto state = call(entity, "collidable_get_restitution");
  EXPECT_TRUE(state["restitution"].is<sol::nil_t>());
}

TEST_F(CollidableLuaTableTest, GetRestitutionReturnsCollidableRestitution) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.restitution = 2.5f;
  entityDatabase.set(entity, collidable);

  auto state = call(entity, "collidable_get_restitution");
  EXPECT_EQ(state["restitution"].get<f32>(), 2.5f);
}

// Set restitution
TEST_F(CollidableLuaTableTest,
       SetRestitutionCreatesCollidableIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidable_set_restitution");
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  EXPECT_EQ(
      entityDatabase.get<quoll::Collidable>(entity).materialDesc.restitution,
      2.5);
}

TEST_F(CollidableLuaTableTest,
       SetBoxGeometryCreatesCollidableIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidable_set_box_geometry");

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

  call(entity, "collidable_set_box_geometry");

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

  call(entity, "collidable_set_sphere_geometry");

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

  call(entity, "collidable_set_sphere_geometry");

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

  call(entity, "collidable_set_capsule_geometry");

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

  call(entity, "collidable_set_capsule_geometry");

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

  call(entity, "collidable_set_plane_geometry");

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

  call(entity, "collidable_set_plane_geometry");

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
TEST_F(CollidableLuaTableTest, SweepReturnsFalseIfSweepTestFails) {
  physicsBackend->setSweepValue(false);

  auto entity = entityDatabase.create();
  auto state = call(entity, "collidable_sweep");
  EXPECT_TRUE(state["sweep_output"].is<bool>());
  EXPECT_FALSE(state["sweep_output"].get<bool>());
  EXPECT_TRUE(state["sweep_data"].is<sol::nil_t>());
}

TEST_F(CollidableLuaTableTest, SweepReturnsTrueIfSweepTestSucceeds) {
  physicsBackend->setSweepValue(true);
  physicsBackend->setSweepHitData({.normal = glm::vec3{2.5f, 3.5f, 4.5f}});

  auto entity = entityDatabase.create();
  auto state = call(entity, "collidable_sweep");
  EXPECT_TRUE(state["sweep_output"].is<bool>());
  EXPECT_TRUE(state["sweep_output"].get<bool>());

  EXPECT_FALSE(state["sweep_normal"].is<sol::nil_t>());

  auto normal = state["sweep_normal"].get<glm::vec3>();
  EXPECT_EQ(normal, glm::vec3(2.5f, 3.5f, 4.5f));
}

// Delete
TEST_F(CollidableLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidable_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaTableTest, DeleteRemovesCollidableComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Collidable>(entity, {});

  call(entity, "collidable_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}
