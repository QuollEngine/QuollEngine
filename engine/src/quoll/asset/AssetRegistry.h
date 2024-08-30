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
#include "Result.h"

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

  std::pair<AssetType, u32> getAssetByUuid(const Uuid &uuid);

  template <typename TAssetData>
  constexpr const TAssetData &get(AssetHandle<TAssetData> handle) const {
    auto &map = getMap<TAssetData>();
    return map.getAsset(handle).data;
  }

  template <typename TAssetData>
  constexpr TAssetData &get(AssetHandle<TAssetData> handle) {
    auto &map = getMap<TAssetData>();
    return map.getAsset(handle).data;
  }

  template <typename TAssetData>
  constexpr const AssetData<TAssetData> &
  getMeta(AssetHandle<TAssetData> handle) const {
    auto &map = getMap<TAssetData>();
    return map.getAsset(handle);
  }

  template <typename TAssetData>
  constexpr AssetData<TAssetData> &getMeta(AssetHandle<TAssetData> handle) {
    auto &map = getMap<TAssetData>();
    return map.getAsset(handle);
  }

  template <typename TAssetData>
  constexpr bool has(AssetHandle<TAssetData> handle) {
    auto &map = getMap<TAssetData>();
    return map.hasAsset(handle);
  }

  template <typename TAssetData>
  constexpr void remove(AssetHandle<TAssetData> handle) {
    auto &map = getMap<TAssetData>();
    map.removeAsset(handle);
  }

  template <typename TAssetData>
  constexpr AssetHandle<TAssetData> add(const AssetData<TAssetData> &data) {
    auto &map = getMap<TAssetData>();
    return map.addAsset(data);
  }

  template <typename TAssetData>
  constexpr void update(AssetHandle<TAssetData> handle,
                        const AssetData<TAssetData> &data) {
    auto &map = getMap<TAssetData>();
    map.updateAsset(handle, data);
  }

  template <typename TAssetData>
  constexpr AssetHandle<TAssetData> findHandleByUuid(const Uuid &uuid) {
    auto &map = getMap<TAssetData>();
    return map.findHandleByUuid(uuid);
  }

  template <typename TAssetData> constexpr usize count() {
    return getMap<TAssetData>().getAssets().size();
  }

  template <typename TAssetData> constexpr auto &getAll() {
    return getMap<TAssetData>().getAssets();
  }

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
