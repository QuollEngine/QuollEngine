#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "AssetFileHeader.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

Result<Path>
AssetCache::createAnimationFromAsset(const AssetData<AnimationAsset> &asset) {
  if (asset.uuid.isEmpty()) {
    QuollAssert(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  auto assetPath = getPathFromUuid(asset.uuid);

  auto metaRes = createAssetMeta(AssetType::Animation, asset.name, assetPath);
  if (!metaRes.hasData()) {
    return Result<Path>::Error("Cannot create animation asset: " + asset.name);
  }

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
  u32 numKeyframes = static_cast<u32>(asset.data.keyframes.size());
  file.write(numKeyframes);

  for (auto &keyframe : asset.data.keyframes) {
    file.write(keyframe.target);
    file.write(keyframe.interpolation);
    file.write(keyframe.jointTarget);
    file.write(keyframe.joint);

    u32 numValues = static_cast<u32>(keyframe.keyframeTimes.size());
    file.write(numValues);
    file.write(keyframe.keyframeTimes);
    file.write(keyframe.keyframeValues);
  }

  return Result<Path>::Ok(assetPath, {});
}

Result<AssetHandle<AnimationAsset>>
AssetCache::loadAnimationDataFromInputStream(InputBinaryStream &stream,
                                             const Path &filePath,
                                             const AssetFileHeader &header) {

  AssetData<AnimationAsset> animation{};
  animation.type = AssetType::Animation;
  animation.uuid = Uuid(filePath.stem().string());
  animation.name = header.name;

  stream.read(animation.data.time);
  u32 numKeyframes = 0;
  stream.read(numKeyframes);
  animation.data.keyframes.resize(numKeyframes);

  for (auto &keyframe : animation.data.keyframes) {
    stream.read(keyframe.target);
    stream.read(keyframe.interpolation);
    stream.read(keyframe.jointTarget);
    stream.read(keyframe.joint);

    u32 numValues = 0;
    stream.read(numValues);
    keyframe.keyframeTimes.resize(numValues);
    keyframe.keyframeValues.resize(numValues);
    stream.read(keyframe.keyframeTimes);
    stream.read(keyframe.keyframeValues);
  }

  return Result<AssetHandle<AnimationAsset>>::Ok(mRegistry.add(animation));
}

Result<AssetHandle<AnimationAsset>>
AssetCache::loadAnimation(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::Animation);
  if (header.hasError()) {
    return Result<AssetHandle<AnimationAsset>>::Error(header.getError());
  }

  return loadAnimationDataFromInputStream(stream, filePath, header.getData());
}

Result<AssetHandle<AnimationAsset>>
AssetCache::getOrLoadAnimation(const Uuid &uuid) {
  if (uuid.isEmpty()) {
    return Result<AssetHandle<AnimationAsset>>::Ok(
        AssetHandle<AnimationAsset>());
  }

  auto handle = mRegistry.findHandleByUuid<AnimationAsset>(uuid);
  if (handle) {
    return Result<AssetHandle<AnimationAsset>>::Ok(handle);
  }

  return loadAnimation(uuid);
}

} // namespace quoll
