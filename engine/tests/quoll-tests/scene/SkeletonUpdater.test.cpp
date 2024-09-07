#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/skeleton/Skeleton.h"
#include "quoll/skeleton/SkeletonUpdater.h"
#include "quoll/system/SystemView.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheUtils.h"

struct SkeletonUpdaterTest : public ::testing::Test {

  SkeletonUpdaterTest() : assetCache("/") {}

  auto createSkeletonAsset(u32 numJoints) {
    quoll::SkeletonAsset skeleton;
    for (u32 i = 0; i < numJoints; ++i) {
      f32 value = static_cast<f32>(i) + 1.2f;
      skeleton.jointLocalPositions.push_back(glm::vec3(value));
      skeleton.jointLocalRotations.push_back(
          glm::quat(value, value, value, value));
      skeleton.jointLocalScales.push_back(glm::vec3(value));
      skeleton.jointParents.push_back(i > 0 ? i - 1 : 0);
      skeleton.jointInverseBindMatrices.push_back(glm::mat4(value));
      skeleton.jointNames.push_back("Joint " + std::to_string(i));
    }

    return createAssetInCache(assetCache, skeleton);
  }

  quoll::Entity createSkeletonEntity(u32 numJoints) {
    auto entity = entityDatabase.create();

    auto ref = createSkeletonAsset(numJoints);
    entityDatabase.set(entity, quoll::SkeletonAssetRef{ref});

    quoll::SkeletonDebug skeletonDebug{};
    auto numBones = ref->jointNames.size() * 2;
    skeletonDebug.bones.reserve(numBones);

    for (u32 joint = 0; joint < ref->jointNames.size(); ++joint) {
      skeletonDebug.bones.push_back(ref->jointParents.at(joint));
      skeletonDebug.bones.push_back(joint);
    }

    skeletonDebug.boneTransforms.resize(numBones, glm::mat4{1.0f});
    entityDatabase.set(entity, skeletonDebug);

    return entity;
  }

  quoll::Skeleton &getSkeleton(quoll::Entity entity) {
    return entityDatabase.get<quoll::Skeleton>(entity);
  }

  quoll::SkeletonDebug &getDebugSkeleton(quoll::Entity entity) {
    return entityDatabase.get<quoll::SkeletonDebug>(entity);
  }

  glm::mat4 getLocalTransform(quoll::Skeleton &skeleton, u32 i) {
    glm::mat4 identity{1.0f};
    return glm::translate(identity, skeleton.jointLocalPositions.at(i)) *
           glm::toMat4(skeleton.jointLocalRotations.at(i)) *
           glm::scale(identity, skeleton.jointLocalScales.at(i));
  };

  template <class T> std::vector<T> createItems(u32 numJoints) {
    return std::vector<T>(numJoints);
  }

  quoll::AssetCache assetCache;
  quoll::Scene scene;
  quoll::EntityDatabase &entityDatabase = scene.entityDatabase;
  quoll::SkeletonUpdater skeletonUpdater;
  quoll::SystemView view{&scene};
};

using SkeletonUpdaterDeathTest = SkeletonUpdaterTest;

TEST_F(SkeletonUpdaterTest,
       DoesNotCreateSkeletonIfNoSkeletonAssetRefComponent) {
  auto asset = createSkeletonAsset(1);

  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::SkeletonCurrentAsset>(entity));
  EXPECT_FALSE(entityDatabase.has<quoll::Skeleton>(entity));

  skeletonUpdater.update(view);

  EXPECT_FALSE(entityDatabase.has<quoll::SkeletonCurrentAsset>(entity));
  EXPECT_FALSE(entityDatabase.has<quoll::Skeleton>(entity));
}

TEST_F(SkeletonUpdaterTest, DoesNotCreateSkeletonIfNoSkeletonAssetHasNoData) {
  auto asset = createAssetInCacheWithoutData<quoll::SkeletonAsset>(assetCache);

  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::SkeletonCurrentAsset>(entity));
  EXPECT_FALSE(entityDatabase.has<quoll::Skeleton>(entity));

  skeletonUpdater.update(view);

  EXPECT_FALSE(entityDatabase.has<quoll::SkeletonCurrentAsset>(entity));
  EXPECT_FALSE(entityDatabase.has<quoll::Skeleton>(entity));
}

TEST_F(SkeletonUpdaterTest, CreatesSkeletonIfHasSkeletonAssetRef) {
  auto asset = createSkeletonAsset(3);

  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::SkeletonCurrentAsset>(entity));
  EXPECT_FALSE(entityDatabase.has<quoll::Skeleton>(entity));

  entityDatabase.set<quoll::SkeletonAssetRef>(entity, {asset});

  skeletonUpdater.update(view);

  ASSERT_TRUE(entityDatabase.has<quoll::SkeletonCurrentAsset>(entity));
  EXPECT_TRUE(entityDatabase.has<quoll::Skeleton>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::SkeletonCurrentAsset>(entity).handle,
            asset.handle());
}

TEST_F(SkeletonUpdaterTest, CreatesNewSkeletonComponentIfAssetRefHasChanged) {
  auto asset1 = createSkeletonAsset(1);
  auto asset2 = createSkeletonAsset(1);

  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::SkeletonCurrentAsset>(entity));
  EXPECT_FALSE(entityDatabase.has<quoll::Skeleton>(entity));

  entityDatabase.set<quoll::SkeletonAssetRef>(entity, {asset1});
  skeletonUpdater.update(view);
  EXPECT_TRUE(entityDatabase.has<quoll::SkeletonCurrentAsset>(entity));
  EXPECT_TRUE(entityDatabase.has<quoll::Skeleton>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::SkeletonCurrentAsset>(entity).handle,
            asset1.handle());

  entityDatabase.set<quoll::SkeletonAssetRef>(entity, {asset2});
  skeletonUpdater.update(view);
  EXPECT_TRUE(entityDatabase.has<quoll::SkeletonCurrentAsset>(entity));
  EXPECT_TRUE(entityDatabase.has<quoll::Skeleton>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::SkeletonCurrentAsset>(entity).handle,
            asset2.handle());
}
TEST_F(SkeletonUpdaterTest, UpdatesWorldTransformsOnUpdate) {
  auto entity = createSkeletonEntity(3);

  skeletonUpdater.update(view);

  auto &skeleton = entityDatabase.get<quoll::Skeleton>(entity);
  EXPECT_EQ(skeleton.jointWorldTransforms.at(0),
            getLocalTransform(skeleton, 0));
  EXPECT_EQ(skeleton.jointWorldTransforms.at(1),
            skeleton.jointWorldTransforms.at(0) *
                getLocalTransform(skeleton, 1));
  EXPECT_EQ(skeleton.jointWorldTransforms.at(2),
            skeleton.jointWorldTransforms.at(1) *
                getLocalTransform(skeleton, 2));
}

TEST_F(SkeletonUpdaterTest, UpdatesFinalTransformOnUpdate) {
  auto entity = createSkeletonEntity(3);

  skeletonUpdater.update(view);

  auto &skeleton = entityDatabase.get<quoll::Skeleton>(entity);
  EXPECT_EQ(skeleton.jointFinalTransforms.at(0),
            skeleton.jointWorldTransforms.at(0) *
                skeleton.jointInverseBindMatrices.at(0));
  EXPECT_EQ(skeleton.jointFinalTransforms.at(1),
            skeleton.jointWorldTransforms.at(1) *
                skeleton.jointInverseBindMatrices.at(1));
  EXPECT_EQ(skeleton.jointFinalTransforms.at(2),
            skeleton.jointWorldTransforms.at(2) *
                skeleton.jointInverseBindMatrices.at(2));
}

TEST_F(SkeletonUpdaterTest, UpdatesDebugBonesOnUpdate) {
  auto entity = createSkeletonEntity(2);

  skeletonUpdater.update(view);

  auto &skeleton = entityDatabase.get<quoll::Skeleton>(entity);
  auto &skeletonDebug = entityDatabase.get<quoll::SkeletonDebug>(entity);

  for (usize i = 0; i < skeletonDebug.bones.size(); ++i) {
    EXPECT_EQ(skeletonDebug.boneTransforms.at(i),
              skeleton.jointWorldTransforms.at(skeletonDebug.bones.at(i)));
  }
}

TEST_F(SkeletonUpdaterDeathTest,
       FailsIfDebugBoneSizeIsNotTwiceTheNumberOfJoints) {
  auto entity = createSkeletonEntity(2);
  entityDatabase.set<quoll::SkeletonDebug>(entity, {});

  EXPECT_DEATH(skeletonUpdater.update(view), ".*");
}
