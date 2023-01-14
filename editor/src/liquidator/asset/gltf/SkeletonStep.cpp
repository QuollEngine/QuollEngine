#include "liquid/core/Base.h"
// TODO: Remove engine logger
#include "liquid/core/Engine.h"

#include "SkeletonStep.h"
#include "Buffer.h"
#include "TransformUtils.h"

namespace liquidator {

void loadSkeletons(GLTFImportData &importData) {
  auto &assetCache = importData.assetCache;
  const auto &targetPath = importData.targetPath;
  const auto &model = importData.model;

  for (uint32_t si = 0; si < static_cast<uint32_t>(model.skins.size()); ++si) {
    const auto &skin = model.skins.at(si);

    std::unordered_map<uint32_t, int> jointParents;
    std::unordered_map<uint32_t, uint32_t> normalizedJointMap;

    auto &&ibMeta = getBufferMetaForAccessor(model, skin.inverseBindMatrices);

    if (ibMeta.accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
      liquid::Engine::getLogger().warning()
          << "Inverse bind matrices accessor must be of type FLOAT. Skipping "
             "skin #"
          << si;

      continue;
    }

    if (ibMeta.accessor.type != TINYGLTF_TYPE_MAT4) {
      liquid::Engine::getLogger().warning()
          << "Inverse bind matrices accessor must be MAT4. Skipping "
             "skin #"
          << si;
      continue;
    }

    if (ibMeta.accessor.count != skin.joints.size()) {
      liquid::Engine::getLogger().warning()
          << "Inverse bind matrices cannot be fewer than number of joints. "
             "Skipping "
             "skin #"
          << si;
      continue;
    }

    bool success = true;

    std::vector<glm::mat4> inverseBindMatrices(ibMeta.accessor.count);

    {
      const auto *data = reinterpret_cast<const glm::mat4 *>(ibMeta.rawData);
      for (size_t i = 0; i < ibMeta.accessor.count; ++i) {
        inverseBindMatrices.at(i) = data[i];
      }
    }

    bool skeletonValid = true;
    for (uint32_t i = 0;
         i < static_cast<uint32_t>(skin.joints.size()) && skeletonValid; ++i) {
      const auto &joint = skin.joints.at(i);

      skeletonValid =
          importData.skeletons.gltfToNormalizedJointMap.find(joint) ==
          importData.skeletons.gltfToNormalizedJointMap.end();
      if (!skeletonValid) {
        liquid::Engine::getLogger().warning()
            << "Single joint cannot be a child of multiple skins. Skipping "
               "joint #"
            << joint
            << " for "
               "skin #"
            << si;
        continue;
      }
      normalizedJointMap.insert({joint, i});
      importData.skeletons.gltfToNormalizedJointMap.insert({joint, i});
      importData.skeletons.jointSkinMap.insert({joint, si});

      jointParents.insert({i, -1});
    }

    if (!skeletonValid)
      continue;

    for (uint32_t j = 0; j < static_cast<uint32_t>(model.nodes.size()); ++j) {
      if (normalizedJointMap.find(j) == normalizedJointMap.end()) {
        continue;
      }

      uint32_t nJ = normalizedJointMap.at(j);

      const auto &node = model.nodes.at(j);
      for (auto &child : node.children) {
        if (normalizedJointMap.find(child) == normalizedJointMap.end()) {
          continue;
        }

        uint32_t nChild = normalizedJointMap.at(child);
        jointParents.at(nChild) = static_cast<int>(nJ);
      }
    }

    uint32_t numJoints = static_cast<uint32_t>(skin.joints.size());

    liquid::AssetData<liquid::SkeletonAsset> asset;
    asset.name = targetPath.string() + "/" + targetPath.filename().string() +
                 "-skeleton" + std::to_string(si);
    asset.type = liquid::AssetType::Skeleton;

    for (auto &joint : skin.joints) {
      uint32_t nJoint = normalizedJointMap.at(joint);
      int parent = jointParents.at(nJoint);
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

    auto path = assetCache.createSkeletonFromAsset(asset);
    auto handle = assetCache.loadSkeletonFromFile(path.getData());

    importData.skeletons.skeletonMap.map.insert_or_assign(
        static_cast<size_t>(si), handle.getData());
  }
}

} // namespace liquidator
