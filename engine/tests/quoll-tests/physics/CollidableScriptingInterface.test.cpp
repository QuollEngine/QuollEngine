#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"

#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class CollidableLuaScriptingInterfaceTest
    : public LuaScriptingInterfaceTestBase {};

TEST_F(CollidableLuaScriptingInterfaceTest,
       SetDefaultMaterialDoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
  call(entity, "collidable_set_default_material_invalid");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       SetDefaultMaterialCreatesCollidableIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
  call(entity, "collidable_set_default_material");
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
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
TEST_F(CollidableLuaScriptingInterfaceTest,
       GetStaticFrictionReturnsNullIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Collidable>(entity, {});

  call(entity, "collidable_get_static_friction_invalid");
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       GetStaticFrictionReturnsNullIfCollidableDoesNotExist) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "collidable_get_static_friction");
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("static_friction"));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       GetStaticFrictionReturnsCollidableStaticFriction) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.staticFriction = 2.5f;
  entityDatabase.set(entity, collidable);

  auto &scope = call(entity, "collidable_get_static_friction");
  EXPECT_EQ(scope.getGlobal<float>("static_friction"), 2.5f);
}

// Set static friction
TEST_F(CollidableLuaScriptingInterfaceTest,
       SetStaticFrictionDoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
  call(entity, "collidable_set_static_friction_invalid");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       SetStaticFrictionCreatesCollidableIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidable_set_static_friction");
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  EXPECT_EQ(
      entityDatabase.get<quoll::Collidable>(entity).materialDesc.staticFriction,
      2.5);
}

TEST_F(CollidableLuaScriptingInterfaceTest,
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
TEST_F(CollidableLuaScriptingInterfaceTest,
       GetDynamicFrictionReturnsNullIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Collidable>(entity, {});

  call(entity, "collidable_get_dynamic_friction_invalid");
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       GetDynamicFrictionReturnsNullIfCollidableDoesNotExist) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "collidable_get_dynamic_friction");
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("dynamic_friction"));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       GetDynamicFrictionReturnsCollidableDynamicFriction) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.dynamicFriction = 2.5f;
  entityDatabase.set(entity, collidable);

  auto &scope = call(entity, "collidable_get_dynamic_friction");
  EXPECT_EQ(scope.getGlobal<float>("dynamic_friction"), 2.5f);
}

// Set dynamic friction
TEST_F(CollidableLuaScriptingInterfaceTest,
       SetDynamicFrictionDoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
  call(entity, "collidable_set_dynamic_friction_invalid");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       SetDynamicFrictionCreatesCollidableIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidable_set_dynamic_friction");
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::Collidable>(entity)
                .materialDesc.dynamicFriction,
            2.5);
}

TEST_F(CollidableLuaScriptingInterfaceTest,
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
TEST_F(CollidableLuaScriptingInterfaceTest,
       GetRestitutionReturnsNullIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Collidable>(entity, {});

  call(entity, "collidable_get_restitution_invalid");
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       GetRestitutionReturnsNullIfCollidableDoesNotExist) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "collidable_get_restitution");
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("restitution"));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       GetRestitutionReturnsCollidableRestitution) {
  auto entity = entityDatabase.create();
  quoll::Collidable collidable{};
  collidable.materialDesc.restitution = 2.5f;
  entityDatabase.set(entity, collidable);

  auto &scope = call(entity, "collidable_get_restitution");
  EXPECT_EQ(scope.getGlobal<float>("restitution"), 2.5f);
}

// Set restitution
TEST_F(CollidableLuaScriptingInterfaceTest,
       SetRestitutionDoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
  call(entity, "collidable_set_restitution_invalid");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       SetRestitutionCreatesCollidableIfItDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidable_set_restitution");
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  EXPECT_EQ(
      entityDatabase.get<quoll::Collidable>(entity).materialDesc.restitution,
      2.5);
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       SetRestitutionUpdatesExistingCollidableIfExists) {
  auto entity = entityDatabase.create();

  quoll::Collidable collidable{};
  collidable.materialDesc.dynamicFriction = 2.5f;
  entityDatabase.set(entity, collidable);

  call(entity, "collidable_set_restitution");

  const auto &updatedCollidable = entityDatabase.get<quoll::Collidable>(entity);
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
  EXPECT_EQ(updatedCollidable.materialDesc.restitution, 2.5);
  EXPECT_NE(updatedCollidable.materialDesc.restitution,
            collidable.materialDesc.restitution);
  EXPECT_EQ(updatedCollidable.materialDesc.dynamicFriction,
            collidable.materialDesc.dynamicFriction);
}

// Set box geometry
TEST_F(CollidableLuaScriptingInterfaceTest,
       SetBoxGeometryDoesNothingIfProvidedArgumentsAreInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
  call(entity, "collidable_set_box_geometry_invalid");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
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

TEST_F(CollidableLuaScriptingInterfaceTest,
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
TEST_F(CollidableLuaScriptingInterfaceTest,
       SetSphereGeometryDoesNothingIfProvidedArgumentsAreInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
  call(entity, "collidable_set_sphere_geometry_invalid");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
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

TEST_F(CollidableLuaScriptingInterfaceTest,
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
TEST_F(CollidableLuaScriptingInterfaceTest,
       SetCapsuleGeometryDoesNothingIfProvidedArgumentsAreInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
  call(entity, "collidable_set_capsule_geometry_invalid");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
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

TEST_F(CollidableLuaScriptingInterfaceTest,
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
TEST_F(CollidableLuaScriptingInterfaceTest,
       SetPlaneGeometryDoesNothingIfProvidedArgumentsAreInvalid) {
  auto entity = entityDatabase.create();

  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
  call(entity, "collidable_set_plane_geometry_invalid");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       SetPlaneGeometryCreatesCollidableIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidable_set_plane_geometry");

  EXPECT_EQ(entityDatabase.get<quoll::Collidable>(entity).geometryDesc.type,
            quoll::PhysicsGeometryType::Plane);
  EXPECT_TRUE(std::holds_alternative<quoll::PhysicsGeometryPlane>(
      entityDatabase.get<quoll::Collidable>(entity).geometryDesc.params));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
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
TEST_F(CollidableLuaScriptingInterfaceTest,
       SweepReturnsFalseIfProvidedArgumentsAreInvalid) {
  physicsBackend->setSweepValue(true);

  auto entity = entityDatabase.create();
  call(entity, "collidable_sweep_invalid");
}

TEST_F(CollidableLuaScriptingInterfaceTest, SweepReturnsFalseIfSweepTestFails) {
  physicsBackend->setSweepValue(false);

  auto entity = entityDatabase.create();
  auto scope = call(entity, "collidable_sweep");
  EXPECT_TRUE(scope.isGlobal<bool>("sweep_output"));
  EXPECT_FALSE(scope.getGlobal<bool>("sweep_output"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("sweep_data"));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       SweepReturnsTrueIfSweepTestSucceeds) {
  physicsBackend->setSweepValue(true);
  physicsBackend->setSweepHitData({.normal = glm::vec3{2.5f, 3.5f, 4.5f}});

  auto entity = entityDatabase.create();
  auto scope = call(entity, "collidable_sweep");
  EXPECT_TRUE(scope.isGlobal<bool>("sweep_output"));
  EXPECT_TRUE(scope.getGlobal<bool>("sweep_output"));

  EXPECT_FALSE(scope.isGlobal<std::nullptr_t>("sweep_normal"));

  auto normal = scope.getGlobal<glm::vec3>("sweep_normal");
  EXPECT_EQ(normal, glm::vec3(2.5f, 3.5f, 4.5f));
}

// Delete
TEST_F(CollidableLuaScriptingInterfaceTest,
       DeleteDoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Collidable>(entity, {});

  call(entity, "collidable_delete_invalid");
  EXPECT_TRUE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "collidable_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}

TEST_F(CollidableLuaScriptingInterfaceTest,
       DeleteRemovesCollidableComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Collidable>(entity, {});

  call(entity, "collidable_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Collidable>(entity));
}
