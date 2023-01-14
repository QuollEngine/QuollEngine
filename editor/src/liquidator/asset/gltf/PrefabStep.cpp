#include "liquid/core/Base.h"

#include "PrefabStep.h"
#include "TransformUtils.h"

namespace liquidator {

/**
 * @brief Load prefabs into asset registry
 *
 * @param importData GLTF import data
 */
void loadPrefabs(GLTFImportData &importData) {
  auto &assetCache = importData.assetCache;
  const auto &targetPath = importData.targetPath;
  const auto &model = importData.model;

  liquid::AssetData<liquid::PrefabAsset> prefab;
  prefab.name = targetPath.string() + "/" + targetPath.stem().string();
  prefab.type = liquid::AssetType::Prefab;

  auto &gltfNodes = model.scenes.at(model.defaultScene);

  std::unordered_map<int, bool> jointNodes;
  for (auto &skin : model.skins) {
    for (auto &joint : skin.joints) {
      jointNodes.insert({joint, true});
    }
  }

  std::list<std::pair<int, int>> nodesToProcess;

  for (auto &nodeIndex : gltfNodes.nodes) {
    if (jointNodes.find(nodeIndex) == jointNodes.end()) {
      nodesToProcess.push_back(std::make_pair(nodeIndex, -1));
    }
  }

  while (!nodesToProcess.empty()) {
    auto [nodeIndex, parentIndex] = nodesToProcess.front();
    nodesToProcess.pop_front();

    auto &node = model.nodes.at(nodeIndex);

    for (auto child : node.children) {
      if (jointNodes.find(child) == jointNodes.end()) {
        nodesToProcess.push_back(std::make_pair(child, nodeIndex));
      }
    }

    if (node.mesh < 0)
      continue;

    auto localEntityId = static_cast<uint32_t>(nodeIndex);

    liquid::PrefabTransformData transform{};
    auto data = loadTransformData(node);
    transform.position = data.localPosition;
    transform.rotation = data.localRotation;
    transform.scale = data.localScale;
    transform.parent = static_cast<int32_t>(parentIndex);

    prefab.data.transforms.push_back({localEntityId, transform});

    if (node.skin >= 0) {
      prefab.data.skinnedMeshes.push_back(
          {localEntityId, importData.skinnedMeshes.map.at(node.mesh)});
      prefab.data.skeletons.push_back(
          {localEntityId, importData.skeletons.skeletonMap.map.at(node.skin)});

      liquid::Animator component;

      auto it = importData.animations.skinAnimationMap.find(node.skin);
      if (it != importData.animations.skinAnimationMap.end()) {
        component.animations = it->second;
      }

      if (component.animations.size() > 0) {
        prefab.data.animators.push_back({localEntityId, component});
      }
    } else {
      prefab.data.meshes.push_back(
          {localEntityId, importData.meshes.map.at(node.mesh)});

      liquid::Animator component;

      auto it = importData.animations.nodeAnimationMap.find(nodeIndex);
      if (it != importData.animations.nodeAnimationMap.end()) {
        component.animations = it->second;
      }

      if (component.animations.size() > 0) {
        prefab.data.animators.push_back({localEntityId, component});
      }
    }
  }

  auto path = assetCache.createPrefabFromAsset(prefab);
  assetCache.loadPrefabFromFile(path.getData());

  importData.outputPath = path;
}

} // namespace liquidator
