#include "liquid/core/Base.h"
#include "liquidator/actions/EntityCollidableActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetCollidableTypeActionTest = ActionTestBase;

TEST_F(EntitySetCollidableTypeActionTest,
       ExecutorSetsCollidableTypeAndResetsTheParamsInScene) {
  auto entity = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<liquid::Collidable>(entity, {});

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  auto res = action.onExecute(state);

  auto sphere = std::get<liquid::PhysicsGeometrySphere>(
      state.scene.entityDatabase.get<liquid::Collidable>(entity)
          .geometryDesc.params);

  EXPECT_EQ(state.scene.entityDatabase.get<liquid::Collidable>(entity)
                .geometryDesc.type,
            liquid::PhysicsGeometryType::Sphere);
  EXPECT_EQ(sphere.radius, liquid::PhysicsGeometrySphere{}.radius);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetCollidableTypeActionTest,
       ExecutorSetsCollidableTypeAndResetsTheParamsInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  state.simulationScene.entityDatabase.set<liquid::Collidable>(entity, {});

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  auto res = action.onExecute(state);

  auto sphere = std::get<liquid::PhysicsGeometrySphere>(
      state.simulationScene.entityDatabase.get<liquid::Collidable>(entity)
          .geometryDesc.params);

  EXPECT_EQ(state.simulationScene.entityDatabase.get<liquid::Collidable>(entity)
                .geometryDesc.type,
            liquid::PhysicsGeometryType::Sphere);
  EXPECT_EQ(sphere.radius, liquid::PhysicsGeometrySphere{}.radius);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetCollidableTypeActionTest,
       PredicatesReturnsFalseIfNoCollidableComponentForEntityInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(
    EntitySetCollidableTypeActionTest,
    PredicatesReturnsFalseIfNoCollidableComponentForEntityInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(
    EntitySetCollidableTypeActionTest,
    PredicatesReturnsFalseIfEntityCollidableGeometryTypeEqualsProvidedTypeInScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Collidable>(entity, {});

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Box);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(
    EntitySetCollidableTypeActionTest,
    PredicatesReturnsFalseIfEntityCollidableGeometryTypeEqualsProvidedTypeInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::Collidable>(entity, {});

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Box);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(
    EntitySetCollidableTypeActionTest,
    PredicatesReturnsTrueIfCollidableTypeForEntityDoesNotEqualProvidedTypeInScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Collidable>(entity, {});

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(
    EntitySetCollidableTypeActionTest,
    PredicatesReturnsTrueIfCollidableTypeForEntityDoesNotEqualProvidedTypeInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::Collidable>(entity, {});

  liquid::editor::EntitySetCollidableType action(
      entity, liquid::PhysicsGeometryType::Sphere);
  EXPECT_TRUE(action.predicate(state));
}

using SetEntityCollidableAction = ActionTestBase;

TEST_F(SetEntityCollidableAction, ExecutorSetsCollidableForEntityInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::Collidable collidable{};
  collidable.materialDesc.staticFriction = 2.5f;

  liquid::editor::EntitySetCollidable action(entity, collidable);
  auto res = action.onExecute(state);

  EXPECT_EQ(state.scene.entityDatabase.get<liquid::Collidable>(entity)
                .materialDesc.staticFriction,
            2.5f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(SetEntityCollidableAction,
       ExecutorSetsCollidableTypeAndResetsTheParamsInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::Collidable collidable{};
  collidable.materialDesc.staticFriction = 2.5f;

  liquid::editor::EntitySetCollidable action(entity, collidable);
  auto res = action.onExecute(state);

  EXPECT_EQ(state.simulationScene.entityDatabase.get<liquid::Collidable>(entity)
                .materialDesc.staticFriction,
            2.5f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}
