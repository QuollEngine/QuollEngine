#pragma once

#include "Asset.h"

namespace liquid {

enum class AssetRevision : uint32_t {
  Material = 230911,
  Texture = 230911,
  Mesh = 230915,
  SkinnedMesh = 230915,
  Skeleton = 230911,
  Animation = 230923,
  Audio = 230911,
  Prefab = 230823,
  LuaScript = 230911,
  Font = 230911,
  Environment = 230911,
  Animator = 230911,
  Scene = 230820
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
  case AssetType::Scene:
    return AssetRevision::Scene;
  default:
    return AssetRevision{0};
  }
}

} // namespace liquid
