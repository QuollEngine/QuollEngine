#pragma once

#include "Asset.h"

namespace liquid {

enum class AssetRevision : uint32_t {
  Material = 230811,
  Texture = 230811,
  Mesh = 230811,
  SkinnedMesh = 230811,
  Skeleton = 230811,
  Animation = 230811,
  Audio = 230811,
  Prefab = 230811,
  LuaScript = 230811,
  Font = 230811,
  Environment = 230811,
  Animator = 230811
};

/**
 * @brief Get revision for asset type
 *
 * @param type Asset type
 * @return Asset revsion
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
  default:
    return AssetRevision{0};
  }
}

} // namespace liquid
