#pragma once

#include "Asset.h"

namespace quoll {

enum class AssetRevision : uint32_t {
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

/**
 * @brief Get revision for asset type
 *
 * @param type Asset type
 * @return Asset revision
 */
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
