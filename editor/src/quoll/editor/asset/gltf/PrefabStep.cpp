#include "quoll/core/Base.h"
#include "PrefabStep.h"
#include "TransformUtils.h"

namespace quoll::editor {

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
  prefab.uuid = getOrCreateGLTFUuid(importData, "root");

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
        importData.meshes.map.find(node.mesh) != importData.meshes.map.end();
    bool hasValidLight = false;

    auto itExtLight = node.extensions.find("KHR_lights_punctual");

    if (itExtLight != node.extensions.end()) {
      const auto &data = (*itExtLight).second;
      if (data.Has("light")) {
        auto index = data.Get("light").GetNumberAsInt();
        hasValidLight =
            index >= 0 &&
            (importData.directionalLights.map.find(static_cast<usize>(index)) !=
                 importData.directionalLights.map.end() ||
             importData.pointLights.map.find(static_cast<usize>(index)) !=
                 importData.pointLights.map.end());
      }
    }

    auto localEntityId = static_cast<u32>(nodeIndex);

    PrefabTransformData transform{};
    auto data = loadTransformData(node);
    transform.position = data.localPosition;
    transform.rotation = data.localRotation;
    transform.scale = data.localScale;
    transform.parent = static_cast<i32>(parentIndex);

    prefab.data.transforms.push_back({localEntityId, transform});

    prefab.data.names.push_back(
        {localEntityId, node.name.empty() ? "Untitled" : node.name});

    if (hasValidMesh) {
      auto handle = importData.meshes.map.at(node.mesh);
      const auto &asset = importData.assetCache.getRegistry().get(handle);
      const auto &materials = importData.meshMaterials.at(handle);
      prefab.data.meshes.push_back({localEntityId, handle});

      if (node.skin >= 0) {
        SkinnedMeshRenderer renderer{materials};
        prefab.data.skinnedMeshRenderers.push_back({localEntityId, renderer});

        prefab.data.skeletons.push_back(
            {localEntityId,
             importData.skeletons.skeletonMap.map.at(node.skin)});

        auto it = importData.animations.skinAnimatorMap.find(node.skin);
        if (it != importData.animations.skinAnimatorMap.end()) {
          prefab.data.animators.push_back({localEntityId, it->second});
        }
      } else {
        MeshRenderer renderer{materials};
        prefab.data.meshRenderers.push_back({localEntityId, renderer});

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
      usize lightIndex =
          static_cast<usize>(light.Get("light").GetNumberAsInt());

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

  auto path = assetCache.createPrefabFromAsset(prefab);

  if (path) {
    auto handle = assetCache.loadPrefab(prefab.uuid);

    if (handle) {
      importData.outputUuids.insert_or_assign(
          "root", assetCache.getRegistry().getMeta(handle.data()).uuid);
    }
  }
}

} // namespace quoll::editor
