#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetCache.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace quoll {

Result<Path>
AssetCache::createSkeletonFromAsset(const AssetData<SkeletonAsset> &asset) {
  if (asset.uuid.isEmpty()) {
    LIQUID_ASSERT(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  auto assetPath = getPathFromUuid(asset.uuid);

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Skeleton;
  header.magic = AssetFileHeader::MagicConstant;
  header.name = asset.name;
  file.write(header);

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
                                            const Path &filePath,
                                            const AssetFileHeader &header) {
  AssetData<SkeletonAsset> skeleton{};
  skeleton.path = filePath;
  skeleton.type = AssetType::Skeleton;
  skeleton.uuid = Uuid(filePath.stem().string());
  skeleton.name = header.name;

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

Result<SkeletonAssetHandle> AssetCache::loadSkeleton(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);
  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::Skeleton);
  if (header.hasError()) {
    return Result<SkeletonAssetHandle>::Error(header.getError());
  }

  return loadSkeletonDataFromInputStream(stream, filePath, header.getData());
}

Result<SkeletonAssetHandle> AssetCache::getOrLoadSkeleton(const Uuid &uuid) {
  if (uuid.isEmpty()) {
    return Result<SkeletonAssetHandle>::Ok(SkeletonAssetHandle::Null);
  }

  auto handle = mRegistry.getSkeletons().findHandleByUuid(uuid);
  if (handle != SkeletonAssetHandle::Null) {
    return Result<SkeletonAssetHandle>::Ok(handle);
  }

  return loadSkeleton(uuid);
}

} // namespace quoll
