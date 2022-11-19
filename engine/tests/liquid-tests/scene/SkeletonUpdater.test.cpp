#include "liquid/core/Base.h"
#include "liquid/scene/SkeletonUpdater.h"

#include "liquid-tests/Testing.h"

struct SkeletonUpdaterTest : public ::testing::Test {
  liquid::SkeletonAssetHandle handle{2};
  liquid::EntityDatabase entityDatabase;
  liquid::SkeletonUpdater skeletonUpdater;

  std::tuple<liquid::Skeleton &, liquid::SkeletonDebug &, liquid::Entity>
  createSkeleton(uint32_t numJoints) {
    auto entity = entityDatabase.create();

    liquid::Skeleton skeleton;

    skeleton.jointWorldTransforms.resize(numJoints, glm::mat4{1.0f});
    skeleton.jointFinalTransforms.resize(numJoints, glm::mat4{1.0f});
    skeleton.numJoints = numJoints;

    for (uint32_t i = 0; i < numJoints; ++i) {
      float value = static_cast<float>(i + 1);
      skeleton.jointLocalPositions.push_back(glm::vec3(value));
      skeleton.jointLocalRotations.push_back(
          glm::quat(value, value, value, value));
      skeleton.jointLocalScales.push_back(glm::vec3(value));
      skeleton.jointParents.push_back(i > 0 ? i - 1 : 0);
      skeleton.jointInverseBindMatrices.push_back(glm::mat4(value));
      skeleton.jointNames.push_back("Joint " + std::to_string(i));
    }

    entityDatabase.set(entity, skeleton);

    liquid::SkeletonDebug skeletonDebug{};
    auto numBones = skeleton.numJoints * 2;
    skeletonDebug.bones.reserve(numBones);

    for (uint32_t joint = 0; joint < skeleton.numJoints; ++joint) {
      skeletonDebug.bones.push_back(skeleton.jointParents.at(joint));
      skeletonDebug.bones.push_back(joint);
    }

    skeletonDebug.boneTransforms.resize(numBones, glm::mat4{1.0f});

    entityDatabase.set(entity, skeletonDebug);

    return {getSkeleton(entity), getDebugSkeleton(entity), entity};
  }

  liquid::Skeleton &getSkeleton(liquid::Entity entity) {
    return entityDatabase.get<liquid::Skeleton>(entity);
  }

  liquid::SkeletonDebug &getDebugSkeleton(liquid::Entity entity) {
    return entityDatabase.get<liquid::SkeletonDebug>(entity);
  }

  glm::mat4 getLocalTransform(liquid::Skeleton &skeleton, uint32_t i) {
    glm::mat4 identity{1.0f};
    return glm::translate(identity, skeleton.jointLocalPositions.at(i)) *
           glm::toMat4(skeleton.jointLocalRotations.at(i)) *
           glm::scale(identity, skeleton.jointLocalScales.at(i));
  };

  template <class T> std::vector<T> createItems(uint32_t numJoints) {
    return std::vector<T>(numJoints);
  }
};

using SkeletonUpdaterDeathTest = SkeletonUpdaterTest;

TEST_F(SkeletonUpdaterTest, UpdatesWorldTransformsOnUpdate) {
  const auto &[skeleton, _, _2] = createSkeleton(3);

  EXPECT_EQ(skeleton.jointWorldTransforms.at(0), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.jointWorldTransforms.at(1), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.jointWorldTransforms.at(2), glm::mat4(1.0f));

  skeletonUpdater.update(entityDatabase);

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

  skeletonUpdater.update(entityDatabase);

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

  skeletonUpdater.update(entityDatabase);

  for (size_t i = 0; i < skeletonDebug.bones.size(); ++i) {
    EXPECT_EQ(skeletonDebug.boneTransforms.at(i),
              skeleton.jointWorldTransforms.at(skeletonDebug.bones.at(i)));
  }
}

TEST_F(SkeletonUpdaterDeathTest,
       FailsIfDebugBoneSizeIsNotTwiceTheNumberOfJoints) {
  const auto &[skeleton, _, entity] = createSkeleton(2);

  entityDatabase.set<liquid::SkeletonDebug>(entity, {});

  EXPECT_DEATH(skeletonUpdater.update(entityDatabase), ".*");
}
