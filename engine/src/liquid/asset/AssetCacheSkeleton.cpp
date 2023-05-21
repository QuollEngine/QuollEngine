#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetCache.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

Result<Path>
AssetCache::createSkeletonFromAsset(const AssetData<SkeletonAsset> &asset) {
  String extension = ".lqskel";
  Path assetPath = (mAssetsPath / (asset.name + extension)).make_preferred();
  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Skeleton;
  header.version = createVersion(0, 1);
  file.write(header.magic, AssetFileMagicLength);
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

  return Result<Path>::Ok(assetPath);
}

Result<SkeletonAssetHandle>
AssetCache::loadSkeletonDataFromInputStream(InputBinaryStream &stream,
                                            const Path &filePath) {
  auto assetName = std::filesystem::relative(filePath, mAssetsPath).string();

  AssetData<SkeletonAsset> skeleton{};
  skeleton.path = filePath;
  skeleton.relativePath = std::filesystem::relative(filePath, mAssetsPath);
  skeleton.name = skeleton.relativePath.string();
  skeleton.type = AssetType::Skeleton;

  uint32_t numJoints = 0;
  stream.read(numJoints);

  skeleton.data.jointLocalPositions.resize(numJoints);
  skeleton.data.jointLocalRotations.resize(numJoints);
  skeleton.data.jointLocalScales.resize(numJoints);
  skeleton.data.jointParents.resize(numJoints);
  skeleton.data.jointInverseBindMatrices.resize(numJoints);
  skeleton.data.jointNames.resize(numJoints);

  stream.read(skeleton.data.jointLocalPositions);
  stream.read(skeleton.data.jointLocalRotations);
  stream.read(skeleton.data.jointLocalScales);
  stream.read(skeleton.data.jointParents);
  stream.read(skeleton.data.jointInverseBindMatrices);
  stream.read(skeleton.data.jointNames);

  return Result<SkeletonAssetHandle>::Ok(
      mRegistry.getSkeletons().addAsset(skeleton));
}

Result<SkeletonAssetHandle>
AssetCache::loadSkeletonFromFile(const Path &filePath) {
  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::Skeleton);
  if (header.hasError()) {
    return Result<SkeletonAssetHandle>::Error(header.getError());
  }

  return loadSkeletonDataFromInputStream(stream, filePath);
}

Result<SkeletonAssetHandle>
AssetCache::getOrLoadSkeletonFromPath(StringView relativePath) {
  if (relativePath.empty()) {
    return Result<SkeletonAssetHandle>::Ok(SkeletonAssetHandle::Null);
  }

  Path fullPath = (mAssetsPath / relativePath).make_preferred();

  for (auto &[handle, asset] : mRegistry.getSkeletons().getAssets()) {
    if (asset.path == fullPath) {
      return Result<SkeletonAssetHandle>::Ok(handle);
    }
  }

  return loadSkeletonFromFile(fullPath);
}

} // namespace liquid
