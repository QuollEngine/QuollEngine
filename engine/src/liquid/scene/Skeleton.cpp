#include "liquid/core/Base.h"
#include "Skeleton.h"

namespace liquid {

Skeleton::Skeleton(uint32_t numJoints_, ResourceAllocator *resourseAllocator)
    : numJoints(numJoints_) {
  LIQUID_ASSERT(numJoints > 0, "Number of joints cannot be ZERO");
  jointLocalTransforms.reserve(numJoints);
  jointWorldTransforms.reserve(numJoints);
  jointInverseBindMatrices.reserve(numJoints);
  jointParents.reserve(numJoints);
  jointNames.reserve(numJoints);
  buffer =
      resourseAllocator->createUniformBuffer(sizeof(glm::mat4) * numJoints);
}

void Skeleton::addJoint(const glm::mat4 &transform, JointId parentId,
                        const glm::mat4 &inverseBindMatrix,
                        const String &name) {
  LIQUID_ASSERT(jointLocalTransforms.size() < numJoints,
                "Maximum number of joints reached");
  LIQUID_ASSERT(parentId < jointParents.size(), "Parent does not exist");
  jointLocalTransforms.push_back(transform);
  jointWorldTransforms.push_back(glm::mat4{1.0f});
  jointInverseBindMatrices.push_back(inverseBindMatrix);
  jointParents.push_back(parentId);
  jointNames.push_back(name);
}

void Skeleton::update() {
  LIQUID_PROFILE_EVENT("Skeleton::update");
  // Starting from index 1 because
  // root does not need to by updated
  for (size_t i = 1; i < jointLocalTransforms.size(); ++i) {
    const auto &parentWorld = jointWorldTransforms.at(jointParents.at(i));
    jointWorldTransforms.at(i) = parentWorld * jointLocalTransforms.at(i);
  }

  for (size_t i = 1; i < jointLocalTransforms.size(); ++i) {
    jointWorldTransforms.at(i) *= jointInverseBindMatrices.at(i);
  }

  buffer->update(jointWorldTransforms.data());
}

} // namespace liquid
