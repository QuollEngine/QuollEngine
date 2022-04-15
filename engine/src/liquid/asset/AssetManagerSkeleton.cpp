#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetManager.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

Result<std::filesystem::path>
AssetManager::createSkeletonFromAsset(const AssetData<SkeletonAsset> &asset) {
  String extension = ".lqskel";
  std::filesystem::path assetPath = (mAssetsPath / (asset.name + extension));
  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<std::filesystem::path>::Error(
        "File cannot be opened for writing: " + assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Skeleton;
  header.version = createVersion(0, 1);
  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

  auto numJoints = static_cast<uint32_t>(asset.data.jointLocalPositions.size());
  file.write(numJoints);

  file.write(asset.data.jointLocalPositions);
  file.write(asset.data.jointLocalRotations);
  file.write(asset.data.jointLocalScales);
  file.write(asset.data.jointParents);
  file.write(asset.data.jointInverseBindMatrices);
  file.write(asset.data.jointNames);

  return Result<std::filesystem::path>::Ok(assetPath);
}

Result<SkeletonAssetHandle>
AssetManager::loadSkeletonFromFile(const std::filesystem::path &filePath) {
  InputBinaryStream file(filePath);

  const auto &header = checkAssetFile(file, filePath, AssetType::Skeleton);
  if (header.hasError()) {
    return Result<SkeletonAssetHandle>::Error(header.getError());
  }

  AssetData<SkeletonAsset> skeleton{};
  skeleton.path = filePath;
  skeleton.name = filePath.filename().string();
  skeleton.type = AssetType::Skeleton;

  uint32_t numJoints = 0;
  file.read(numJoints);

  skeleton.data.jointLocalPositions.resize(numJoints);
  skeleton.data.jointLocalRotations.resize(numJoints);
  skeleton.data.jointLocalScales.resize(numJoints);
  skeleton.data.jointParents.resize(numJoints);
  skeleton.data.jointInverseBindMatrices.resize(numJoints);
  skeleton.data.jointNames.resize(numJoints);

  file.read(skeleton.data.jointLocalPositions);
  file.read(skeleton.data.jointLocalRotations);
  file.read(skeleton.data.jointLocalScales);
  file.read(skeleton.data.jointParents);
  file.read(skeleton.data.jointInverseBindMatrices);
  file.read(skeleton.data.jointNames);

  return Result<SkeletonAssetHandle>::Ok(
      mRegistry.getSkeletons().addAsset(skeleton));
}

Result<SkeletonAssetHandle>
AssetManager::getOrLoadSkeletonFromPath(const String &relativePath) {
  if (relativePath.empty()) {
    return Result<SkeletonAssetHandle>::Ok(SkeletonAssetHandle::Invalid);
  }

  std::filesystem::path fullPath =
      (mAssetsPath / relativePath).make_preferred();

  for (auto &[handle, asset] : mRegistry.getSkeletons().getAssets()) {
    if (asset.path == fullPath) {
      return Result<SkeletonAssetHandle>::Ok(handle);
    }
  }

  return loadSkeletonFromFile(fullPath);
}

} // namespace liquid
