#include "liquid/core/Base.h"
#include "Skeleton.h"

namespace liquid {

Skeleton::Skeleton(std::vector<glm::vec3> &&positions,
                   std::vector<glm::quat> &&rotations,
                   std::vector<glm::vec3> &&scales,
                   std::vector<JointId> &&parents,
                   std::vector<glm::mat4> &&inverseBindMatrices,
                   std::vector<String> &&names,
                   rhi::ResourceRegistry *registry_)
    : jointLocalPositions(positions), jointLocalRotations(rotations),
      jointLocalScales(scales), jointParents(parents),
      jointInverseBindMatrices(inverseBindMatrices), jointNames(names),
      numJoints(positions.size()), registry(registry_) {
  LIQUID_ASSERT(numJoints > 0, "No joints provided");
  LIQUID_ASSERT(jointLocalPositions.size() == numJoints &&
                    jointLocalRotations.size() == numJoints &&
                    jointLocalScales.size() == numJoints &&
                    jointParents.size() == numJoints &&
                    jointInverseBindMatrices.size() == numJoints &&
                    jointNames.size() == numJoints,
                "All joint parameter arrays must have the same size");

  jointWorldTransforms.resize(numJoints, glm::mat4{1.0f});
  jointFinalTransforms.resize(numJoints, glm::mat4{1.0f});

  buffer = registry->addBuffer(
      {rhi::BufferType::Uniform, sizeof(glm::mat4) * numJoints});

  // Debug data
  for (JointId joint = 0; joint < static_cast<uint32_t>(numJoints); ++joint) {
    debugBones.push_back(jointParents.at(joint));
    debugBones.push_back(joint);
  }

  const size_t numDebugBones = numJoints * 2;

  debugBoneTransforms.resize(numDebugBones, glm::mat4{1.0f});

  debugBuffer = registry->addBuffer(
      {rhi::BufferType::Uniform, sizeof(glm::mat4) * numDebugBones});
}

const glm::mat4 Skeleton::getJointLocalTransform(JointId joint) const {
  LIQUID_ASSERT(joint < numJoints, "Joint does not exist");

  glm::mat4 identity{1.0f};
  return glm::translate(identity, jointLocalPositions.at(joint)) *
         glm::toMat4(jointLocalRotations.at(joint)) *
         glm::scale(identity, jointLocalScales.at(joint));
}

void Skeleton::setJointPosition(JointId joint, const glm::vec3 &position) {
  LIQUID_ASSERT(joint < numJoints, "Joint does not exist");

  jointLocalPositions.at(joint) = position;
}

void Skeleton::setJointRotation(JointId joint, const glm::quat &rotation) {
  LIQUID_ASSERT(joint < numJoints, "Joint does not exist");

  jointLocalRotations.at(joint) = rotation;
}

void Skeleton::setJointScale(JointId joint, const glm::vec3 &scale) {
  LIQUID_ASSERT(joint < numJoints, "Joint does not exist");

  jointLocalScales.at(joint) = scale;
}

void Skeleton::update() {
  LIQUID_PROFILE_EVENT("Skeleton::update");

  for (uint32_t i = 0; i < static_cast<uint32_t>(numJoints); ++i) {
    const auto &parentWorld = jointWorldTransforms.at(jointParents.at(i));
    jointWorldTransforms.at(i) = parentWorld * getJointLocalTransform(i);
  }

  for (size_t i = 0; i < numJoints; ++i) {
    jointFinalTransforms.at(i) =
        jointWorldTransforms.at(i) * jointInverseBindMatrices.at(i);
  }

  registry->updateBuffer(buffer, {rhi::BufferType::Uniform,
                                  sizeof(glm::mat4) * numJoints,
                                  jointFinalTransforms.data()});
}

void Skeleton::updateDebug() {
  LIQUID_PROFILE_EVENT("Skeleton::updateDebug");

  for (size_t i = 0; i < debugBones.size(); ++i) {
    debugBoneTransforms.at(i) = jointWorldTransforms.at(debugBones.at(i));
  }

  registry->updateBuffer(debugBuffer, {rhi::BufferType::Uniform,
                                       sizeof(glm::mat4) * debugBones.size(),
                                       debugBoneTransforms.data()});
}

} // namespace liquid
