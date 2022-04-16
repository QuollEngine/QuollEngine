#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetManager.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

Result<std::filesystem::path>
AssetManager::createPrefabFromAsset(const AssetData<PrefabAsset> &asset) {
  String extension = ".lqprefab";
  std::filesystem::path assetPath = (mAssetsPath / (asset.name + extension));
  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<std::filesystem::path>::Error(
        "File cannot be opened for writing: " + assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Prefab;
  header.version = createVersion(0, 1);
  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

  // Load assets
  uint32_t numMeshes = static_cast<uint32_t>(asset.data.meshes.size());
  file.write(numMeshes);

  for (auto &component : asset.data.meshes) {
    auto &mesh = mRegistry.getMeshes().getAsset(component.value);
    auto path = std::filesystem::relative(mesh.path, mAssetsPath).string();
    std::replace(path.begin(), path.end(), '\\', '/');
    file.write(path);
  }

  uint32_t numSkinnedMeshes =
      static_cast<uint32_t>(asset.data.skinnedMeshes.size());
  file.write(numSkinnedMeshes);

  for (auto &component : asset.data.skinnedMeshes) {
    auto &mesh = mRegistry.getSkinnedMeshes().getAsset(component.value);
    auto path = std::filesystem::relative(mesh.path, mAssetsPath).string();
    std::replace(path.begin(), path.end(), '\\', '/');
    file.write(path);
  }

  uint32_t numSkeletons = static_cast<uint32_t>(asset.data.skeletons.size());
  file.write(numSkeletons);

  for (auto &component : asset.data.skeletons) {
    auto &skeleton = mRegistry.getSkeletons().getAsset(component.value);
    auto path = std::filesystem::relative(skeleton.path, mAssetsPath).string();
    std::replace(path.begin(), path.end(), '\\', '/');
    file.write(path);
  }

  uint32_t numAnimations = 0;
  for (auto &animator : asset.data.animators) {
    numAnimations += static_cast<uint32_t>(animator.value.animations.size());
  }
  file.write(numAnimations);

  std::map<AnimationAssetHandle, uint32_t> localMap;
  uint32_t lastId = 0;

  for (auto &component : asset.data.animators) {
    for (auto &handle : component.value.animations) {
      auto &animation = mRegistry.getAnimations().getAsset(handle);
      auto path =
          std::filesystem::relative(animation.path, mAssetsPath).string();
      std::replace(path.begin(), path.end(), '\\', '/');
      file.write(path);

      if (localMap.find(handle) == localMap.end()) {
        localMap.insert_or_assign(handle, lastId++);
      }
    }
  }

  // Load component data
  uint32_t dummySize = 0;
  // Size for transforms
  file.write(dummySize);

  // Size for meshes
  file.write(dummySize);

  // Size for skinned meshes
  file.write(dummySize);

  // Size for skeletons
  file.write(dummySize);

  auto numAnimators = static_cast<uint32_t>(asset.data.animators.size());
  file.write(numAnimators);

  for (auto &animator : asset.data.animators) {
    file.write(animator.entity);

    auto numAnimations =
        static_cast<uint32_t>(animator.value.animations.size());
    file.write(numAnimations);
    for (auto &handle : animator.value.animations) {
      file.write(localMap.at(handle));
    }
  }

  return Result<std::filesystem::path>::Ok(assetPath);
}

Result<PrefabAssetHandle>
AssetManager::loadPrefabFromFile(const std::filesystem::path &filePath) {
  InputBinaryStream file(filePath);

  const auto &header = checkAssetFile(file, filePath, AssetType::Prefab);
  if (header.hasError()) {
    return Result<PrefabAssetHandle>::Error(header.getError());
  }

  std::vector<String> warnings;

  AssetData<PrefabAsset> prefab{};
  prefab.path = filePath;
  prefab.name = filePath.filename().string();
  prefab.type = AssetType::Prefab;

  {
    auto &component = prefab.data.meshes;
    uint32_t numAssets = 0;
    file.read(numAssets);
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);
    component.resize(numAssets);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto assetPathStr = actual.at(i);
      const auto &res = getOrLoadMeshFromPath(assetPathStr);
      if (res.hasData()) {
        component.at(i).value = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());
      } else {
        warnings.push_back("Cannot load mesh in path: " + assetPathStr);
      }
    }
  }

  {
    auto &component = prefab.data.skinnedMeshes;

    uint32_t numAssets = 0;
    file.read(numAssets);
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);
    component.resize(numAssets);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto assetPathStr = actual.at(i);
      const auto &res = getOrLoadSkinnedMeshFromPath(assetPathStr);
      if (res.hasData()) {
        component.at(i).value = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());
      } else {
        warnings.push_back("Cannot skinned load mesh in path: " + assetPathStr);
      }
    }
  }

  {
    auto &component = prefab.data.skeletons;

    uint32_t numAssets = 0;
    file.read(numAssets);
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);
    component.resize(numAssets);
    for (uint32_t i = 0; i < numAssets; ++i) {
      auto assetPathStr = actual.at(i);
      const auto &res = getOrLoadSkeletonFromPath(assetPathStr);
      if (res.hasData()) {
        component.at(i).value = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());
      } else {
        warnings.push_back("Cannot load skeleton in path: " + assetPathStr);
      }
    }
  }

  std::vector<AnimationAssetHandle> localMap;
  {
    auto &map = mRegistry.getAnimations();

    uint32_t numAssets = 0;
    file.read(numAssets);
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);
    localMap.resize(numAssets);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto assetPathStr = actual.at(i);
      const auto &res = getOrLoadAnimationFromPath(assetPathStr);
      if (res.hasData()) {
        localMap.at(i) = res.getData();
        warnings.insert(warnings.end(), res.getWarnings().begin(),
                        res.getWarnings().end());

      } else {
        warnings.push_back("Cannot load animation in path: " + assetPathStr);
      }
    }
  }

  uint32_t dummy = 0;
  file.read(dummy);
  file.read(dummy);
  file.read(dummy);
  file.read(dummy);

  {
    uint32_t numComponents = 0;
    file.read(numComponents);
    auto &map = mRegistry.getAnimations();

    prefab.data.animators.resize(numComponents);

    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 0;
      file.read(entity);

      uint32_t numAnimations = 0;
      file.read(numAnimations);

      std::vector<uint32_t> animations(numAnimations);
      file.read(animations);

      prefab.data.animators.at(i).value.animations.resize(numAnimations);

      for (size_t j = 0; j < animations.size(); ++j) {
        prefab.data.animators.at(i).value.animations.at(j) =
            localMap.at(animations.at(j));
      }
    }
  }

  return Result<PrefabAssetHandle>::Ok(mRegistry.getPrefabs().addAsset(prefab),
                                       warnings);
}

} // namespace liquid
