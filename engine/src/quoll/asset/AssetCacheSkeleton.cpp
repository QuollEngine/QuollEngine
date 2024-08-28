#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/skeleton/SkeletonAsset.h"
#include "AssetCache.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

Result<void> AssetCache::createSkeletonFromData(const SkeletonAsset &data,
                                                const Path &assetPath) {
  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Error("File cannot be opened for writing: " + assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Skeleton;
  header.magic = AssetFileHeader::MagicConstant;
  file.write(header);

  auto numJoints = static_cast<u32>(data.jointLocalPositions.size());
  file.write(numJoints);

  file.write(data.jointLocalPositions);
  file.write(data.jointLocalRotations);
  file.write(data.jointLocalScales);
  file.write(data.jointParents);
  file.write(data.jointInverseBindMatrices);
  file.write(data.jointNames);

  return Ok();
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
