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
    return Error("Invalid uuid provided");
  }

  auto assetPath = getPathFromUuid(asset.uuid);
  auto metaRes = createAssetMeta(AssetType::Skeleton, asset.name, assetPath);
  if (!metaRes) {
    return Error("Cannot create skeleton asset: " + asset.name);
  }

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Error("File cannot be opened for writing: " + assetPath.string());
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

  return assetPath;
}

Result<SkeletonAsset>
AssetCache::loadSkeletonDataFromInputStream(const Path &path) {
  InputBinaryStream stream(path);
  AssetFileHeader header;
  stream.read(header);
  if (header.magic != AssetFileHeader::MagicConstant ||
      header.type != AssetType::Skeleton) {
    return Error("Invalid file format");
  }

  SkeletonAsset skeleton{};

  u32 numJoints = 0;
  stream.read(numJoints);

  skeleton.jointLocalPositions.resize(numJoints);
  skeleton.jointLocalRotations.resize(numJoints);
  skeleton.jointLocalScales.resize(numJoints);
  skeleton.jointParents.resize(numJoints);
  skeleton.jointInverseBindMatrices.resize(numJoints);
  skeleton.jointNames.resize(numJoints);

  stream.read(skeleton.jointLocalPositions);
  stream.read(skeleton.jointLocalRotations);
  stream.read(skeleton.jointLocalScales);
  stream.read(skeleton.jointParents);
  stream.read(skeleton.jointInverseBindMatrices);
  stream.read(skeleton.jointNames);

  return skeleton;
}

Result<SkeletonAsset> AssetCache::loadSkeleton(const Uuid &uuid) {
  auto meta = getAssetMeta(uuid);
  if (meta.type != AssetType::Skeleton) {
    return Error("Asset type is not skeleton");
  }

  return loadSkeletonDataFromInputStream(getPathFromUuid(uuid));
}

} // namespace quoll
