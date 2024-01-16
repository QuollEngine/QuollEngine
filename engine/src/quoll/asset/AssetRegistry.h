#pragma once

#include "quoll/renderer/RenderStorage.h"
#include "quoll/rhi/RenderDevice.h"
#include "AnimationAsset.h"
#include "AnimatorAsset.h"
#include "Asset.h"
#include "AssetMap.h"
#include "AudioAsset.h"
#include "EnvironmentAsset.h"
#include "FontAsset.h"
#include "InputMapAsset.h"
#include "LuaScriptAsset.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"
#include "PrefabAsset.h"
#include "SceneAsset.h"
#include "SkeletonAsset.h"
#include "TextureAsset.h"

namespace quoll {

/**
 * Registry of all the loaded assets
 *
 * All engine modules **directly** work with the registry since
 * the registry already has all the processed data in memory.
 */
class AssetRegistry {
  using TextureMap = AssetMap<TextureAssetHandle, TextureAsset>;
  using FontMap = AssetMap<FontAssetHandle, FontAsset>;
  using MaterialMap = AssetMap<MaterialAssetHandle, MaterialAsset>;
  using MeshMap = AssetMap<MeshAssetHandle, MeshAsset>;
  using SkeletonMap = AssetMap<SkeletonAssetHandle, SkeletonAsset>;
  using AnimationMap = AssetMap<AnimationAssetHandle, AnimationAsset>;
  using AnimatorMap = AssetMap<AnimatorAssetHandle, AnimatorAsset>;
  using AudioMap = AssetMap<AudioAssetHandle, AudioAsset>;
  using PrefabMap = AssetMap<PrefabAssetHandle, PrefabAsset>;
  using LuaScriptMap = AssetMap<LuaScriptAssetHandle, LuaScriptAsset>;
  using EnvironmentMap = AssetMap<EnvironmentAssetHandle, EnvironmentAsset>;
  using SceneMap = AssetMap<SceneAssetHandle, SceneAsset>;
  using InputMapMap = AssetMap<InputMapAssetHandle, InputMapAsset>;

  struct DefaultObjects {
    MeshAssetHandle cube = MeshAssetHandle::Null;
    MaterialAssetHandle defaultMaterial = MaterialAssetHandle::Null;
    FontAssetHandle defaultFont = FontAssetHandle::Null;
  };

public:
  AssetRegistry() = default;

  ~AssetRegistry() = default;

  AssetRegistry(const AssetRegistry &) = delete;
  AssetRegistry &operator=(const AssetRegistry &) = delete;
  AssetRegistry(AssetRegistry &&) = delete;
  AssetRegistry &operator=(AssetRegistry &&) = delete;

  void createDefaultObjects();

  inline const DefaultObjects &getDefaultObjects() const {
    return mDefaultObjects;
  }

  void syncWithDevice(RenderStorage &renderStorage);

  inline TextureMap &getTextures() { return mTextures; }

  inline FontMap &getFonts() { return mFonts; }

  inline MaterialMap &getMaterials() { return mMaterials; }

  inline MeshMap &getMeshes() { return mMeshes; }

  inline SkeletonMap &getSkeletons() { return mSkeletons; }

  inline AnimationMap &getAnimations() { return mAnimations; }

  inline AnimatorMap &getAnimators() { return mAnimators; }

  inline AudioMap &getAudios() { return mAudios; }

  inline PrefabMap &getPrefabs() { return mPrefabs; }

  inline LuaScriptMap &getLuaScripts() { return mLuaScripts; }

  inline EnvironmentMap &getEnvironments() { return mEnvironments; }

  inline SceneMap &getScenes() { return mScenes; }

  inline InputMapMap &getInputMaps() { return mInputMaps; }

  std::pair<AssetType, u32> getAssetByUuid(const Uuid &uuid);

private:
  TextureMap mTextures;
  FontMap mFonts;
  MaterialMap mMaterials;
  MeshMap mMeshes;
  SkeletonMap mSkeletons;
  AnimationMap mAnimations;
  AnimatorMap mAnimators;
  AudioMap mAudios;
  PrefabMap mPrefabs;
  LuaScriptMap mLuaScripts;
  EnvironmentMap mEnvironments;
  SceneMap mScenes;
  InputMapMap mInputMaps;

  DefaultObjects mDefaultObjects;
};

} // namespace quoll
