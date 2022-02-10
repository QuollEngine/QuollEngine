#include "liquid/core/Base.h"
#include "Skeleton.h"

namespace liquid {

Skeleton::Skeleton(uint32_t numJoints_, ResourceAllocator *resourseAllocator)
    : numJoints(numJoints_) {
  LIQUID_ASSERT(numJoints > 0, "Number of joints cannot be ZERO");
  jointLocalPositions.reserve(numJoints);
  jointLocalRotations.reserve(numJoints);
  jointLocalScales.reserve(numJoints);
  jointWorldTransforms.reserve(numJoints);
  jointInverseBindMatrices.reserve(numJoints);
  jointParents.reserve(numJoints);
  jointNames.reserve(numJoints);
  buffer =
      resourseAllocator->createUniformBuffer(sizeof(glm::mat4) * numJoints);
}

void Skeleton::addJoint(const glm::vec3 &position, const glm::quat &rotation,
                        const glm::vec3 scale, JointId parentId,
                        const glm::mat4 &inverseBindMatrix,
                        const String &name) {
  LIQUID_ASSERT(jointWorldTransforms.size() < numJoints,
                "Maximum number of joints reached");
  LIQUID_ASSERT(parentId < jointParents.size(), "Parent does not exist");
  jointLocalPositions.push_back(position);
  jointLocalRotations.push_back(rotation);
  jointLocalScales.push_back(scale);
  jointWorldTransforms.push_back(glm::mat4{1.0f});
  jointInverseBindMatrices.push_back(inverseBindMatrix);
  jointParents.push_back(parentId);
  jointNames.push_back(name);
}

const glm::mat4 Skeleton::getJointLocalTransform(JointId joint) const {
  LIQUID_ASSERT(joint < jointWorldTransforms.size(), "Joint does not exist");

  glm::mat4 identity{1.0f};
  return glm::translate(identity, jointLocalPositions.at(joint)) *
         glm::toMat4(jointLocalRotations.at(joint)) *
         glm::scale(identity, jointLocalScales.at(joint));
}

void Skeleton::setJointPosition(JointId joint, const glm::vec3 &position) {
  LIQUID_ASSERT(joint < jointLocalPositions.size(), "Joint does not exist");

  jointLocalPositions.at(joint) = position;
}

void Skeleton::setJointRotation(JointId joint, const glm::quat &rotation) {
  LIQUID_ASSERT(joint < jointLocalRotations.size(), "Joint does not exist");

  jointLocalRotations.at(joint) = rotation;
}

void Skeleton::setJointScale(JointId joint, const glm::vec3 &scale) {
  LIQUID_ASSERT(joint < jointLocalScales.size(), "Joint does not exist");

  jointLocalScales.at(joint) = scale;
}

void Skeleton::update() {
  LIQUID_PROFILE_EVENT("Skeleton::update");
  // Starting from index 1 because
  // root does not need to be updated
  for (size_t i = 1; i < jointLocalPositions.size(); ++i) {
    const auto &parentWorld = jointWorldTransforms.at(jointParents.at(i));
    jointWorldTransforms.at(i) = parentWorld * getJointLocalTransform(i);
  }

  for (size_t i = 1; i < jointLocalPositions.size(); ++i) {
    jointWorldTransforms.at(i) *= jointInverseBindMatrices.at(i);
  }

  buffer->update(jointWorldTransforms.data());
}

} // namespace liquid
