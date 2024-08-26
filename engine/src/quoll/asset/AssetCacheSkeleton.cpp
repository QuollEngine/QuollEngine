#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/skeleton/SkeletonAsset.h"
#include "AssetCache.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

Result<Path>
AssetCache::createSkeletonFromAsset(const AssetData<SkeletonAsset> &asset) {
  if (asset.uuid.isEmpty()) {
    QuollAssert(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  auto assetPath = getPathFromUuid(asset.uuid);
  auto metaRes = createAssetMeta(AssetType::Skeleton, asset.name, assetPath);
  if (!metaRes.hasData()) {
    return Result<Path>::Error("Cannot create skeleton asset: " + asset.name);
  }

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Skeleton;
  header.magic = AssetFileHeader::MagicConstant;
  file.write(header);

  auto numJoints = static_cast<u32>(asset.data.jointLocalPositions.size());
  file.write(numJoints);

  file.write(asset.data.jointLocalPositions);
  file.write(asset.data.jointLocalRotations);
  file.write(asset.data.jointLocalScales);
  file.write(asset.data.jointParents);
  file.write(asset.data.jointInverseBindMatrices);
  file.write(asset.data.jointNames);

  return Result<Path>::Ok(assetPath);
}

Result<AssetHandle<SkeletonAsset>>
AssetCache::loadSkeletonDataFromInputStream(const Path &path, const Uuid &uuid,
                                            const AssetMeta &meta) {
  InputBinaryStream stream(path);
  AssetFileHeader header;
  stream.read(header);
  if (header.magic != AssetFileHeader::MagicConstant ||
      header.type != AssetType::Skeleton) {
    return Result<AssetHandle<SkeletonAsset>>::Error("Invalid file format");
  }

  AssetData<SkeletonAsset> skeleton{};
  skeleton.type = AssetType::Skeleton;
  skeleton.uuid = uuid;
  skeleton.name = meta.name;

  u32 numJoints = 0;
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

  return Result<AssetHandle<SkeletonAsset>>::Ok(mRegistry.add(skeleton));
}

Result<AssetHandle<SkeletonAsset>> AssetCache::loadSkeleton(const Uuid &uuid) {
  auto meta = getAssetMeta(uuid);
  if (meta.type != AssetType::Skeleton) {
    return Result<AssetHandle<SkeletonAsset>>::Error(
        "Asset type is not skeleton");
  }

  return loadSkeletonDataFromInputStream(getPathFromUuid(uuid), uuid, meta);
}

Result<AssetHandle<SkeletonAsset>>
AssetCache::getOrLoadSkeleton(const Uuid &uuid) {
  if (uuid.isEmpty()) {
    return Result<AssetHandle<SkeletonAsset>>::Ok(AssetHandle<SkeletonAsset>());
  }

  auto handle = mRegistry.findHandleByUuid<SkeletonAsset>(uuid);
  if (handle) {
    return Result<AssetHandle<SkeletonAsset>>::Ok(handle);
  }

  return loadSkeleton(uuid);
}

} // namespace quoll
