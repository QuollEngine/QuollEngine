#include "quoll/core/Base.h"
#include "quoll/core/Profiler.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/system/SystemView.h"
#include "Skeleton.h"
#include "SkeletonUpdater.h"

namespace quoll {

namespace {

void updateSkeletonAssets(SystemView &view) {
  auto &entityDatabase = view.scene->entityDatabase;
  for (auto [entity, ref] : entityDatabase.view<SkeletonAssetRef>()) {
    if (entityDatabase.has<SkeletonCurrentAsset>(entity) &&
        entityDatabase.get<SkeletonCurrentAsset>(entity).handle ==
            ref.asset.handle()) {
      continue;
    }

    if (!ref.asset) {
      continue;
    }

    const auto &asset = ref.asset.get();
    Skeleton skeleton{};
    skeleton.jointLocalPositions = asset.jointLocalPositions;
    skeleton.jointLocalRotations = asset.jointLocalRotations;
    skeleton.jointLocalScales = asset.jointLocalScales;
    skeleton.jointParents = asset.jointParents;
    skeleton.jointInverseBindMatrices = asset.jointInverseBindMatrices;
    skeleton.jointNames = asset.jointNames;
    skeleton.numJoints = static_cast<u32>(asset.jointLocalPositions.size());
    skeleton.jointFinalTransforms.resize(skeleton.numJoints, glm::mat4{1.0f});
    skeleton.jointWorldTransforms.resize(skeleton.numJoints, glm::mat4{1.0f});

    entityDatabase.set(entity, skeleton);
    entityDatabase.set(entity, SkeletonCurrentAsset(ref.asset.handle()));
  }
}

void updateSkeletons(SystemView &view) {
  QUOLL_PROFILE_EVENT("SkeletonUpdater::update");

  auto &entityDatabase = view.scene->entityDatabase;
  for (auto [entity, skeleton] : entityDatabase.view<Skeleton>()) {

    {
      const glm::mat4 identity{1.0f};
      skeleton.jointWorldTransforms.at(0) =
          glm::translate(identity, skeleton.jointLocalPositions.at(0)) *
          glm::toMat4(skeleton.jointLocalRotations.at(0)) *
          glm::scale(identity, skeleton.jointLocalScales.at(0));
    }

    for (u32 i = 1; i < skeleton.numJoints; ++i) {
      const glm::mat4 identity{1.0f};
      auto localTransform =
          glm::translate(identity, skeleton.jointLocalPositions.at(i)) *
          glm::toMat4(skeleton.jointLocalRotations.at(i)) *
          glm::scale(identity, skeleton.jointLocalScales.at(i));

      const auto &parentWorld =
          skeleton.jointWorldTransforms.at(skeleton.jointParents.at(i));
      skeleton.jointWorldTransforms.at(i) = parentWorld * localTransform;
    }

    for (usize i = 0; i < skeleton.numJoints; ++i) {
      skeleton.jointFinalTransforms.at(i) =
          skeleton.jointWorldTransforms.at(i) *
          skeleton.jointInverseBindMatrices.at(i);
    }
  }
}

void updateDebugBones(SystemView &view) {
  QUOLL_PROFILE_EVENT("SkeletonUpdater::updateDebug");

  auto &entityDatabase = view.scene->entityDatabase;
  for (auto [entity, skeleton, debug] :
       entityDatabase.view<Skeleton, SkeletonDebug>()) {
    QuollAssert(static_cast<u32>(debug.bones.size()) == skeleton.numJoints * 2,
                "Debug bones must be twice the size skeleton joint size");

    for (usize i = 0; i < debug.bones.size(); ++i) {
      debug.boneTransforms.at(i) =
          skeleton.jointWorldTransforms.at(debug.bones.at(i));
    }
  }
}

} // namespace

void SkeletonUpdater::update(SystemView &view) {
  QUOLL_PROFILE_EVENT("SkeletonUpdater::update");

  updateSkeletonAssets(view);
  updateSkeletons(view);
  updateDebugBones(view);
}

} // namespace quoll
