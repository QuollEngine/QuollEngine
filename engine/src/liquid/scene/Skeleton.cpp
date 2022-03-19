#include "liquid/core/Base.h"
#include "Skeleton.h"

namespace liquid {

Skeleton::Skeleton(std::vector<glm::vec3> &&positions,
                   std::vector<glm::quat> &&rotations,
                   std::vector<glm::vec3> &&scales,
                   std::vector<JointId> &&parents,
                   std::vector<glm::mat4> &&inverseBindMatrices,
                   std::vector<String> &&names, rhi::ResourceRegistry *registry)
    : mJointLocalPositions(positions), mJointLocalRotations(rotations),
      mJointLocalScales(scales), mJointParents(parents),
      mJointInverseBindMatrices(inverseBindMatrices), mJointNames(names),
      mNumJoints(positions.size()), mRegistry(registry) {
  LIQUID_ASSERT(mNumJoints > 0, "No joints provided");
  LIQUID_ASSERT(mJointLocalPositions.size() == mNumJoints &&
                    mJointLocalRotations.size() == mNumJoints &&
                    mJointLocalScales.size() == mNumJoints &&
                    mJointParents.size() == mNumJoints &&
                    mJointInverseBindMatrices.size() == mNumJoints &&
                    mJointNames.size() == mNumJoints,
                "All joint parameter arrays must have the same size");

  mJointWorldTransforms.resize(mNumJoints, glm::mat4{1.0f});
  mJointFinalTransforms.resize(mNumJoints, glm::mat4{1.0f});

  mBuffer = registry->setBuffer(
      {rhi::BufferType::Uniform, sizeof(glm::mat4) * mNumJoints});

  // Debug data
  for (JointId joint = 0; joint < static_cast<uint32_t>(mNumJoints); ++joint) {
    mDebugBones.push_back(mJointParents.at(joint));
    mDebugBones.push_back(joint);
  }

  const size_t numDebugBones = mNumJoints * 2;

  mDebugBoneTransforms.resize(numDebugBones, glm::mat4{1.0f});

  mDebugBuffer = registry->setBuffer(
      {rhi::BufferType::Uniform, sizeof(glm::mat4) * numDebugBones});
}

const glm::mat4 Skeleton::getJointLocalTransform(JointId joint) const {
  LIQUID_ASSERT(joint < mNumJoints, "Joint does not exist");

  glm::mat4 identity{1.0f};
  return glm::translate(identity, mJointLocalPositions.at(joint)) *
         glm::toMat4(mJointLocalRotations.at(joint)) *
         glm::scale(identity, mJointLocalScales.at(joint));
}

void Skeleton::setJointPosition(JointId joint, const glm::vec3 &position) {
  LIQUID_ASSERT(joint < mNumJoints, "Joint does not exist");

  mJointLocalPositions.at(joint) = position;
}

void Skeleton::setJointRotation(JointId joint, const glm::quat &rotation) {
  LIQUID_ASSERT(joint < mNumJoints, "Joint does not exist");

  mJointLocalRotations.at(joint) = rotation;
}

void Skeleton::setJointScale(JointId joint, const glm::vec3 &scale) {
  LIQUID_ASSERT(joint < mNumJoints, "Joint does not exist");

  mJointLocalScales.at(joint) = scale;
}

void Skeleton::update() {
  LIQUID_PROFILE_EVENT("Skeleton::update");

  for (uint32_t i = 0; i < static_cast<uint32_t>(mNumJoints); ++i) {
    const auto &parentWorld = mJointWorldTransforms.at(mJointParents.at(i));
    mJointWorldTransforms.at(i) = parentWorld * getJointLocalTransform(i);
  }

  for (size_t i = 0; i < mNumJoints; ++i) {
    mJointFinalTransforms.at(i) =
        mJointWorldTransforms.at(i) * mJointInverseBindMatrices.at(i);
  }

  mRegistry->setBuffer({rhi::BufferType::Uniform,
                        sizeof(glm::mat4) * mNumJoints,
                        mJointFinalTransforms.data()},
                       mBuffer);
}

void Skeleton::updateDebug() {
  LIQUID_PROFILE_EVENT("Skeleton::updateDebug");

  for (size_t i = 0; i < mDebugBones.size(); ++i) {
    mDebugBoneTransforms.at(i) = mJointWorldTransforms.at(mDebugBones.at(i));
  }

  mRegistry->setBuffer({rhi::BufferType::Uniform,
                        sizeof(glm::mat4) * mDebugBones.size(),
                        mDebugBoneTransforms.data()},
                       mDebugBuffer);
}

} // namespace liquid
