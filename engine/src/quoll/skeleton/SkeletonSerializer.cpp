#include "quoll/core/Base.h"
#include "Skeleton.h"
#include "SkeletonSerializer.h"

namespace quoll {

void SkeletonSerializer::serialize(YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity) {
  if (entityDatabase.has<Skeleton>(entity)) {
    const auto &asset = entityDatabase.get<Skeleton>(entity).assetHandle;
    if (asset) {
      node["skeleton"] = asset.meta().uuid;
    }
  }
}

void SkeletonSerializer::deserialize(const YAML::Node &node,
                                     EntityDatabase &entityDatabase,
                                     Entity entity, AssetCache &assetCache) {
  if (node["skeleton"]) {
    auto uuid = node["skeleton"].as<Uuid>(Uuid{});

    auto res = assetCache.request<SkeletonAsset>(uuid);
    if (res) {
      const auto &skeleton = res.data().get();
      Skeleton skeletonComponent{};
      skeletonComponent.assetHandle = res.data();
      skeletonComponent.jointLocalPositions = skeleton.jointLocalPositions;
      skeletonComponent.jointLocalRotations = skeleton.jointLocalRotations;
      skeletonComponent.jointLocalScales = skeleton.jointLocalScales;
      skeletonComponent.jointParents = skeleton.jointParents;
      skeletonComponent.jointInverseBindMatrices =
          skeleton.jointInverseBindMatrices;
      skeletonComponent.jointNames = skeleton.jointNames;
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
