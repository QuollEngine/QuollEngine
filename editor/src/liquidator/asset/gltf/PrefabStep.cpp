#include "liquid/core/Base.h"

#include "PrefabStep.h"
#include "TransformUtils.h"

namespace liquid::editor {

/**
 * @brief Load prefabs into asset registry
 *
 * @param importData GLTF import data
 */
void loadPrefabs(GLTFImportData &importData) {
  auto &assetCache = importData.assetCache;
  const auto &model = importData.model;

  AssetData<PrefabAsset> prefab;
  prefab.name = importData.sourcePath.filename().string();
  prefab.type = AssetType::Prefab;

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

    bool hasValidMesh =
        node.mesh >= 0 &&
        (importData.skinnedMeshes.map.find(node.mesh) !=
             importData.skinnedMeshes.map.end() ||
         importData.meshes.map.find(node.mesh) != importData.meshes.map.end());
    bool hasValidLight = false;

    auto itExtLight = node.extensions.find("KHR_lights_punctual");

    if (itExtLight != node.extensions.end()) {
      const auto &data = (*itExtLight).second;
      if (data.Has("light")) {
        auto index = data.Get("light").GetNumberAsInt();
        hasValidLight =
            index >= 0 &&
            (importData.directionalLights.map.find(static_cast<size_t>(
                 index)) != importData.directionalLights.map.end() ||
             importData.pointLights.map.find(static_cast<size_t>(index)) !=
                 importData.pointLights.map.end());
      }
    }

    auto localEntityId = static_cast<uint32_t>(nodeIndex);

    PrefabTransformData transform{};
    auto data = loadTransformData(node);
    transform.position = data.localPosition;
    transform.rotation = data.localRotation;
    transform.scale = data.localScale;
    transform.parent = static_cast<int32_t>(parentIndex);

    prefab.data.transforms.push_back({localEntityId, transform});

    prefab.data.names.push_back(
        {localEntityId, node.name.empty() ? "Untitled" : node.name});

    if (hasValidMesh) {
      if (node.skin >= 0) {
        prefab.data.skinnedMeshes.push_back(
            {localEntityId, importData.skinnedMeshes.map.at(node.mesh)});

        prefab.data.skeletons.push_back(
            {localEntityId,
             importData.skeletons.skeletonMap.map.at(node.skin)});

        auto it = importData.animations.skinAnimatorMap.find(node.skin);
        if (it != importData.animations.skinAnimatorMap.end()) {
          prefab.data.animators.push_back({localEntityId, it->second});
        }

      } else {
        prefab.data.meshes.push_back(
            {localEntityId, importData.meshes.map.at(node.mesh)});

        auto it = importData.animations.nodeAnimatorMap.find(nodeIndex);
        if (it != importData.animations.nodeAnimatorMap.end()) {
          prefab.data.animators.push_back({localEntityId, it->second});
        }
      }
    }

    if (hasValidLight) {
      const auto &light = itExtLight->second;
      size_t lightIndex =
          static_cast<size_t>(light.Get("light").GetNumberAsInt());

      if (importData.directionalLights.map.find(lightIndex) !=
          importData.directionalLights.map.end()) {
        prefab.data.directionalLights.push_back(
            {localEntityId, importData.directionalLights.map.at(lightIndex)});
      } else if (importData.pointLights.map.find(lightIndex) !=
                 importData.pointLights.map.end()) {
        prefab.data.pointLights.push_back(
            {localEntityId, importData.pointLights.map.at(lightIndex)});
      }
    }
  }

  auto it = importData.uuids.find("root");

  auto path = assetCache.createPrefabFromAsset(
      prefab, it != importData.uuids.end() ? it->second : "");

  if (path.hasData()) {
    auto handle = assetCache.loadPrefabFromFile(path.getData());

    if (handle.hasData()) {
      importData.outputUuids.insert_or_assign("root",
                                              assetCache.getRegistry()
                                                  .getPrefabs()
                                                  .getAsset(handle.getData())
                                                  .uuid);
    }
  }
}

} // namespace liquid::editor
