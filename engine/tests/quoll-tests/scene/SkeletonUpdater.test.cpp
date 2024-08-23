#include "quoll/core/Base.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/skeleton/Skeleton.h"
#include "quoll/skeleton/SkeletonUpdater.h"
#include "quoll/system/SystemView.h"
#include "quoll-tests/Testing.h"

struct SkeletonUpdaterTest : public ::testing::Test {
  quoll::SkeletonAssetHandle handle{2};
  quoll::Scene scene;
  quoll::EntityDatabase &entityDatabase = scene.entityDatabase;
  quoll::SkeletonUpdater skeletonUpdater;
  quoll::SystemView view{&scene};

  SkeletonUpdaterTest() { skeletonUpdater.createSystemViewData(view); }

  std::tuple<quoll::Skeleton &, quoll::SkeletonDebug &, quoll::Entity>
  createSkeleton(u32 numJoints) {
    auto entity = entityDatabase.entity();

    quoll::Skeleton skeleton;

    skeleton.jointWorldTransforms.resize(numJoints, glm::mat4{1.0f});
    skeleton.jointFinalTransforms.resize(numJoints, glm::mat4{1.0f});
    skeleton.numJoints = numJoints;

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

    quoll::SkeletonDebug skeletonDebug{};
    auto numBones = skeleton.numJoints * 2;
    skeletonDebug.bones.reserve(numBones);

    for (u32 joint = 0; joint < skeleton.numJoints; ++joint) {
      skeletonDebug.bones.push_back(skeleton.jointParents.at(joint));
      skeletonDebug.bones.push_back(joint);
    }

    skeletonDebug.boneTransforms.resize(numBones, glm::mat4{1.0f});

    entity.set(skeleton);
    entity.set(skeletonDebug);

    return {getSkeleton(entity), getDebugSkeleton(entity), entity};
  }

  quoll::Skeleton &getSkeleton(quoll::Entity entity) {
    return *entity.get_ref<quoll::Skeleton>().get();
  }

  quoll::SkeletonDebug &getDebugSkeleton(quoll::Entity entity) {
    return *entity.get_ref<quoll::SkeletonDebug>().get();
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
};

using SkeletonUpdaterDeathTest = SkeletonUpdaterTest;

TEST_F(SkeletonUpdaterTest, UpdatesWorldTransformsOnUpdate) {
  const auto &[skeleton, _, _2] = createSkeleton(3);

  EXPECT_EQ(skeleton.jointWorldTransforms.at(0), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.jointWorldTransforms.at(1), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.jointWorldTransforms.at(2), glm::mat4(1.0f));

  skeletonUpdater.update(view);

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
  const auto &[skeleton, _, _2] = createSkeleton(3);

  EXPECT_EQ(skeleton.jointFinalTransforms.at(0), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.jointFinalTransforms.at(1), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.jointFinalTransforms.at(2), glm::mat4(1.0f));

  skeletonUpdater.update(view);

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
  const auto &[skeleton, skeletonDebug, _2] = createSkeleton(2);

  for (auto &transform : skeletonDebug.boneTransforms) {
    EXPECT_EQ(transform, glm::mat4{1.0f});
  }

  skeletonUpdater.update(view);

  for (usize i = 0; i < skeletonDebug.bones.size(); ++i) {
    EXPECT_EQ(skeletonDebug.boneTransforms.at(i),
              skeleton.jointWorldTransforms.at(skeletonDebug.bones.at(i)));
  }
}

TEST_F(SkeletonUpdaterDeathTest,
       FailsIfDebugBoneSizeIsNotTwiceTheNumberOfJoints) {
  const auto &[skeleton, _, entity] = createSkeleton(2);

  entity.set<quoll::SkeletonDebug>({});

  EXPECT_DEATH(skeletonUpdater.update(view), ".*");
}
