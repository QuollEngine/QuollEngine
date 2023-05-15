#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetCache.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

Result<Path>
AssetCache::createPrefabFromAsset(const AssetData<PrefabAsset> &asset) {
  String extension = ".lqprefab";
  Path assetPath = (mAssetsPath / (asset.name + extension)).make_preferred();
  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Prefab;
  header.version = createVersion(0, 1);
  file.write(header.magic, AssetFileMagicLength);
  file.write(header.version);
  file.write(header.type);

  std::map<MeshAssetHandle, uint32_t> localMeshMap;
  {
    std::vector<String> assetPaths;
    assetPaths.reserve(asset.data.meshes.size());

    for (auto &component : asset.data.meshes) {
      auto &mesh = mRegistry.getMeshes().getAsset(component.value);
      auto path = std::filesystem::relative(mesh.path, mAssetsPath).string();
      std::replace(path.begin(), path.end(), '\\', '/');

      if (localMeshMap.find(component.value) == localMeshMap.end()) {
        localMeshMap.insert_or_assign(component.value,
                                      static_cast<uint32_t>(assetPaths.size()));
        assetPaths.push_back(path);
      }
    }

    file.write(static_cast<uint32_t>(assetPaths.size()));
    file.write(assetPaths);
  }

  std::map<SkinnedMeshAssetHandle, uint32_t> localSkinnedMeshMap;
  {
    std::vector<String> assetPaths;
    assetPaths.reserve(asset.data.skinnedMeshes.size());

    for (auto &component : asset.data.skinnedMeshes) {
      auto &mesh = mRegistry.getSkinnedMeshes().getAsset(component.value);
      auto path = std::filesystem::relative(mesh.path, mAssetsPath).string();
      std::replace(path.begin(), path.end(), '\\', '/');

      if (localSkinnedMeshMap.find(component.value) ==
          localSkinnedMeshMap.end()) {
        localSkinnedMeshMap.insert_or_assign(
            component.value, static_cast<uint32_t>(assetPaths.size()));
        assetPaths.push_back(path);
      }
    }

    file.write(static_cast<uint32_t>(assetPaths.size()));
    file.write(assetPaths);
  }

  std::map<SkeletonAssetHandle, uint32_t> localSkeletonMap;
  {
    std::vector<String> assetPaths;
    assetPaths.reserve(asset.data.skeletons.size());

    for (auto &component : asset.data.skeletons) {
      auto &skeleton = mRegistry.getSkeletons().getAsset(component.value);
      auto path =
          std::filesystem::relative(skeleton.path, mAssetsPath).string();
      std::replace(path.begin(), path.end(), '\\', '/');

      if (localSkeletonMap.find(component.value) == localSkeletonMap.end()) {
        localSkeletonMap.insert_or_assign(
            component.value, static_cast<uint32_t>(assetPaths.size()));
        assetPaths.push_back(path);
      }
    }

    file.write(static_cast<uint32_t>(assetPaths.size()));
    file.write(assetPaths);
  }

  std::map<AnimationAssetHandle, uint32_t> localAnimationMap;
  {
    std::vector<String> assetPaths;
    uint32_t numAnimations = 0;
    for (auto &animator : asset.data.animators) {
      numAnimations += static_cast<uint32_t>(animator.value.animations.size());
    }
    assetPaths.reserve(numAnimations);

    for (auto &component : asset.data.animators) {
      for (auto &handle : component.value.animations) {
        auto &animation = mRegistry.getAnimations().getAsset(handle);
        auto path =
            std::filesystem::relative(animation.path, mAssetsPath).string();
        std::replace(path.begin(), path.end(), '\\', '/');

        if (localAnimationMap.find(handle) == localAnimationMap.end()) {
          localAnimationMap.insert_or_assign(
              handle, static_cast<uint32_t>(assetPaths.size()));
          assetPaths.push_back(path);
        }
      }
    }

    file.write(static_cast<uint32_t>(assetPaths.size()));
    file.write(assetPaths);
  }

  // Load component data
  {
    auto numComponents = static_cast<uint32_t>(asset.data.transforms.size());

    file.write(numComponents);
    for (uint32_t i = 0; i < numComponents; ++i) {
      const auto &transform = asset.data.transforms.at(i).value;
      file.write(asset.data.transforms.at(i).entity);

      file.write(transform.position);
      file.write(transform.rotation);
      file.write(transform.scale);
      file.write(transform.parent);
    }
  }

  {
    auto numComponents = static_cast<uint32_t>(asset.data.meshes.size());
    file.write(numComponents);
    for (auto &component : asset.data.meshes) {
      file.write(component.entity);
      file.write(localMeshMap.at(component.value));
    }
  }

  {
    auto numComponents = static_cast<uint32_t>(asset.data.skinnedMeshes.size());
    file.write(numComponents);
    for (auto &component : asset.data.skinnedMeshes) {
      file.write(component.entity);
      file.write(localSkinnedMeshMap.at(component.value));
    }
  }

  {
    auto numComponents = static_cast<uint32_t>(asset.data.skeletons.size());
    file.write(numComponents);
    for (auto &component : asset.data.skeletons) {
      file.write(component.entity);
      file.write(localSkeletonMap.at(component.value));
    }
  }

  {
    auto numComponents = static_cast<uint32_t>(asset.data.animators.size());
    file.write(numComponents);

    for (auto &animator : asset.data.animators) {
      file.write(animator.entity);

      auto numAnimations =
          static_cast<uint32_t>(animator.value.animations.size());
      file.write(numAnimations);
      for (auto &handle : animator.value.animations) {
        file.write(localAnimationMap.at(handle));
      }
    }
  }

  {
    auto numComponents =
        static_cast<uint32_t>(asset.data.directionalLights.size());
    file.write(numComponents);

    for (auto &light : asset.data.directionalLights) {
      file.write(light.entity);
      file.write(light.value.color);
      file.write(light.value.intensity);
    }
  }

  {
    auto numComponents = static_cast<uint32_t>(asset.data.pointLights.size());
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

Result<PrefabAssetHandle>
AssetCache::loadPrefabDataFromInputStream(InputBinaryStream &stream,
                                          const Path &filePath) {

  std::vector<String> warnings;

  AssetData<PrefabAsset> prefab{};
  prefab.path = filePath;
  prefab.relativePath = std::filesystem::relative(filePath, mAssetsPath);
  prefab.name = prefab.relativePath.string();
  prefab.type = AssetType::Prefab;

  std::vector<MeshAssetHandle> localMeshMap;

  {
    uint32_t numAssets = 0;
    stream.read(numAssets);
    std::vector<liquid::String> actual(numAssets);
    stream.read(actual);
    localMeshMap.resize(numAssets, MeshAssetHandle::Invalid);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto assetPathStr = actual.at(i);
      const auto &res = getOrLoadMeshFromPath(assetPathStr);
      if (res.hasData()) {
        localMeshMap.at(i) = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());
      } else {
        warnings.push_back(res.getError());
      }
    }
  }

  std::vector<SkinnedMeshAssetHandle> localSkinnedMeshMap;
  {
    uint32_t numAssets = 0;
    stream.read(numAssets);
    std::vector<liquid::String> actual(numAssets);
    stream.read(actual);
    localSkinnedMeshMap.resize(numAssets, SkinnedMeshAssetHandle::Invalid);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto assetPathStr = actual.at(i);
      const auto &res = getOrLoadSkinnedMeshFromPath(assetPathStr);
      if (res.hasData()) {
        localSkinnedMeshMap.at(i) = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());
      } else {
        warnings.push_back(res.getError());
      }
    }
  }

  std::vector<SkeletonAssetHandle> localSkeletonMap;
  {
    uint32_t numAssets = 0;
    stream.read(numAssets);
    std::vector<liquid::String> actual(numAssets);
    stream.read(actual);
    localSkeletonMap.resize(numAssets, SkeletonAssetHandle::Invalid);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto assetPathStr = actual.at(i);
      const auto &res = getOrLoadSkeletonFromPath(assetPathStr);
      if (res.hasData()) {
        localSkeletonMap.at(i) = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());
      } else {
        warnings.push_back(res.getError());
      }
    }
  }

  std::vector<AnimationAssetHandle> localAnimationMap;
  {
    auto &map = mRegistry.getAnimations();

    uint32_t numAssets = 0;
    stream.read(numAssets);
    std::vector<liquid::String> actual(numAssets);
    stream.read(actual);
    localAnimationMap.resize(numAssets, AnimationAssetHandle::Invalid);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto assetPathStr = actual.at(i);
      const auto &res = getOrLoadAnimationFromPath(assetPathStr);
      if (res.hasData()) {
        localAnimationMap.at(i) = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());

      } else {
        warnings.push_back(res.getError());
      }
    }
  }

  {
    uint32_t numComponents = 0;
    stream.read(numComponents);
    prefab.data.transforms.resize(numComponents);
    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 0;
      stream.read(entity);

      glm::vec3 position;
      glm::quat rotation;
      glm::vec3 scale;
      int32_t parent = -1;
      stream.read(position);
      stream.read(rotation);
      stream.read(scale);
      stream.read(parent);

      prefab.data.transforms.at(i).entity = entity;
      prefab.data.transforms.at(i).value = {position, rotation, scale, parent};
    }
  }

  {
    uint32_t numComponents = 0;
    stream.read(numComponents);

    prefab.data.meshes.resize(numComponents);

    auto &map = mRegistry.getMeshes();
    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 0;
      stream.read(entity);

      uint32_t meshIndex = 0;
      stream.read(meshIndex);

      prefab.data.meshes.at(i).entity = entity;
      prefab.data.meshes.at(i).value = localMeshMap.at(meshIndex);
    }
  }

  {
    uint32_t numComponents = 0;
    stream.read(numComponents);

    prefab.data.skinnedMeshes.resize(numComponents);

    auto &map = mRegistry.getSkinnedMeshes();
    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 0;
      stream.read(entity);

      uint32_t meshIndex = 0;
      stream.read(meshIndex);

      prefab.data.skinnedMeshes.at(i).entity = entity;
      prefab.data.skinnedMeshes.at(i).value = localSkinnedMeshMap.at(meshIndex);
    }
  }

  {
    uint32_t numComponents = 0;
    stream.read(numComponents);

    prefab.data.skeletons.resize(numComponents);

    auto &map = mRegistry.getSkeletons();
    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 0;
      stream.read(entity);

      uint32_t meshIndex = 0;
      stream.read(meshIndex);

      prefab.data.skeletons.at(i).entity = entity;
      prefab.data.skeletons.at(i).value = localSkeletonMap.at(meshIndex);
    }
  }

  {
    uint32_t numComponents = 0;
    stream.read(numComponents);
    auto &map = mRegistry.getAnimations();

    prefab.data.animators.resize(numComponents);

    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 0;
      stream.read(entity);

      uint32_t numAnimations = 0;
      stream.read(numAnimations);

      std::vector<uint32_t> animations(numAnimations);
      stream.read(animations);

      prefab.data.animators.at(i).entity = entity;
      prefab.data.animators.at(i).value.animations.resize(numAnimations);

      for (size_t j = 0; j < animations.size(); ++j) {
        prefab.data.animators.at(i).value.animations.at(j) =
            localAnimationMap.at(animations.at(j));
      }
    }
  }

  {
    uint32_t numComponents = 0;
    stream.read(numComponents);
    prefab.data.directionalLights.resize(numComponents);
    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 0;
      stream.read(entity);

      glm::vec4 color;
      float intensity = 0.0f;
      stream.read(color);
      stream.read(intensity);

      prefab.data.directionalLights.at(i).entity = entity;
      prefab.data.directionalLights.at(i).value = {color, intensity};
    }
  }

  {
    uint32_t numComponents = 0;
    stream.read(numComponents);
    prefab.data.pointLights.resize(numComponents);
    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 0;
      stream.read(entity);

      glm::vec4 color;
      float intensity = 0.0f;
      float range = 0.0f;
      stream.read(color);
      stream.read(intensity);
      stream.read(range);

      prefab.data.pointLights.at(i).entity = entity;
      prefab.data.pointLights.at(i).value = {color, intensity, range};
    }
  }

  if (prefab.data.transforms.empty() && prefab.data.directionalLights.empty() &&
      prefab.data.pointLights.empty() && prefab.data.meshes.empty() &&
      prefab.data.skinnedMeshes.empty() && prefab.data.skeletons.empty() &&
      prefab.data.animators.empty()) {
    return Result<PrefabAssetHandle>::Error("Prefab is empty");
  }

  return Result<PrefabAssetHandle>::Ok(mRegistry.getPrefabs().addAsset(prefab),
                                       warnings);
}

Result<PrefabAssetHandle> AssetCache::loadPrefabFromFile(const Path &filePath) {
  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::Prefab);
  if (header.hasError()) {
    return Result<PrefabAssetHandle>::Error(header.getError());
  }

  return loadPrefabDataFromInputStream(stream, filePath);
}

} // namespace liquid
