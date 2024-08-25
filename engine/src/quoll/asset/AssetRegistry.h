#pragma once

#include "quoll/animation/AnimationAsset.h"
#include "quoll/animation/AnimatorAsset.h"
#include "quoll/audio/AudioAsset.h"
#include "quoll/input/InputMapAsset.h"
#include "quoll/lua-scripting/LuaScriptAsset.h"
#include "quoll/renderer/MaterialAsset.h"
#include "quoll/renderer/MeshAsset.h"
#include "quoll/renderer/TextureAsset.h"
#include "quoll/scene/EnvironmentAsset.h"
#include "quoll/scene/PrefabAsset.h"
#include "quoll/scene/SceneAsset.h"
#include "quoll/skeleton/SkeletonAsset.h"
#include "quoll/text/FontAsset.h"
#include "AssetHandle.h"
#include "AssetMap.h"
#include "AssetType.h"

namespace quoll {

class RenderStorage;

/**
 * Registry of all the loaded assets
 *
 * All engine modules **directly** work with the registry since
 * the registry already has all the processed data in memory.
 */
class AssetRegistry : NoCopyMove {
  using TextureMap = AssetMap<TextureAsset>;
  using FontMap = AssetMap<FontAsset>;
  using MaterialMap = AssetMap<MaterialAsset>;
  using MeshMap = AssetMap<MeshAsset>;
  using SkeletonMap = AssetMap<SkeletonAsset>;
  using AnimationMap = AssetMap<AnimationAsset>;
  using AnimatorMap = AssetMap<AnimatorAsset>;
  using AudioMap = AssetMap<AudioAsset>;
  using PrefabMap = AssetMap<PrefabAsset>;
  using LuaScriptMap = AssetMap<LuaScriptAsset>;
  using EnvironmentMap = AssetMap<EnvironmentAsset>;
  using SceneMap = AssetMap<SceneAsset>;
  using InputMapMap = AssetMap<InputMapAsset>;

  struct DefaultObjects {
    AssetHandle<MeshAsset> cube;
    AssetHandle<MaterialAsset> defaultMaterial;
    AssetHandle<FontAsset> defaultFont;
  };

public:
  AssetRegistry() = default;

  ~AssetRegistry() = default;

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
