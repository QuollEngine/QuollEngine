#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "AssetFileHeader.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

Result<Path>
AssetCache::createPrefabFromAsset(const AssetData<PrefabAsset> &asset) {
  if (asset.uuid.isEmpty()) {
    QuollAssert(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  auto assetPath = getPathFromUuid(asset.uuid);

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Prefab;
  header.magic = AssetFileHeader::MagicConstant;
  header.name = asset.name;
  file.write(header);

  std::unordered_map<AssetHandle<MaterialAsset>, u32> localMaterialMap;
  {
    std::vector<Uuid> assetPaths;
    assetPaths.reserve(asset.data.meshes.size());

    for (auto &component : asset.data.meshRenderers) {
      for (auto material : component.value.materials) {
        if (localMaterialMap.find(material) == localMaterialMap.end()) {
          auto uuid = mRegistry.get(material).uuid;
          localMaterialMap.insert_or_assign(
              material, static_cast<u32>(assetPaths.size()));
          assetPaths.push_back(uuid);
        }
      }
    }

    for (auto &component : asset.data.skinnedMeshRenderers) {
      for (auto material : component.value.materials) {
        if (localMaterialMap.find(material) == localMaterialMap.end()) {
          auto uuid = mRegistry.get(material).uuid;
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
        auto uuid = mRegistry.get(component.value).uuid;
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
      auto uuid = mRegistry.get(component.value).uuid;

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
      auto uuid = mRegistry.get(handle).uuid;

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
      auto uuid = mRegistry.get(component.value).uuid;

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

  return Result<Path>::Ok(assetPath);
}

Result<AssetHandle<PrefabAsset>>
AssetCache::loadPrefabDataFromInputStream(InputBinaryStream &stream,
                                          const Path &filePath,
                                          const AssetFileHeader &header) {

  std::vector<String> warnings;

  AssetData<PrefabAsset> prefab{};
  prefab.name = header.name;
  prefab.path = filePath;
  prefab.type = AssetType::Prefab;
  prefab.uuid = Uuid(filePath.stem().string());

  std::vector<AssetHandle<MaterialAsset>> localMaterialMap;
  {
    u32 numAssets = 0;
    stream.read(numAssets);
    std::vector<quoll::Uuid> actual(numAssets);
    stream.read(actual);
    localMaterialMap.resize(numAssets, AssetHandle<MaterialAsset>());

    for (u32 i = 0; i < numAssets; ++i) {
      auto assetUuid = actual.at(i);
      const auto &res = getOrLoadMaterial(assetUuid);
      if (res.hasData()) {
        localMaterialMap.at(i) = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());
      } else {
        warnings.push_back(res.getError());
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
      const auto &res = getOrLoadMesh(assetUuid);
      if (res.hasData()) {
        localMeshMap.at(i) = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());
      } else {
        warnings.push_back(res.getError());
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
      const auto &res = getOrLoadSkeleton(assetUuid);
      if (res.hasData()) {
        localSkeletonMap.at(i) = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());
      } else {
        warnings.push_back(res.getError());
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
      const auto &res = getOrLoadAnimation(assetUuid);
      if (res.hasData()) {
        localAnimationMap.at(i) = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());

      } else {
        warnings.push_back(res.getError());
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
      const auto &res = getOrLoadAnimator(assetUuid);
      if (res.hasData()) {
        localAnimatorMap.at(i) = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());

      } else {
        warnings.push_back(res.getError());
      }
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);
    prefab.data.transforms.resize(numComponents);
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

      prefab.data.transforms.at(i).entity = entity;
      prefab.data.transforms.at(i).value = {position, rotation, scale, parent};
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);
    prefab.data.names.resize(numComponents);
    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      String name;
      stream.read(name);

      prefab.data.names.at(i).entity = entity;
      prefab.data.names.at(i).value = name;
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);

    prefab.data.meshes.resize(numComponents);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      u32 meshIndex = 0;
      stream.read(meshIndex);

      prefab.data.meshes.at(i).entity = entity;
      prefab.data.meshes.at(i).value = localMeshMap.at(meshIndex);
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);

    prefab.data.meshRenderers.resize(numComponents);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      u32 numMaterials = 0;
      stream.read(numMaterials);

      std::vector<u32> materialIndices(numMaterials);
      stream.read(materialIndices);

      prefab.data.meshRenderers.at(i).entity = entity;

      for (auto materialIndex : materialIndices) {
        prefab.data.meshRenderers.at(i).value.materials.push_back(
            localMaterialMap.at(materialIndex));
      }
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);

    prefab.data.skinnedMeshRenderers.resize(numComponents);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      u32 numMaterials = 0;
      stream.read(numMaterials);

      std::vector<u32> materialIndices(numMaterials);
      stream.read(materialIndices);

      prefab.data.skinnedMeshRenderers.at(i).entity = entity;

      for (auto materialIndex : materialIndices) {
        prefab.data.skinnedMeshRenderers.at(i).value.materials.push_back(
            localMaterialMap.at(materialIndex));
      }
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);

    prefab.data.skeletons.resize(numComponents);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      u32 meshIndex = 0;
      stream.read(meshIndex);

      prefab.data.skeletons.at(i).entity = entity;
      prefab.data.skeletons.at(i).value = localSkeletonMap.at(meshIndex);
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);

    prefab.data.animations.resize(numComponents);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 animationIndex = 0;
      stream.read(animationIndex);

      prefab.data.animations.at(i) = localAnimationMap.at(animationIndex);
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);

    prefab.data.animators.resize(numComponents);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      u32 animatorIndex = 0;
      stream.read(animatorIndex);

      prefab.data.animators.at(i).entity = entity;
      prefab.data.animators.at(i).value = localAnimatorMap.at(animatorIndex);
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);
    prefab.data.directionalLights.resize(numComponents);
    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      glm::vec4 color;
      f32 intensity = 0.0f;
      stream.read(color);
      stream.read(intensity);

      prefab.data.directionalLights.at(i).entity = entity;
      prefab.data.directionalLights.at(i).value = {color, intensity};
    }
  }

  {
    u32 numComponents = 0;
    stream.read(numComponents);
    prefab.data.pointLights.resize(numComponents);
    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      stream.read(entity);

      glm::vec4 color;
      f32 intensity = 0.0f;
      f32 range = 0.0f;
      stream.read(color);
      stream.read(intensity);
      stream.read(range);

      prefab.data.pointLights.at(i).entity = entity;
      prefab.data.pointLights.at(i).value = {color, intensity, range};
    }
  }

  if (prefab.data.transforms.empty() && prefab.data.directionalLights.empty() &&
      prefab.data.pointLights.empty() && prefab.data.meshes.empty() &&
      prefab.data.skeletons.empty() && prefab.data.animators.empty() &&
      prefab.data.names.empty() && prefab.data.meshRenderers.empty() &&
      prefab.data.skinnedMeshRenderers.empty()) {
    return Result<AssetHandle<PrefabAsset>>::Error("Prefab is empty");
  }

  return Result<AssetHandle<PrefabAsset>>::Ok(mRegistry.add(prefab), warnings);
}

Result<AssetHandle<PrefabAsset>> AssetCache::loadPrefab(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::Prefab);
  if (header.hasError()) {
    return Result<AssetHandle<PrefabAsset>>::Error(header.getError());
  }

  return loadPrefabDataFromInputStream(stream, filePath, header.getData());
}

} // namespace quoll
