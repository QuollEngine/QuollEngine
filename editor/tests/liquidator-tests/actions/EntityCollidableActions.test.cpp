#include "liquid/core/Base.h"
#include "liquidator/actions/EntityCollidableActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetCollidableTypeActionTest = ActionTestBase;

TEST_P(EntitySetCollidableTypeActionTest,
       ExecutorSetsCollidableTypeAndResetsTheParamsOfExistingCollidable) {
  auto entity = activeScene().entityDatabase.create();

  liquid::Collidable collidable{};
  collidable.materialDesc.staticFriction = 5.0f;
  activeScene().entityDatabase.set(entity, collidable);

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  auto res = action.onExecute(state);

  auto sphere = std::get<liquid::PhysicsGeometrySphere>(
      activeScene()
          .entityDatabase.get<liquid::Collidable>(entity)
          .geometryDesc.params);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::Collidable>(entity)
                .geometryDesc.type,
            liquid::PhysicsGeometryType::Sphere);
  EXPECT_EQ(sphere.radius, liquid::PhysicsGeometrySphere{}.radius);
  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::Collidable>(entity)
                .materialDesc.staticFriction,
            5.0f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetCollidableTypeActionTest,
       ExecutorCreatesNewCollidableIfEntityDoesNotHaveCollidable) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Capsule);
  auto res = action.onExecute(state);

  auto capsule = std::get<liquid::PhysicsGeometryCapsule>(
      activeScene()
          .entityDatabase.get<liquid::Collidable>(entity)
          .geometryDesc.params);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::Collidable>(entity)
                .geometryDesc.type,
            liquid::PhysicsGeometryType::Capsule);
  EXPECT_EQ(capsule.radius, liquid::PhysicsGeometryCapsule{}.radius);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetCollidableTypeActionTest,
       PredicatesReturnsTrueIfEntityHasNoCollidableComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntitySetCollidableTypeActionTest,
       PredicatesReturnsFalseIfEntityCollidableGeometryTypeEqualsProvidedType) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Collidable>(entity, {});

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Box);
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntitySetCollidableTypeActionTest,
       PredicatesReturnsTrueIfCollidableTypeForEntityDoesNotEqualProvidedType) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Collidable>(entity, {});

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntitySetCollidableTypeActionTest);

using SetEntityCollidableAction = ActionTestBase;

TEST_P(SetEntityCollidableAction, ExecutorSetsCollidableForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::Collidable collidable{};
  collidable.materialDesc.staticFriction = 2.5f;

  liquid::editor::EntitySetCollidable action(entity, collidable);
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::Collidable>(entity)
                .materialDesc.staticFriction,
            2.5f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, SetEntityCollidableAction);
