#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

Result<Path>
AssetCache::createPrefabFromAsset(const AssetData<PrefabAsset> &asset) {
  if (asset.uuid.isEmpty()) {
    QuollAssert(false, "Invalid uuid provided");
    return Error("Invalid uuid provided");
  }

  auto assetPath = getPathFromUuid(asset.uuid);

  auto metaRes = createAssetMeta(AssetType::Prefab, asset.name, assetPath);
  if (!metaRes) {
    return Error("Cannot create prefab asset: " + asset.name);
  }

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Error("File cannot be opened for writing: " + assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Prefab;
  header.magic = AssetFileHeader::MagicConstant;
  file.write(header);

  std::unordered_map<AssetHandle<MaterialAsset>, u32> localMaterialMap;
  {
    std::vector<Uuid> assetPaths;
    assetPaths.reserve(asset.data.meshes.size());

    for (auto &component : asset.data.meshRenderers) {
      for (auto material : component.value.materials) {
        if (localMaterialMap.find(material) == localMaterialMap.end()) {
          auto uuid = mRegistry.getMeta(material).uuid;
          localMaterialMap.insert_or_assign(
              material, static_cast<u32>(assetPaths.size()));
          assetPaths.push_back(uuid);
        }
      }
    }

    for (auto &component : asset.data.skinnedMeshRenderers) {
      for (auto material : component.value.materials) {
        if (localMaterialMap.find(material) == localMaterialMap.end()) {
          auto uuid = mRegistry.getMeta(material).uuid;
          localMaterialMap.insert_or_assign(
              material, static_cast<u32>(assetPaths.size()));
          assetPaths.push_back(uuid);
        }
      }
    }

    file.write(static_cast<u32>(assetPaths.size()));
    file.write(assetPaths);
  }

  std::unordered_map<AssetHandle<MeshAsset>, u32> localMeshMap;
  {
    std::vector<Uuid> assetPaths;
    assetPaths.reserve(asset.data.meshes.size());

    for (auto &component : asset.data.meshes) {
      if (localMeshMap.find(component.value) == localMeshMap.end()) {
        auto uuid = mRegistry.getMeta(component.value).uuid;
        localMeshMap.insert_or_assign(component.value,
                                      static_cast<u32>(assetPaths.size()));
        assetPaths.push_back(uuid);
      }
    }

    file.write(static_cast<u32>(assetPaths.size()));
    file.write(assetPaths);
  }

  std::map<AssetHandle<SkeletonAsset>, u32> localSkeletonMap;
  {
    std::vector<Uuid> assetPaths;
    assetPaths.reserve(asset.data.skeletons.size());

    for (auto &component : asset.data.skeletons) {
      auto uuid = mRegistry.getMeta(component.value).uuid;

      if (localSkeletonMap.find(component.value) == localSkeletonMap.end()) {
        localSkeletonMap.insert_or_assign(component.value,
                                          static_cast<u32>(assetPaths.size()));
        assetPaths.push_back(uuid);
      }
    }

    file.write(static_cast<u32>(assetPaths.size()));
    file.write(assetPaths);
  }

  std::map<AssetHandle<AnimationAsset>, u32> localAnimationMap;
  {
    std::vector<Uuid> assetPaths;
    assetPaths.reserve(asset.data.animations.size());

    for (auto handle : asset.data.animations) {
      auto uuid = mRegistry.getMeta(handle).uuid;

      if (localAnimationMap.find(handle) == localAnimationMap.end()) {
        localAnimationMap.insert_or_assign(handle,
                                           static_cast<u32>(assetPaths.size()));
        assetPaths.push_back(uuid);
      }
    }

    file.write(static_cast<u32>(assetPaths.size()));
    file.write(assetPaths);
  }

  std::map<AssetHandle<AnimatorAsset>, u32> localAnimatorMap;
  {
    std::vector<Uuid> assetPaths;
    assetPaths.reserve(asset.data.animators.size());

    for (auto &component : asset.data.animators) {
      auto uuid = mRegistry.getMeta(component.value).uuid;

      if (localAnimatorMap.find(component.value) == localAnimatorMap.end()) {
        localAnimatorMap.insert_or_assign(component.value,
                                          static_cast<u32>(assetPaths.size()));
        assetPaths.push_back(uuid);
      }
    }

    file.write(static_cast<u32>(assetPaths.size()));
    file.write(assetPaths);
  }

  // Load component data
  {
    auto numComponents = static_cast<u32>(asset.data.transforms.size());

    file.write(numComponents);
    for (u32 i = 0; i < numComponents; ++i) {
      const auto &transform = asset.data.transforms.at(i).value;
      file.write(asset.data.transforms.at(i).entity);

      file.write(transform.position);
      file.write(transform.rotation);
      file.write(transform.scale);
      file.write(transform.parent);
    }
  }

  {
    auto numComponents = static_cast<u32>(asset.data.names.size());
    file.write(numComponents);

    for (auto &name : asset.data.names) {
      file.write(name.entity);
      file.write(name.value);
    }
  }

  {
    auto numComponents = static_cast<u32>(asset.data.meshes.size());
    file.write(numComponents);
    for (auto &component : asset.data.meshes) {
      file.write(component.entity);
      file.write(localMeshMap.at(component.value));
    }
  }

  {
    auto numComponents = static_cast<u32>(asset.data.meshRenderers.size());
    file.write(numComponents);
    for (auto &component : asset.data.meshRenderers) {
      file.write(component.entity);

      file.write(static_cast<u32>(component.value.materials.size()));
      for (auto handle : component.value.materials) {
        file.write(localMaterialMap.at(handle));
      }
    }
  }

  {
    auto numComponents =
        static_cast<u32>(asset.data.skinnedMeshRenderers.size());
    file.write(numComponents);
    for (auto &component : asset.data.skinnedMeshRenderers) {
      file.write(component.entity);

      file.write(static_cast<u32>(component.value.materials.size()));
      for (auto handle : component.value.materials) {
        file.write(localMaterialMap.at(handle));
      }
    }
  }

  {
    auto numComponents = static_cast<u32>(asset.data.skeletons.size());
    file.write(numComponents);
    for (auto &component : asset.data.skeletons) {
      file.write(component.entity);
      file.write(localSkeletonMap.at(component.value));
    }
  }

  {
    auto numComponents = static_cast<u32>(asset.data.animations.size());
    file.write(numComponents);

    for (auto &component : asset.data.animations) {
      file.write(localAnimationMap.at(component));
    }
  }

  {
    auto numComponents = static_cast<u32>(asset.data.animators.size());
    file.write(numComponents);

    for (auto &component : asset.data.animators) {
      file.write(component.entity);
      file.write(localAnimatorMap.at(component.value));
    }
  }

  {
    auto numComponents = static_cast<u32>(asset.data.directionalLights.size());
    file.write(numComponents);

    for (auto &light : asset.data.directionalLights) {
      file.write(light.entity);
      file.write(light.value.color);
      file.write(light.value.intensity);
    }
  }

  {
    auto numComponents = static_cast<u32>(asset.data.pointLights.size());
    file.write(numComponents);

    for (auto &light : asset.data.pointLights) {
      file.write(light.entity);
      file.write(light.value.color);
      file.write(light.value.intensity);
      file.write(light.value.range);
    }
  }

  return assetPath;
}

Result<PrefabAsset>
AssetCache::loadPrefabDataFromInputStream(const Path &path) {
  InputBinaryStream stream(path);
  AssetFileHeader header;
  stream.read(header);
  if (header.magic != AssetFileHeader::MagicConstant ||
      header.type != AssetType::Prefab) {
    return Error("Invalid file format");
  }

  std::vector<String> warnings;

  PrefabAsset prefab{};

  std::vector<AssetHandle<MaterialAsset>> localMaterialMap;
  {
    u32 numAssets = 0;
    stream.read(numAssets);
    std::vector<quoll::Uuid> actual(numAssets);
    stream.read(actual);
    localMaterialMap.resize(numAssets, AssetHandle<MaterialAsset>());

    for (u32 i = 0; i < numAssets; ++i) {
      auto assetUuid = actual.at(i);
      auto res = getOrLoad<MaterialAsset>(assetUuid);
      if (res) {
        localMaterialMap.at(i) = res;
        warnings.insert(warnings.end(), res.warnings().begin(),
                        res.warnings().end());
      } else {
        warnings.push_back(res.error());
      }
    }
  }

  std::vector<AssetHandle<MeshAsset>> localMeshMap;
  {
    u32 numAssets = 0;
    stream.read(numAssets);
    std::vector<quoll::Uuid> actual(numAssets);
    stream.read(actual);
    localMeshMap.resize(numAssets, AssetHandle<MeshAsset>());

    for (u32 i = 0; i < numAssets; ++i) {
      auto assetUuid = actual.at(i);
      auto res = getOrLoad<MeshAsset>(assetUuid);
      if (res) {
        localMeshMap.at(i) = res;
        warnings.insert(warnings.end(), res.warnings().begin(),
                        res.warnings().end());
      } else {
        warnings.push_back(res.error());
      }
    }
  }

  std::vector<AssetHandle<SkeletonAsset>> localSkeletonMap;
  {
    u32 numAssets = 0;
    stream.read(numAssets);
    std::vector<quoll::Uuid> actual(numAssets);
    stream.read(actual);
    localSkeletonMap.resize(numAssets, AssetHandle<SkeletonAsset>());

    for (u32 i = 0; i < numAssets; ++i) {
      auto assetUuid = actual.at(i);
      auto res = getOrLoad<SkeletonAsset>(assetUuid);
      if (res) {
        localSkeletonMap.at(i) = res;
        warnings.insert(warnings.end(), res.warnings().begin(),
                        res.warnings().end());
      } else {
        warnings.push_back(res.error());
      }
    }
  }

  std::vector<AssetHandle<AnimationAsset>> localAnimationMap;
  {
    u32 numAssets = 0;
    stream.read(numAssets);
    std::vector<quoll::Uuid> actual(numAssets);
    stream.read(actual);
    localAnimationMap.resize(numAssets, AssetHandle<AnimationAsset>());

    for (u32 i = 0; i < numAssets; ++i) {
      auto assetUuid = actual.at(i);
      auto res = getOrLoad<AnimationAsset>(assetUuid);
      if (res) {
        localAnimationMap.at(i) = res;
        warnings.insert(warnings.end(), res.warnings().begin(),
                        res.warnings().end());

      } else {
        warnings.push_back(res.error());
      }
    }
  }

  std::vector<AssetHandle<AnimatorAsset>> localAnimatorMap;
  {
    u32 numAssets = 0;
    stream.read(numAssets);
    std::vector<quoll::Uuid> actual(numAssets);
    stream.read(actual);
    localAnimatorMap.resize(numAssets, AssetHandle<AnimatorAsset>());

    for (u32 i = 0; i < numAssets; ++i) {
      auto assetUuid = actual.at(i);
      auto res = getOrLoad<AnimatorAsset>(assetUuid);
      if (res) {
        localAnimatorMap.at(i) = res;
        warnings.insert(warnings.end(), res.warnings().begin(),
                        res.warnings().end());

      } else {
        warnings.push_back(res.error());
      }
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);
    prefab.transforms.resize(numComponents);
    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      glm::vec3 position;
      glm::quat rotation;
      glm::vec3 scale;
      i32 parent = -1;
      stream.read(position);
      stream.read(rotation);
      stream.read(scale);
      stream.read(parent);

      prefab.transforms.at(i).entity = entity;
      prefab.transforms.at(i).value = {position, rotation, scale, parent};
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);
    prefab.names.resize(numComponents);
    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      String name;
      stream.read(name);

      prefab.names.at(i).entity = entity;
      prefab.names.at(i).value = name;
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);

    prefab.meshes.resize(numComponents);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      u32 meshIndex = 0;
      stream.read(meshIndex);

      prefab.meshes.at(i).entity = entity;
      prefab.meshes.at(i).value = localMeshMap.at(meshIndex);
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);

    prefab.meshRenderers.resize(numComponents);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      u32 numMaterials = 0;
      stream.read(numMaterials);

      std::vector<u32> materialIndices(numMaterials);
      stream.read(materialIndices);

      prefab.meshRenderers.at(i).entity = entity;

      for (auto materialIndex : materialIndices) {
        prefab.meshRenderers.at(i).value.materials.push_back(
            localMaterialMap.at(materialIndex));
      }
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);

    prefab.skinnedMeshRenderers.resize(numComponents);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      u32 numMaterials = 0;
      stream.read(numMaterials);

      std::vector<u32> materialIndices(numMaterials);
      stream.read(materialIndices);

      prefab.skinnedMeshRenderers.at(i).entity = entity;

      for (auto materialIndex : materialIndices) {
        prefab.skinnedMeshRenderers.at(i).value.materials.push_back(
            localMaterialMap.at(materialIndex));
      }
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);

    prefab.skeletons.resize(numComponents);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      u32 meshIndex = 0;
      stream.read(meshIndex);

      prefab.skeletons.at(i).entity = entity;
      prefab.skeletons.at(i).value = localSkeletonMap.at(meshIndex);
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);

    prefab.animations.resize(numComponents);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 animationIndex = 0;
      stream.read(animationIndex);

      prefab.animations.at(i) = localAnimationMap.at(animationIndex);
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);

    prefab.animators.resize(numComponents);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      u32 animatorIndex = 0;
      stream.read(animatorIndex);

      prefab.animators.at(i).entity = entity;
      prefab.animators.at(i).value = localAnimatorMap.at(animatorIndex);
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);
    prefab.directionalLights.resize(numComponents);
    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      glm::vec4 color;
      f32 intensity = 0.0f;
      stream.read(color);
      stream.read(intensity);

      prefab.directionalLights.at(i).entity = entity;
      prefab.directionalLights.at(i).value = {color, intensity};
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);
    prefab.pointLights.resize(numComponents);
    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      glm::vec4 color;
      f32 intensity = 0.0f;
      f32 range = 0.0f;
      stream.read(color);
      stream.read(intensity);
      stream.read(range);

      prefab.pointLights.at(i).entity = entity;
      prefab.pointLights.at(i).value = {color, intensity, range};
    }
  }

  if (prefab.transforms.empty() && prefab.directionalLights.empty() &&
      prefab.pointLights.empty() && prefab.meshes.empty() &&
      prefab.skeletons.empty() && prefab.animators.empty() &&
      prefab.names.empty() && prefab.meshRenderers.empty() &&
      prefab.skinnedMeshRenderers.empty()) {
    return Error("Prefab is empty");
  }

  return {prefab, warnings};
}

Result<PrefabAsset> AssetCache::loadPrefab(const Uuid &uuid) {
  auto meta = getAssetMeta(uuid);
  if (meta.type != AssetType::Prefab) {
    return Error("Asset type is not prefab");
  }

  return loadPrefabDataFromInputStream(getPathFromUuid(uuid));
}

} // namespace quoll
