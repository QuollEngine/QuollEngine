#include "quoll/core/Base.h"
#include "quoll/physics/Collidable.h"
#include "quoll/editor/actions/EntityCollidableActions.h"

#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

#include "DefaultEntityTests.h"

using EntitySetCollidableTypeActionTest = ActionTestBase;

TEST_F(EntitySetCollidableTypeActionTest,
       ExecutorSetsCollidableTypeAndResetsTheParamsOfExistingCollidable) {
  auto entity = state.scene.entityDatabase.create();

  quoll::Collidable collidable{};
  collidable.materialDesc.staticFriction = 5.0f;
  state.scene.entityDatabase.set(entity, collidable);

  quoll::editor::EntitySetCollidableType action(
      entity, quoll::PhysicsGeometryType::Sphere);
  auto res = action.onExecute(state, assetRegistry);

  const auto &collidableNew =
      state.scene.entityDatabase.get<quoll::Collidable>(entity);

  auto sphere =
      std::get<quoll::PhysicsGeometrySphere>(collidableNew.geometryDesc.params);

  EXPECT_EQ(collidableNew.geometryDesc.type,
            quoll::PhysicsGeometryType::Sphere);
  EXPECT_EQ(sphere.radius, quoll::PhysicsGeometrySphere{}.radius);
  EXPECT_EQ(collidableNew.materialDesc.staticFriction, 5.0f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetCollidableTypeActionTest,
       UndoSetsOldCollidableParametersForEntity) {
  auto entity = state.scene.entityDatabase.create();

  quoll::Collidable collidable{};
  std::get<quoll::PhysicsGeometryBox>(collidable.geometryDesc.params)
      .halfExtents = glm::vec3(0.2f);
  collidable.materialDesc.staticFriction = 5.0f;
  state.scene.entityDatabase.set(entity, collidable);

  quoll::editor::EntitySetCollidableType action(
      entity, quoll::PhysicsGeometryType::Sphere);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  const auto &collidableNew =
      state.scene.entityDatabase.get<quoll::Collidable>(entity);

  EXPECT_EQ(collidableNew.geometryDesc.type, quoll::PhysicsGeometryType::Box);
  EXPECT_EQ(
      std::get<quoll::PhysicsGeometryBox>(collidableNew.geometryDesc.params)
          .halfExtents,
      glm::vec3(0.2f));
  EXPECT_EQ(collidableNew.materialDesc.staticFriction, 5.0f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetCollidableTypeActionTest,
       PredicatesReturnsFalseIfEntityHasNoCollidableComponent) {
  auto entity = state.scene.entityDatabase.create();

  quoll::editor::EntitySetCollidableType action(
      entity, quoll::PhysicsGeometryType::Sphere);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntitySetCollidableTypeActionTest,
       PredicatesReturnsFalseIfEntityCollidableGeometryTypeEqualsProvidedType) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Collidable>(entity, {});

  quoll::editor::EntitySetCollidableType action(
      entity, quoll::PhysicsGeometryType::Box);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntitySetCollidableTypeActionTest,
       PredicatesReturnsTrueIfCollidableTypeForEntityDoesNotEqualProvidedType) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Collidable>(entity, {});

  quoll::editor::EntitySetCollidableType action(
      entity, quoll::PhysicsGeometryType::Sphere);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}
