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

  template <typename TAssetData>
  const AssetData<TAssetData> &get(AssetHandle<TAssetData> handle) const {
    auto &map = getMap<TAssetData>();
    return map.getAsset(handle);
  }

  template <typename TAssetData>
  AssetData<TAssetData> &get(AssetHandle<TAssetData> handle) {
    auto &map = getMap<TAssetData>();
    return map.getAsset(handle);
  }

  template <typename TAssetData> bool has(AssetHandle<TAssetData> handle) {
    auto &map = getMap<TAssetData>();
    return map.hasAsset(handle);
  }

  template <typename TAssetData> void remove(AssetHandle<TAssetData> handle) {
    auto &map = getMap<TAssetData>();
    map.removeAsset(handle);
  }

  template <typename TAssetData>
  AssetHandle<TAssetData> add(const AssetData<TAssetData> &data) {
    auto &map = getMap<TAssetData>();
    return map.addAsset(data);
  }

  template <typename TAssetData>
  void update(AssetHandle<TAssetData> handle,
              const AssetData<TAssetData> &data) {
    auto &map = getMap<TAssetData>();
    map.updateAsset(handle, data);
  }

  template <typename TAssetData>
  AssetHandle<TAssetData> findHandleByUuid(const Uuid &uuid) {
    auto &map = getMap<TAssetData>();
    return map.findHandleByUuid(uuid);
  }

  template <typename TAssetData> usize count() {
    return getMap<TAssetData>().getAssets().size();
  }

private:
  template <typename TAssetData> constexpr auto &getMap() {
    if constexpr (std::is_same_v<TAssetData, TextureAsset>) {
      return mTextures;
    } else if constexpr (std::is_same_v<TAssetData, FontAsset>) {
      return mFonts;
    } else if constexpr (std::is_same_v<TAssetData, MaterialAsset>) {
      return mMaterials;
    } else if constexpr (std::is_same_v<TAssetData, MeshAsset>) {
      return mMeshes;
    } else if constexpr (std::is_same_v<TAssetData, SkeletonAsset>) {
      return mSkeletons;
    } else if constexpr (std::is_same_v<TAssetData, AnimationAsset>) {
      return mAnimations;
    } else if constexpr (std::is_same_v<TAssetData, AnimatorAsset>) {
      return mAnimators;
    } else if constexpr (std::is_same_v<TAssetData, AudioAsset>) {
      return mAudios;
    } else if constexpr (std::is_same_v<TAssetData, PrefabAsset>) {
      return mPrefabs;
    } else if constexpr (std::is_same_v<TAssetData, LuaScriptAsset>) {
      return mLuaScripts;
    } else if constexpr (std::is_same_v<TAssetData, EnvironmentAsset>) {
      return mEnvironments;
    } else if constexpr (std::is_same_v<TAssetData, SceneAsset>) {
      return mScenes;
    } else if constexpr (std::is_same_v<TAssetData, InputMapAsset>) {
      return mInputMaps;
    }
  }

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
