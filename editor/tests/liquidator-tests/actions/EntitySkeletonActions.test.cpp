#include "liquid/core/Base.h"
#include "liquidator/actions/EntitySkeletonActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntityToggleSkeletonDebugBonesActionTest = ActionTestBase;

TEST_P(EntityToggleSkeletonDebugBonesActionTest,
       ExecutorSetsDebugBonesForEntityIfNoDebugBones) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Skeleton>(entity, {});

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::SkeletonDebug>(entity));
  EXPECT_TRUE(res.entitiesToSave.empty());
}

TEST_P(EntityToggleSkeletonDebugBonesActionTest,
       ExecutorRemovesDebugBonesForEntityIfHasDebugBones) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Skeleton>(entity, {});
  activeScene().entityDatabase.set<liquid::SkeletonDebug>(entity, {});

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::SkeletonDebug>(entity));
  EXPECT_TRUE(res.entitiesToSave.empty());
}

TEST_P(EntityToggleSkeletonDebugBonesActionTest,
       PredicateReturnsFalseIfEntityHasNoSkeleton) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntityToggleSkeletonDebugBonesActionTest,
       PredicateReturnsTrueIfEntityHasSkeleton) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Skeleton>(entity, {});

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest,
                     EntityToggleSkeletonDebugBonesActionTest);

using EntitySetSkeletonActionTest = ActionTestBase;

TEST_P(EntitySetSkeletonActionTest,
       ExecutorSetsSkeletonComponentFromAssetRegistry) {
  liquid::AssetData<liquid::SkeletonAsset> asset{};
  for (size_t i = 0; i < 5; ++i) {
    float value = static_cast<float>(i);
    asset.data.jointParents.push_back(static_cast<liquid::JointId>(i));
    asset.data.jointNames.push_back(std::to_string(i));
    asset.data.jointLocalPositions.push_back(glm::vec3{value});
    asset.data.jointLocalScales.push_back(glm::vec3{value});
    asset.data.jointLocalRotations.push_back(
        glm::quat(value / 5.0f, 0.0f, 0.0f, 1.0f));
    asset.data.jointInverseBindMatrices.push_back(glm::mat4{value});
  }
  auto handle = state.assetRegistry.getSkeletons().addAsset(asset);

  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntitySetSkeleton action(entity, handle);

  auto res = action.onExecute(state);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Skeleton>(entity));
  auto &skeleton = activeScene().entityDatabase.get<liquid::Skeleton>(entity);

  EXPECT_EQ(skeleton.assetHandle, handle);
  EXPECT_EQ(skeleton.numJoints, 5);

  for (size_t i = 0; i < 5; ++i) {
    EXPECT_EQ(skeleton.jointNames.at(i), asset.data.jointNames.at(i));
    EXPECT_EQ(skeleton.jointParents.at(i), asset.data.jointParents.at(i));
    EXPECT_EQ(skeleton.jointLocalPositions.at(i),
              asset.data.jointLocalPositions.at(i));
    EXPECT_EQ(skeleton.jointLocalRotations.at(i),
              asset.data.jointLocalRotations.at(i));
    EXPECT_EQ(skeleton.jointLocalScales.at(i),
              asset.data.jointLocalScales.at(i));
    EXPECT_EQ(skeleton.jointInverseBindMatrices.at(i),
              asset.data.jointInverseBindMatrices.at(i));
    EXPECT_EQ(skeleton.jointFinalTransforms.at(i), glm::mat4{1.0f});
    EXPECT_EQ(skeleton.jointWorldTransforms.at(i), glm::mat4{1.0f});
  }

  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetSkeletonActionTest, PredicateReturnsTrueIfSkeletonAssetExists) {
  liquid::AssetData<liquid::SkeletonAsset> asset{};
  auto handle = state.assetRegistry.getSkeletons().addAsset(asset);

  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntitySetSkeleton action(entity, handle);

  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntitySetSkeletonActionTest,
       PredicateReturnsFalseIfSkeletonAssetDoesNotExist) {
  liquid::AssetData<liquid::SkeletonAsset> asset{};
  liquid::SkeletonAssetHandle handle{15};

  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntitySetSkeleton action(entity, handle);

  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntitySetSkeletonActionTest);

using EntityDeleteSkeletonActionTest = ActionTestBase;

TEST_P(EntityDeleteSkeletonActionTest,
       ExecutorDeletesSkeletonComponentFromEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Skeleton>(entity, {});

  liquid::editor::EntityDeleteSkeleton action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Skeleton>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityDeleteSkeletonActionTest,
       ExecutorDeletesSkeletonDebugComponentFromEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Skeleton>(entity, {});
  activeScene().entityDatabase.set<liquid::SkeletonDebug>(entity, {});

  liquid::editor::EntityDeleteSkeleton action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Skeleton>(entity));
  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::SkeletonDebug>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityDeleteSkeletonActionTest,
       PredicateReturnsTrueIfEntityHasSkeletonComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Skeleton>(entity, {});

  liquid::editor::EntityDeleteSkeleton action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntityDeleteSkeletonActionTest,
       PredicateReturnsFalseIfEntityHasNoSkeletonComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityDeleteSkeleton action(entity);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteSkeletonActionTest);
