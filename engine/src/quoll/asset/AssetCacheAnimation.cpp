#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "AssetFileHeader.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

Result<void> AssetCache::createAnimationFromData(const AnimationAsset &data,
                                                 const Path &assetPath) {
  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Error("File cannot be opened for writing: " + assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Animation;
  header.magic = AssetFileHeader::MagicConstant;
  file.write(header);

  file.write(data.time);
  u32 numKeyframes = static_cast<u32>(data.keyframes.size());
  file.write(numKeyframes);

  for (auto &keyframe : data.keyframes) {
    file.write(keyframe.target);
    file.write(keyframe.interpolation);
    file.write(keyframe.jointTarget);
    file.write(keyframe.joint);

    u32 numValues = static_cast<u32>(keyframe.keyframeTimes.size());
    file.write(numValues);
    file.write(keyframe.keyframeTimes);
    file.write(keyframe.keyframeValues);
  }

  return Ok();
}

Result<AnimationAsset>
AssetCache::loadAnimationDataFromInputStream(const Path &path) {
  InputBinaryStream stream(path);

  AssetFileHeader header;
  stream.read(header);
  if (header.magic != AssetFileHeader::MagicConstant ||
      header.type != AssetType::Animation) {
    return Error("Invalid file format");
  }

  AnimationAsset animation{};

  stream.read(animation.time);
  u32 numKeyframes = 0;
  stream.read(numKeyframes);
  animation.keyframes.resize(numKeyframes);

  for (auto &keyframe : animation.keyframes) {
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

  return animation;
}

Result<AnimationAsset> AssetCache::loadAnimation(const Uuid &uuid) {
  auto meta = getAssetMeta(uuid);
  if (meta.type != AssetType::Animation) {
    return Error("Asset type is not animation");
  }

  return loadAnimationDataFromInputStream(getPathFromUuid(uuid));
}

} // namespace quoll
