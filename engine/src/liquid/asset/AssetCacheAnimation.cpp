#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetCache.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

Result<Path>
AssetCache::createAnimationFromAsset(const AssetData<AnimationAsset> &asset) {
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
  header.type = AssetType::Animation;
  header.magic = AssetFileHeader::MagicConstant;
  header.name = asset.name;
  file.write(header);

  file.write(asset.data.time);
  uint32_t numKeyframes = static_cast<uint32_t>(asset.data.keyframes.size());
  file.write(numKeyframes);

  for (auto &keyframe : asset.data.keyframes) {
    file.write(keyframe.target);
    file.write(keyframe.interpolation);
    file.write(keyframe.jointTarget);
    file.write(keyframe.joint);

    uint32_t numValues = static_cast<uint32_t>(keyframe.keyframeTimes.size());
    file.write(numValues);
    file.write(keyframe.keyframeTimes);
    file.write(keyframe.keyframeValues);
  }

  return Result<Path>::Ok(assetPath, {});
}

Result<AnimationAssetHandle>
AssetCache::loadAnimationDataFromInputStream(InputBinaryStream &stream,
                                             const Path &filePath,
                                             const AssetFileHeader &header) {

  AssetData<AnimationAsset> animation{};
  animation.path = filePath;
  animation.type = AssetType::Animation;
  animation.uuid = Uuid(filePath.stem().string());
  animation.name = header.name;

  stream.read(animation.data.time);
  uint32_t numKeyframes = 0;
  stream.read(numKeyframes);
  animation.data.keyframes.resize(numKeyframes);

  for (auto &keyframe : animation.data.keyframes) {
    stream.read(keyframe.target);
    stream.read(keyframe.interpolation);
    stream.read(keyframe.jointTarget);
    stream.read(keyframe.joint);

    uint32_t numValues = 0;
    stream.read(numValues);
    keyframe.keyframeTimes.resize(numValues);
    keyframe.keyframeValues.resize(numValues);
    stream.read(keyframe.keyframeTimes);
    stream.read(keyframe.keyframeValues);
  }

  return Result<AnimationAssetHandle>::Ok(
      mRegistry.getAnimations().addAsset(animation));
}

Result<AnimationAssetHandle> AssetCache::loadAnimation(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::Animation);
  if (header.hasError()) {
    return Result<AnimationAssetHandle>::Error(header.getError());
  }

  return loadAnimationDataFromInputStream(stream, filePath, header.getData());
}

Result<AnimationAssetHandle> AssetCache::getOrLoadAnimation(const Uuid &uuid) {
  if (uuid.isEmpty()) {
    return Result<AnimationAssetHandle>::Ok(AnimationAssetHandle::Null);
  }

  auto handle = mRegistry.getAnimations().findHandleByUuid(uuid);
  if (handle != AnimationAssetHandle::Null) {
    return Result<AnimationAssetHandle>::Ok(handle);
  }

  return loadAnimation(uuid);
}

} // namespace liquid
