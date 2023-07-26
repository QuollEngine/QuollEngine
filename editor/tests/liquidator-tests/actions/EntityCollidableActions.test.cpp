#include "liquid/core/Base.h"
#include "liquidator/actions/EntityCollidableActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

#include "DefaultEntityTests.h"

using EntityCreateCollidableActionTest = ActionTestBase;
InitDefaultCreateComponentTests(EntityCreateCollidableActionTest,
                                EntityCreateCollidable, Collidable);
InitActionsTestSuite(EntityActionsTest, EntityCreateCollidableActionTest);

using EntityDeleteCollidableActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteCollidableActionTest,
                                EntityDeleteCollidable, Collidable);
InitActionsTestSuite(EntityActionsTest, EntityDeleteCollidableActionTest);

using EntitySetCollidableActionTest = ActionTestBase;

InitDefaultUpdateComponentTests(EntitySetCollidableActionTest,
                                EntitySetCollidable, Collidable,
                                materialDesc.staticFriction, 2.5f);
InitActionsTestSuite(EntityActionsTest, EntitySetCollidableActionTest);

using EntitySetCollidableTypeActionTest = ActionTestBase;

TEST_P(EntitySetCollidableTypeActionTest,
       ExecutorSetsCollidableTypeAndResetsTheParamsOfExistingCollidable) {
  auto entity = activeScene().entityDatabase.create();

  liquid::Collidable collidable{};
  collidable.materialDesc.staticFriction = 5.0f;
  activeScene().entityDatabase.set(entity, collidable);

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  auto res = action.onExecute(state, assetRegistry);

  const auto &collidableNew =
      activeScene().entityDatabase.get<liquid::Collidable>(entity);

  auto sphere = std::get<liquid::PhysicsGeometrySphere>(
      collidableNew.geometryDesc.params);

  EXPECT_EQ(collidableNew.geometryDesc.type,
            liquid::PhysicsGeometryType::Sphere);
  EXPECT_EQ(sphere.radius, liquid::PhysicsGeometrySphere{}.radius);
  EXPECT_EQ(collidableNew.materialDesc.staticFriction, 5.0f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetCollidableTypeActionTest,
       UndoSetsOldCollidableParametersForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::Collidable collidable{};
  std::get<liquid::PhysicsGeometryBox>(collidable.geometryDesc.params)
      .halfExtents = glm::vec3(0.2f);
  collidable.materialDesc.staticFriction = 5.0f;
  activeScene().entityDatabase.set(entity, collidable);

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  const auto &collidableNew =
      activeScene().entityDatabase.get<liquid::Collidable>(entity);

  EXPECT_EQ(collidableNew.geometryDesc.type, liquid::PhysicsGeometryType::Box);
  EXPECT_EQ(
      std::get<liquid::PhysicsGeometryBox>(collidableNew.geometryDesc.params)
          .halfExtents,
      glm::vec3(0.2f));
  EXPECT_EQ(collidableNew.materialDesc.staticFriction, 5.0f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetCollidableTypeActionTest,
       PredicatesReturnsFalseIfEntityHasNoCollidableComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetCollidableTypeActionTest,
       PredicatesReturnsFalseIfEntityCollidableGeometryTypeEqualsProvidedType) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Collidable>(entity, {});

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Box);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetCollidableTypeActionTest,
       PredicatesReturnsTrueIfCollidableTypeForEntityDoesNotEqualProvidedType) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Collidable>(entity, {});

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntitySetCollidableTypeActionTest);
