#include "quoll/core/Base.h"
#include "SkeletonUpdater.h"

namespace quoll {

void SkeletonUpdater::update(EntityDatabase &entityDatabase) {
  QUOLL_PROFILE_EVENT("SkeletonUpdater::update");

  updateSkeletons(entityDatabase);
  updateDebugBones(entityDatabase);
}

void SkeletonUpdater::updateSkeletons(EntityDatabase &entityDatabase) {
  QUOLL_PROFILE_EVENT("SkeletonUpdater::update");
  for (auto [entity, skeleton] : entityDatabase.view<Skeleton>()) {

    {
      glm::mat4 identity{1.0f};
      skeleton.jointWorldTransforms.at(0) =
          glm::translate(identity, skeleton.jointLocalPositions.at(0)) *
          glm::toMat4(skeleton.jointLocalRotations.at(0)) *
          glm::scale(identity, skeleton.jointLocalScales.at(0));
    }

    for (uint32_t i = 1; i < skeleton.numJoints; ++i) {
      glm::mat4 identity{1.0f};
      auto localTransform =
          glm::translate(identity, skeleton.jointLocalPositions.at(i)) *
          glm::toMat4(skeleton.jointLocalRotations.at(i)) *
          glm::scale(identity, skeleton.jointLocalScales.at(i));

      const auto &parentWorld =
          skeleton.jointWorldTransforms.at(skeleton.jointParents.at(i));
      skeleton.jointWorldTransforms.at(i) = parentWorld * localTransform;
    }

    for (size_t i = 0; i < skeleton.numJoints; ++i) {
      skeleton.jointFinalTransforms.at(i) =
          skeleton.jointWorldTransforms.at(i) *
          skeleton.jointInverseBindMatrices.at(i);
    }
  }
}

void SkeletonUpdater::updateDebugBones(EntityDatabase &entityDatabase) {
  QUOLL_PROFILE_EVENT("SkeletonUpdater::updateDebug");
  for (auto [entity, skeleton, debug] :
       entityDatabase.view<Skeleton, SkeletonDebug>()) {
    QuollAssert(static_cast<uint32_t>(debug.bones.size()) ==
                    skeleton.numJoints * 2,
                "Debug bones must be twice the size skeleton joint size");

    for (size_t i = 0; i < debug.bones.size(); ++i) {
      debug.boneTransforms.at(i) =
          skeleton.jointWorldTransforms.at(debug.bones.at(i));
    }
  }
}

} // namespace quoll
