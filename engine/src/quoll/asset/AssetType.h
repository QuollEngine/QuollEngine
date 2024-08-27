#pragma once

namespace quoll {

enum class AssetType : u8 {
  None,
  Material,
  Texture,
  Mesh,
  Skeleton,
  Animation,
  Audio,
  Prefab,
  LuaScript,
  Font,
  Environment,
  Animator,
  Scene,
  InputMap
};

inline const String getAssetTypeString(AssetType type) {
  switch (type) {
  case AssetType::Material:
    return "material";
  case AssetType::Texture:
    return "texture";
  case AssetType::Font:
    return "font";
  case AssetType::Mesh:
    return "mesh";
  case AssetType::Animation:
    return "animation";
  case AssetType::Animator:
    return "animator";
  case AssetType::Environment:
    return "environment";
  case AssetType::Audio:
    return "audio";
  case AssetType::Prefab:
    return "prefab";
  case AssetType::LuaScript:
    return "lua script";
  case AssetType::Scene:
    return "scene";
  case AssetType::InputMap:
    return "input map";
  default:
    return "none";
  }
}

} // namespace quoll
