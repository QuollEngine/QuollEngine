#include "quoll/core/Base.h"
#include "Skeleton.h"
#include "SkeletonSerializer.h"

namespace quoll {

void SkeletonSerializer::serialize(YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity,
                                   AssetRegistry &assetRegistry) {
  if (entityDatabase.has<Skeleton>(entity)) {
    auto handle = entityDatabase.get<Skeleton>(entity).assetHandle;
    if (assetRegistry.has(handle)) {
      auto uuid = assetRegistry.getMeta(handle).uuid;

      node["skeleton"] = uuid;
    }
  }
}

void SkeletonSerializer::deserialize(const YAML::Node &node,
                                     EntityDatabase &entityDatabase,
                                     Entity entity,
                                     AssetRegistry &assetRegistry) {
  if (node["skeleton"]) {
    auto uuid = node["skeleton"].as<Uuid>(Uuid{});
    auto handle = assetRegistry.findHandleByUuid<SkeletonAsset>(uuid);

    if (handle) {
      const auto &skeleton = assetRegistry.get(handle);

      Skeleton skeletonComponent{};
      skeletonComponent.jointLocalPositions = skeleton.jointLocalPositions;
      skeletonComponent.jointLocalRotations = skeleton.jointLocalRotations;
      skeletonComponent.jointLocalScales = skeleton.jointLocalScales;
      skeletonComponent.jointParents = skeleton.jointParents;
      skeletonComponent.jointInverseBindMatrices =
          skeleton.jointInverseBindMatrices;
      skeletonComponent.jointNames = skeleton.jointNames;
      skeletonComponent.assetHandle = handle;
      skeletonComponent.numJoints =
          static_cast<u32>(skeleton.jointLocalPositions.size());
      skeletonComponent.jointFinalTransforms.resize(skeletonComponent.numJoints,
                                                    glm::mat4{1.0f});
      skeletonComponent.jointWorldTransforms.resize(skeletonComponent.numJoints,
                                                    glm::mat4{1.0f});

      entityDatabase.set(entity, skeletonComponent);
    }
  }
}

} // namespace quoll
