#include "liquid/core/Base.h"
#include "liquid/scene/Skeleton.h"

#include "../mocks/TestResourceAllocator.h"
#include <gtest/gtest.h>

struct SkeletonTest : public ::testing::Test {
  TestResourceAllocator allocator;
};

using SkeletonDeathTest = SkeletonTest;

TEST_F(SkeletonDeathTest, FailsIfNumberOfJointsIsZeroOnConstruct) {
  EXPECT_DEATH({ liquid::Skeleton skeleton(0, &allocator); }, ".*");
}

TEST_F(SkeletonTest, CreatesRootJointOnConstruct) {
  liquid::Skeleton skeleton(1, &allocator);
  EXPECT_EQ(skeleton.getJointLocalTransform(0), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.getJointParent(0), 0);
  EXPECT_EQ(skeleton.getJointLocalTransform(0), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.getJointInverseBindMatrix(0), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.getJointWorldTransform(0), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.getJointName(0), "__rootJoint");
}

TEST_F(SkeletonTest, CreatesUniformBufferOnConstruct) {
  liquid::Skeleton skeleton(5, &allocator);

  EXPECT_EQ(skeleton.getBuffer()->getType(), liquid::HardwareBuffer::Uniform);
  EXPECT_EQ(skeleton.getBuffer()->getBufferSize(), sizeof(glm::mat4) * 5);
}

TEST_F(SkeletonTest, AddsJointWithTransformParentInverseBindAndName) {
  liquid::Skeleton skeleton(3, &allocator);

  skeleton.addJoint(glm::vec3{1.4f}, glm::quat{1.0f, 2.0f, 0.0f, 0.0f},
                    glm::vec3{1.2f}, 0, glm::mat4{2.0f}, "B0");
  skeleton.addJoint(glm::vec3{1.2f}, glm::quat{1.0f, 3.0f, 0.0f, 0.0f},
                    glm::vec3{1.5f}, 1, glm::mat4{2.5f}, "B1");

  EXPECT_EQ(skeleton.getLocalPosition(1), glm::vec3(1.4f));
  EXPECT_EQ(skeleton.getLocalRotation(1), glm::quat(1.0f, 2.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.getLocalScale(1), glm::vec3(1.2f));
  EXPECT_EQ(skeleton.getJointParent(1), 0);
  EXPECT_EQ(skeleton.getJointInverseBindMatrix(1), glm::mat4(2.0f));
  EXPECT_EQ(skeleton.getJointName(1), "B0");
  EXPECT_EQ(skeleton.getLocalPosition(2), glm::vec3(1.2f));
  EXPECT_EQ(skeleton.getLocalRotation(2), glm::quat(1.0f, 3.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.getLocalScale(2), glm::vec3(1.5f));
  EXPECT_EQ(skeleton.getJointParent(2), 1);
  EXPECT_EQ(skeleton.getJointInverseBindMatrix(2), glm::mat4(2.5f));
  EXPECT_EQ(skeleton.getJointName(2), "B1");
}

TEST_F(SkeletonTest, UpdatesWorldTransformsOnUpdate) {
  liquid::Skeleton skeleton(3, &allocator);

  skeleton.addJoint(glm::vec3{0.0f}, glm::quat{1.0f, 0.0f, 0.0f, 0.0f},
                    glm::vec3{1.2f}, 0, glm::mat4{2.0f}, "B0");
  skeleton.addJoint(glm::vec3{1.2f}, glm::quat{1.0f, 0.0f, 0.0f, 0.0f},
                    glm::vec3{1.5f}, 1, glm::mat4{2.5f}, "B1");

  EXPECT_EQ(skeleton.getJointWorldTransform(0), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.getJointWorldTransform(1), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.getJointWorldTransform(2), glm::mat4(1.0f));

  skeleton.update();

  EXPECT_EQ(skeleton.getJointWorldTransform(0), glm::mat4(1.0f));
  EXPECT_EQ(skeleton.getJointWorldTransform(1),
            skeleton.getJointLocalTransform(1) * glm::mat4(2.0f));
  EXPECT_EQ(skeleton.getJointWorldTransform(2),
            skeleton.getJointLocalTransform(1) *
                skeleton.getJointLocalTransform(2) * glm::mat4(2.5f));
}

TEST_F(SkeletonDeathTest, FailsAddingJointIfMaximumIsReached) {
  liquid::Skeleton skeleton(1, &allocator);
  EXPECT_DEATH(skeleton.addJoint(glm::vec3(), glm::quat(), glm::vec3(), 0,
                                 glm::mat4(1.0f)),
               ".*");
}

TEST_F(SkeletonDeathTest, FailsAddingJointIfParentDoesNotExist) {
  liquid::Skeleton skeleton(5, &allocator);
  EXPECT_DEATH(
      {
        skeleton.addJoint(glm::vec3(), glm::quat(), glm::vec3(), 121,
                          glm::mat4(1.0f));
      },
      ".*");
}

TEST_F(SkeletonDeathTest, FailsIfNonExistentJointTransformIsRequested) {
  liquid::Skeleton skeleton(5, &allocator);
  EXPECT_DEATH({ skeleton.getJointLocalTransform(2); }, ".*");
}

TEST_F(SkeletonDeathTest, FailsIfNonExistentJointInverseBindMatrixIsRequested) {
  liquid::Skeleton skeleton(5, &allocator);
  EXPECT_DEATH({ skeleton.getJointInverseBindMatrix(2); }, ".*");
}

TEST_F(SkeletonDeathTest, FailsIfNonExistentJointParentIsRequested) {
  liquid::Skeleton skeleton(5, &allocator);
  EXPECT_DEATH({ skeleton.getJointParent(2); }, ".*");
}

TEST_F(SkeletonDeathTest, FailsIfNonExistentJointNameIsRequested) {
  liquid::Skeleton skeleton(5, &allocator);
  EXPECT_DEATH({ skeleton.getJointName(2); }, ".*");
}
