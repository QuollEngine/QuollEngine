#include "quoll/core/Base.h"
#include "quoll/core/Delete.h"
#include "quoll/core/Name.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/Sprite.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll/editor/actions/SpawnEntityActions.h"
#include "quoll-tests/Testing.h"
#include "ActionTestBase.h"

using SpawnEmptyEntityAtViewActionTest = ActionTestBase;

TEST_F(SpawnEmptyEntityAtViewActionTest, ExecutorSpawnsEmptyEntityAtView) {
  auto camera = state.scene.entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  quoll::Camera component{};
  component.viewMatrix = viewMatrix;
  state.scene.entityDatabase.set(camera, component);
  state.camera = camera;

  quoll::editor::SpawnEmptyEntityAtView action;
  auto res = action.onExecute(state, assetCache);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_NE(res.entitiesToSave.at(0), quoll::Entity::Null);

  auto entity = res.entitiesToSave.at(0);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::LocalTransform>(entity)
                .localPosition,
            glm::vec3(0.0f, 0.0f, -10.0f));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Name>(entity).name,
            "New entity");
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::WorldTransform>(entity));
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(SpawnEmptyEntityAtViewActionTest, UndoRemovesSpawnedEntity) {
  auto camera = state.scene.entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  quoll::Camera component{};
  component.viewMatrix = viewMatrix;
  state.scene.entityDatabase.set(camera, component);
  state.camera = camera;

  quoll::editor::SpawnEmptyEntityAtView action;
  auto execRes = action.onExecute(state, assetCache);
  auto undoRes = action.onUndo(state, assetCache);

  EXPECT_EQ(execRes.entitiesToSave.size(), 1);
  EXPECT_EQ(undoRes.entitiesToDelete.size(), 1);
  EXPECT_EQ(execRes.entitiesToSave.at(0), undoRes.entitiesToDelete.at(0));
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Delete>(
      execRes.entitiesToSave.at(0)));
}

TEST_F(SpawnEmptyEntityAtViewActionTest,
       UndoSetsSelectedEntityToNullIfSpawnedEntityIsSelected) {
  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnEmptyEntityAtView action;
  auto res = action.onExecute(state, assetCache);
  state.selectedEntity = res.entitiesToSave.at(0);

  action.onUndo(state, assetCache);

  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_F(
    SpawnEmptyEntityAtViewActionTest,
    UndoSetsSelectedEntityToNullIfSelectedEntityIsADescendantOfSpawnedEntity) {
  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnEmptyEntityAtView action;
  auto res = action.onExecute(state, assetCache);

  auto entity = res.entitiesToSave.at(0);

  {
    auto e1 = state.scene.entityDatabase.create();
    state.scene.entityDatabase.set<quoll::Parent>(e1, {entity});

    auto e2 = state.scene.entityDatabase.create();
    state.scene.entityDatabase.set<quoll::Parent>(e2, {e1});
    state.selectedEntity = e2;
  }

  action.onUndo(state, assetCache);

  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_F(SpawnEmptyEntityAtViewActionTest,
       UndoDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotSpawnedEntity) {
  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;
  state.selectedEntity = state.scene.entityDatabase.create();

  quoll::editor::SpawnEmptyEntityAtView action;
  action.onExecute(state, assetCache);
  action.onUndo(state, assetCache);

  EXPECT_NE(state.selectedEntity, quoll::Entity::Null);
}

TEST_F(SpawnEmptyEntityAtViewActionTest,
       PredicateReturnsTrueIfCameraEntityHasCamera) {
  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnEmptyEntityAtView action;
  EXPECT_TRUE(action.predicate(state, assetCache));
}

TEST_F(SpawnEmptyEntityAtViewActionTest,
       PredicateReturnsFalseIfCameraEntityHasNoCamera) {
  auto camera = state.scene.entityDatabase.create();
  state.camera = camera;

  quoll::editor::SpawnEmptyEntityAtView action;
  EXPECT_FALSE(action.predicate(state, assetCache));
}

using SpawnPrefabAtViewActionTest = ActionTestBase;

TEST_F(SpawnPrefabAtViewActionTest, ExecutorSpawnsPrefabAtView) {
  auto camera = state.scene.entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  quoll::Camera component{};
  component.viewMatrix = viewMatrix;
  state.scene.entityDatabase.set(camera, component);

  quoll::PrefabAsset data{};
  data.transforms.push_back({0});
  data.transforms.push_back({1});
  auto prefab = createAsset(data);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state, assetCache);

  ASSERT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_NE(res.entitiesToSave.at(0), quoll::Entity::Null);
  EXPECT_NE(res.entitiesToSave.at(1), quoll::Entity::Null);
  EXPECT_NE(res.entitiesToSave.at(2), quoll::Entity::Null);

  auto entity = res.entitiesToSave.at(2);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::LocalTransform>(entity)
                .localPosition,
            glm::vec3(0.0f, 0.0f, -10.0f));
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(SpawnPrefabAtViewActionTest, UndoRemovesRootNode) {
  auto camera = state.scene.entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  quoll::Camera component{};
  component.viewMatrix = viewMatrix;
  state.scene.entityDatabase.set(camera, component);

  quoll::PrefabAsset data{};
  data.transforms.push_back({0});
  data.transforms.push_back({1});
  auto prefab = createAsset(data);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  auto execRes = action.onExecute(state, assetCache);

  auto undoRes = action.onUndo(state, assetCache);

  ASSERT_EQ(execRes.entitiesToSave.size(), 3);
  ASSERT_EQ(undoRes.entitiesToDelete.size(), 1);

  auto entity = undoRes.entitiesToDelete.at(0);

  EXPECT_EQ(undoRes.entitiesToDelete.back(), entity);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Delete>(entity));
}

TEST_F(SpawnPrefabAtViewActionTest,
       UndoSetsSelectedEntityToNullIfSpawnedRootIsSelected) {
  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});

  quoll::PrefabAsset data{};
  data.transforms.push_back({0});
  auto prefab = createAsset(data);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state, assetCache);

  auto entity = res.entitiesToSave.at(0);
  state.selectedEntity = entity;

  action.onUndo(state, assetCache);
  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_F(SpawnPrefabAtViewActionTest,
       UndoSetsSelectedEntityToNullIfSelectedEntityIsDescendantOfSpawnedRoot) {
  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});

  quoll::PrefabAsset data{};
  data.transforms.push_back({0});
  data.transforms.push_back({1});
  auto prefab = createAsset(data);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state, assetCache);

  state.selectedEntity = res.entitiesToSave.back();

  action.onUndo(state, assetCache);
  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_F(
    SpawnEmptyEntityAtViewActionTest,
    UndoDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotInSpawnedPrefabTree) {
  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});

  quoll::PrefabAsset data{};
  data.transforms.push_back({0});
  data.transforms.push_back({1});
  auto prefab = createAsset(data);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state, assetCache);

  state.selectedEntity = quoll::Entity{25};

  action.onUndo(state, assetCache);
  EXPECT_NE(state.selectedEntity, quoll::Entity::Null);
}

TEST_F(
    SpawnPrefabAtViewActionTest,
    PredicateReturnsTrueIfPrefabAssetExistsAndIsNotEmptyAndCameraEntityHasCamera) {
  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});

  quoll::PrefabAsset data{};
  data.transforms.push_back({0});
  auto prefab = createAsset(data);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  EXPECT_TRUE(action.predicate(state, assetCache));
}

TEST_F(SpawnPrefabAtViewActionTest,
       PredicateReturnsFalseIfPrefabAssetDoesNotExist) {
  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});
  quoll::editor::SpawnPrefabAtView action(quoll::AssetRef<quoll::PrefabAsset>(),
                                          camera);
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(SpawnPrefabAtViewActionTest, PredicateReturnsFalseIfPrefabAssetIsEmpty) {
  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});

  auto prefab = createAsset<quoll::PrefabAsset>();

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(SpawnPrefabAtViewActionTest,
       PredicateReturnsFalseIfCameraEntityHasNoCamera) {
  auto camera = state.scene.entityDatabase.create();

  quoll::PrefabAsset data{};
  data.transforms.push_back({0});
  auto prefab = createAsset(data);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  EXPECT_FALSE(action.predicate(state, assetCache));
}

using SpawnSpriteAtViewActionTest = ActionTestBase;

TEST_F(SpawnSpriteAtViewActionTest, ExecutorSpawnsSpriteAtView) {
  auto texture =
      createAssetInCache<quoll::TextureAsset>(assetCache, {}, "my-texture");

  auto camera = state.scene.entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  quoll::Camera component{};
  component.viewMatrix = viewMatrix;
  state.scene.entityDatabase.set(camera, component);
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(texture, camera);
  auto res = action.onExecute(state, assetCache);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_NE(res.entitiesToSave.at(0), quoll::Entity::Null);

  auto entity = res.entitiesToSave.at(0);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::LocalTransform>(entity)
                .localPosition,
            glm::vec3(0.0f, 0.0f, -10.0f));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Name>(entity).name,
            "my-texture");
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::WorldTransform>(entity));
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Sprite>(entity));

  EXPECT_TRUE(res.addToHistory);
}

TEST_F(SpawnSpriteAtViewActionTest, UndoRemovesSpawnedEntity) {
  auto texture = createAsset<quoll::TextureAsset>();

  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(texture, camera);
  auto execRes = action.onExecute(state, assetCache);
  auto undoRes = action.onUndo(state, assetCache);

  EXPECT_EQ(execRes.entitiesToSave.size(), 1);
  EXPECT_EQ(undoRes.entitiesToDelete.size(), 1);
  EXPECT_EQ(execRes.entitiesToSave.at(0), undoRes.entitiesToDelete.at(0));
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Delete>(
      execRes.entitiesToSave.at(0)));
}

TEST_F(SpawnSpriteAtViewActionTest,
       UndoSetsSelectedEntityToNullIfSpawnedEntityIsSelected) {
  auto texture = createAsset<quoll::TextureAsset>();

  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(texture, camera);

  auto res = action.onExecute(state, assetCache);
  state.selectedEntity = res.entitiesToSave.at(0);

  action.onUndo(state, assetCache);

  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_F(
    SpawnSpriteAtViewActionTest,
    UndoSetsSelectedEntityToNullIfSelectedEntityIsADescendantOfSpawnedEntity) {
  auto texture = createAsset<quoll::TextureAsset>();

  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(texture, camera);
  auto res = action.onExecute(state, assetCache);
  state.selectedEntity = res.entitiesToSave.at(0);

  auto entity = res.entitiesToSave.at(0);

  {
    auto e1 = state.scene.entityDatabase.create();
    state.scene.entityDatabase.set<quoll::Parent>(e1, {entity});

    auto e2 = state.scene.entityDatabase.create();
    state.scene.entityDatabase.set<quoll::Parent>(e2, {e1});
    state.selectedEntity = e2;
  }

  action.onUndo(state, assetCache);

  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_F(SpawnSpriteAtViewActionTest,
       UndoDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotSpawnedEntity) {
  auto texture = createAsset<quoll::TextureAsset>();

  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;
  state.selectedEntity = state.scene.entityDatabase.create();

  quoll::editor::SpawnSpriteAtView action(texture, camera);

  action.onExecute(state, assetCache);
  action.onUndo(state, assetCache);

  EXPECT_NE(state.selectedEntity, quoll::Entity::Null);
}

TEST_F(SpawnSpriteAtViewActionTest,
       PredicateReturnsTrueIfCameraEntityHasCameraAndAssetExists) {
  auto texture = createAsset<quoll::TextureAsset>();

  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(texture, camera);

  EXPECT_TRUE(action.predicate(state, assetCache));
}

TEST_F(SpawnSpriteAtViewActionTest,
       PredicateReturnsFalseIfCameraEntityHasNoCamera) {
  auto texture = createAsset<quoll::TextureAsset>();

  auto camera = state.scene.entityDatabase.create();
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(texture, camera);

  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(SpawnSpriteAtViewActionTest, PredicateReturnsFalseIfAssetDoesNotExist) {
  auto camera = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(
      quoll::AssetRef<quoll::TextureAsset>(), camera);

  EXPECT_FALSE(action.predicate(state, assetCache));
}
