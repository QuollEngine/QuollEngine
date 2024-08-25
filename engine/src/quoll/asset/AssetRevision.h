#pragma once

#include "AssetType.h"

namespace quoll {

/**
 * Asset revisions
 *
 * Revisions are internal engine
 * versions of the asset that is typically
 * used to identify if an asset needs to be
 * recreated to be aligned with engine changes.
 *
 * Revision update values should be based on current date.
 *
 * Example: If current date is 2023-09-01, the
 *          value should be `230901`
 */
enum class AssetRevision : u32 {
  Material = 230901,
  Texture = 230901,
  Mesh = 230901,
  SkinnedMesh = 230901,
  Skeleton = 230901,
  Animation = 230901,
  Audio = 230901,
  Prefab = 230901,
  LuaScript = 230901,
  Font = 230901,
  Environment = 230901,
  Animator = 230901,
  InputMap = 230916,
  Scene = 230901
};

static constexpr AssetRevision getRevisionForAssetType(AssetType type) {
  switch (type) {
  case AssetType::Material:
    return AssetRevision::Material;
  case AssetType::Texture:
    return AssetRevision::Texture;
  case AssetType::Mesh:
    return AssetRevision::Mesh;
  case AssetType::SkinnedMesh:
    return AssetRevision::SkinnedMesh;
  case AssetType::Skeleton:
    return AssetRevision::Skeleton;
  case AssetType::Animation:
    return AssetRevision::Animation;
  case AssetType::Audio:
    return AssetRevision::Audio;
  case AssetType::Prefab:
    return AssetRevision::Prefab;
  case AssetType::LuaScript:
    return AssetRevision::LuaScript;
  case AssetType::Font:
    return AssetRevision::Font;
  case AssetType::Environment:
    return AssetRevision::Environment;
  case AssetType::Animator:
    return AssetRevision::Animator;
  case AssetType::InputMap:
    return AssetRevision::InputMap;
  case AssetType::Scene:
    return AssetRevision::Scene;
  default:
    return AssetRevision{0};
  }
}

} // namespace quoll
