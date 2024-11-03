#include "quoll/core/Base.h"
#include "Buffer.h"
#include "SkeletonStep.h"
#include "TransformUtils.h"

namespace quoll::editor {

void loadSkeletons(GLTFImportData &importData) {
  auto &assetCache = importData.assetCache;
  const auto &model = importData.model;

  for (u32 si = 0; si < static_cast<u32>(model.skins.size()); ++si) {
    const auto &skin = model.skins.at(si);

    auto skeletonName =
        skin.name.empty() ? "skeleton" + std::to_string(si) : skin.name;
    skeletonName += ".skel";

    std::unordered_map<u32, int> jointParents;
    std::unordered_map<u32, u32> normalizedJointMap;

    auto &&ibMeta = getBufferMetaForAccessor(model, skin.inverseBindMatrices);

    if (ibMeta.accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
      importData.warnings.push_back(
          skeletonName +
          " skipped because inverse bind matrices component type is not FLOAT");

      continue;
    }

    if (ibMeta.accessor.type != TINYGLTF_TYPE_MAT4) {
      importData.warnings.push_back(
          skeletonName +
          " skipped because inverse bind matrices type is not MAT4");
      continue;
    }

    if (ibMeta.accessor.count != skin.joints.size()) {
      importData.warnings.push_back(
          skeletonName + " skipped because number of inverse bind matrices "
                         "is different from number of joints");
      continue;
    }

    std::vector<glm::mat4> inverseBindMatrices(ibMeta.accessor.count);

    {
      const auto *data = reinterpret_cast<const glm::mat4 *>(ibMeta.rawData);
      for (usize i = 0; i < ibMeta.accessor.count; ++i) {
        inverseBindMatrices.at(i) = data[i];
      }
    }

    bool skeletonValid = true;
    for (u32 i = 0; i < static_cast<u32>(skin.joints.size()) && skeletonValid;
         ++i) {
      const auto &joint = skin.joints.at(i);

      skeletonValid =
          importData.skeletons.gltfToNormalizedJointMap.find(joint) ==
          importData.skeletons.gltfToNormalizedJointMap.end();
      if (!skeletonValid) {
        importData.warnings.push_back(
            skeletonName +
            " skipped because a joint is a child of multiple skins");
        continue;
      }
      normalizedJointMap.insert({joint, i});
      importData.skeletons.gltfToNormalizedJointMap.insert({joint, i});
      importData.skeletons.jointSkinMap.insert({joint, si});

      jointParents.insert({i, -1});
    }

    if (!skeletonValid)
      continue;

    for (u32 j = 0; j < static_cast<u32>(model.nodes.size()); ++j) {
      if (normalizedJointMap.find(j) == normalizedJointMap.end()) {
        continue;
      }

      const u32 nJ = normalizedJointMap.at(j);

      const auto &node = model.nodes.at(j);
      for (auto &child : node.children) {
        if (normalizedJointMap.find(child) == normalizedJointMap.end()) {
          continue;
        }

        const u32 nChild = normalizedJointMap.at(child);
        jointParents.at(nChild) = static_cast<int>(nJ);
      }
    }

    const u32 numJoints = static_cast<u32>(skin.joints.size());

    auto assetName =
        skin.name.empty() ? "skeleton" + std::to_string(si) : skin.name;

    assetName += ".skel";

    AssetData<SkeletonAsset> asset;
    asset.name = getGLTFAssetName(importData, assetName);
    asset.type = AssetType::Skeleton;
    asset.uuid = getOrCreateGLTFUuid(importData, skeletonName);

    for (auto &joint : skin.joints) {
      const u32 nJoint = normalizedJointMap.at(joint);
      const int parent = jointParents.at(nJoint);
      const auto &node = model.nodes.at(joint);
      const auto &data = loadTransformData(node);

      asset.data.jointLocalPositions.push_back(data.localPosition);
      asset.data.jointLocalRotations.push_back(data.localRotation);
      asset.data.jointLocalScales.push_back(data.localScale);
      asset.data.jointInverseBindMatrices.push_back(
          inverseBindMatrices.at(nJoint));
      asset.data.jointNames.push_back(node.name);
      asset.data.jointParents.push_back(parent >= 0 ? parent : 0);
    }

    auto path = assetCache.createFromData(asset);
    auto ref = assetCache.request<SkeletonAsset>(asset.uuid);

    if (!ref) {
      importData.warnings.push_back(ref.error());
      return;
    }

    auto res = assetCache.waitForIdle(asset.uuid);
    if (!res) {
      importData.warnings.push_back(res.error());
      return;
    }

    importData.skeletons.skeletonMap.map.insert_or_assign(
        static_cast<usize>(si), ref.data());
    importData.outputUuids.insert_or_assign(assetName, asset.uuid);
  }
}

} // namespace quoll::editor
