#include "liquid/core/Base.h"
#include "SkeletonUpdater.h"

namespace liquid {

void SkeletonUpdater::update(EntityContext &entityContext) {
  {
    LIQUID_PROFILE_EVENT("SkeletonUpdater::update");
    entityContext.iterateEntities<SkeletonComponent>(
        [](auto entity, SkeletonComponent &skeleton) {
          for (uint32_t i = 0; i < skeleton.numJoints; ++i) {
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
        });
  }

  {
    LIQUID_PROFILE_EVENT("SkeletonUpdater::updateDebug");
    entityContext.iterateEntities<SkeletonComponent, SkeletonDebugComponent>(
        [](auto entity, SkeletonComponent &skeleton,
           SkeletonDebugComponent &debug) {
          LIQUID_ASSERT(
              static_cast<uint32_t>(debug.bones.size()) ==
                  skeleton.numJoints * 2,
              "Debug bones must be twice the size skeleton joint size");

          for (size_t i = 0; i < debug.bones.size(); ++i) {
            debug.boneTransforms.at(i) =
                skeleton.jointWorldTransforms.at(debug.bones.at(i));
          }
        });
  }
}

} // namespace liquid
