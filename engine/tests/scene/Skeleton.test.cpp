#include "liquid/core/Base.h"
#include "liquid/scene/Skeleton.h"

#include <gtest/gtest.h>
#include <glm/gtx/string_cast.hpp>

struct SkeletonTest : public ::testing::Test {
  liquid::rhi::ResourceRegistry registry;
  liquid::SkeletonAssetHandle handle{2};

  liquid::Skeleton createSkeleton(uint32_t numJoints) {
    std::vector<glm::vec3> positions;
    std::vector<glm::quat> rotations;
    std::vector<glm::vec3> scales;
    std::vector<liquid::JointId> parents;
    std::vector<glm::mat4> inverseBindMatrices;
    std::vector<liquid::String> names;

    for (uint32_t i = 0; i < numJoints; ++i) {
      float value = static_cast<float>(i + 1);
      positions.push_back(glm::vec3(value));
      rotations.push_back(glm::quat(value, value, value, value));
      scales.push_back(glm::vec3(value));
      parents.push_back(i > 0 ? i - 1 : 0);
      inverseBindMatrices.push_back(glm::mat4(value));
      names.push_back("Joint " + std::to_string(i));
    }

    return liquid::Skeleton(handle, std::move(positions), std::move(rotations),
                            std::move(scales), std::move(parents),
                            std::move(inverseBindMatrices), std::move(names));
  }

  template <class T> std::vector<T> createItems(uint32_t numJoints) {
    return std::vector<T>(numJoints);
  }
};

using SkeletonDeathTest = SkeletonTest;

TEST_F(SkeletonDeathTest, FailsIfNumberOfJointsIsZeroOnConstruct) {
  EXPECT_DEATH(createSkeleton(0), ".*");
}

TEST_F(SkeletonDeathTest, FailsIfSizeMismatchBetweenJointParameters) {
  EXPECT_DEATH(
      {
        liquid::Skeleton skeleton(
            handle, createItems<glm::vec3>(2), createItems<glm::quat>(1),
            createItems<glm::vec3>(1), createItems<liquid::JointId>(1),
            createItems<glm::mat4>(1), createItems<liquid::String>(1));
      },
      ".*");
  EXPECT_DEATH(
      {
        liquid::Skeleton skeleton(
            handle, createItems<glm::vec3>(1), createItems<glm::quat>(2),
            createItems<glm::vec3>(1), createItems<liquid::JointId>(1),
            createItems<glm::mat4>(1), createItems<liquid::String>(1));
      },
      ".*");
  EXPECT_DEATH(
      {
        liquid::Skeleton skeleton(
            handle, createItems<glm::vec3>(1), createItems<glm::quat>(1),
            createItems<glm::vec3>(2), createItems<liquid::JointId>(1),
            createItems<glm::mat4>(1), createItems<liquid::String>(1));
      },
      ".*");
  EXPECT_DEATH(
      {
        liquid::Skeleton skeleton(
            handle, createItems<glm::vec3>(1), createItems<glm::quat>(1),
            createItems<glm::vec3>(1), createItems<liquid::JointId>(2),
            createItems<glm::mat4>(1), createItems<liquid::String>(1));
      },
      ".*");
  EXPECT_DEATH(
      {
        liquid::Skeleton skeleton(
            handle, createItems<glm::vec3>(1), createItems<glm::quat>(1),
            createItems<glm::vec3>(1), createItems<liquid::JointId>(1),
            createItems<glm::mat4>(2), createItems<liquid::String>(1));
      },
      ".*");
  EXPECT_DEATH(
      {
        liquid::Skeleton skeleton(
            handle, createItems<glm::vec3>(1), createItems<glm::quat>(2),
            createItems<glm::vec3>(1), createItems<liquid::JointId>(1),
            createItems<glm::mat4>(1), createItems<liquid::String>(2));
      },
      ".*");
}

TEST_F(SkeletonTest, CreatesAllJointPrametersOnConstruct) {
  auto &&skeleton = createSkeleton(2);
  EXPECT_EQ(skeleton.getNumJoints(), 2);

  EXPECT_EQ(skeleton.getJointName(0), "Joint 0");
  EXPECT_EQ(skeleton.getJointParent(0), 0);
  EXPECT_EQ(skeleton.getLocalPosition(0), glm::vec3(1.0f));
  EXPECT_EQ(skeleton.getLocalRotation(0), glm::quat(1.0f, 1.0f, 1.0f, 1.0f));
  EXPECT_EQ(skeleton.getLocalScale(0), glm::vec3(1.0f));
  EXPECT_EQ(skeleton.getJointInverseBindMatrix(0), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.getJointWorldTransform(0), glm::mat4(1.0f));

  EXPECT_EQ(skeleton.getJointName(1), "Joint 1");
  EXPECT_EQ(skeleton.getJointParent(1), 0);
  EXPECT_EQ(skeleton.getLocalPosition(1), glm::vec3(2.0f));
  EXPECT_EQ(skeleton.getLocalRotation(1), glm::quat(2.0f, 2.0f, 2.0f, 2.0f));
  EXPECT_EQ(skeleton.getLocalScale(1), glm::vec3(2.0f));
  EXPECT_EQ(skeleton.getJointInverseBindMatrix(1), glm::mat4(2.0f));
  EXPECT_EQ(skeleton.getJointWorldTransform(1), glm::mat4(1.0f));
}

TEST_F(SkeletonTest, CreatesDebugParametersOnConstruct) {
  auto &&skeleton = createSkeleton(2);
  EXPECT_EQ(skeleton.getNumDebugBones(), 4);
}

TEST_F(SkeletonTest, UpdatesWorldTransformsOnUpdate) {
  auto &&skeleton = createSkeleton(3);

  EXPECT_EQ(skeleton.getJointWorldTransform(0), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.getJointWorldTransform(1), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.getJointWorldTransform(2), glm::mat4(1.0f));

  skeleton.update();

  EXPECT_EQ(skeleton.getJointWorldTransform(0),
            skeleton.getJointLocalTransform(0));
  EXPECT_EQ(skeleton.getJointWorldTransform(1),
            skeleton.getJointWorldTransform(0) *
                skeleton.getJointLocalTransform(1));
  EXPECT_EQ(skeleton.getJointWorldTransform(2),
            skeleton.getJointWorldTransform(1) *
                skeleton.getJointLocalTransform(2));
}

TEST_F(SkeletonDeathTest, FailsIfNonExistentJointTransformIsRequested) {
  auto &&skeleton = createSkeleton(1);
  EXPECT_DEATH({ skeleton.getJointLocalTransform(2); }, ".*");
}

TEST_F(SkeletonDeathTest, FailsIfNonExistentJointInverseBindMatrixIsRequested) {
  auto &&skeleton = createSkeleton(1);
  EXPECT_DEATH({ skeleton.getJointInverseBindMatrix(2); }, ".*");
}

TEST_F(SkeletonDeathTest, FailsIfNonExistentJointParentIsRequested) {
  auto &&skeleton = createSkeleton(1);
  EXPECT_DEATH({ skeleton.getJointParent(2); }, ".*");
}

TEST_F(SkeletonDeathTest, FailsIfNonExistentJointNameIsRequested) {
  auto &&skeleton = createSkeleton(1);
  EXPECT_DEATH({ skeleton.getJointName(2); }, ".*");
}
