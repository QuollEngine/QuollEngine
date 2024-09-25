#pragma once

#include "quoll/core/Result.h"
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

/**
 * Registry of all the loaded assets
 *
 * All engine modules **directly** work with the registry since
 * the registry already has all the processed data in memory.
 */
class AssetRegistry : NoCopyMove {
  using TextureMap = AssetMap<TextureAsset>;
  using MaterialMap = AssetMap<MaterialAsset>;
  using MeshMap = AssetMap<MeshAsset>;
  using SkeletonMap = AssetMap<SkeletonAsset>;
  using AnimationMap = AssetMap<AnimationAsset>;
  using AnimatorMap = AssetMap<AnimatorAsset>;
  using AudioMap = AssetMap<AudioAsset>;
  using FontMap = AssetMap<FontAsset>;
  using LuaScriptMap = AssetMap<LuaScriptAsset>;
  using InputMapMap = AssetMap<InputMapAsset>;
  using EnvironmentMap = AssetMap<EnvironmentAsset>;
  using PrefabMap = AssetMap<PrefabAsset>;
  using SceneMap = AssetMap<SceneAsset>;

  struct DefaultObjects {
    AssetRef<MeshAsset> cube;
    AssetRef<MaterialAsset> defaultMaterial;
    AssetRef<FontAsset> defaultFont;
  };

public:
  void createDefaultObjects();

  inline const DefaultObjects &getDefaultObjects() const {
    return mDefaultObjects;
  }

  template <typename TAssetData>
  constexpr const TAssetData &get(AssetHandle<TAssetData> handle) const {
    const auto &map = getMap<TAssetData>();
    return map.get(handle);
  }

  template <typename TAssetData>
  constexpr const AssetMeta &getMeta(AssetHandle<TAssetData> handle) const {
    const auto &map = getMap<TAssetData>();
    return map.getMeta(handle);
  }

  template <typename TAssetData>
  constexpr bool has(AssetHandle<TAssetData> handle) const {
    const auto &map = getMap<TAssetData>();
    return map.contains(handle);
  }

  template <typename TAssetData>
  constexpr AssetHandle<TAssetData> allocate(const AssetMeta &meta) {
    auto &map = getMap<TAssetData>();
    return map.allocate(meta);
  }

  template <typename TAssetData> constexpr void destroy(const Uuid &uuid) {
    auto &map = getMap<TAssetData>();
    return map.destroy(uuid);
  }

  template <typename TAssetData>
  constexpr void store(AssetHandle<TAssetData> handle, const TAssetData &data) {
    auto &map = getMap<TAssetData>();
    map.store(handle, data);
  }

  template <typename TAssetData>
  constexpr AssetHandle<TAssetData> findHandleByUuid(const Uuid &uuid) {
    const auto &map = getMap<TAssetData>();
    return map.findHandleByUuid(uuid);
  }

  template <typename TAssetData> constexpr usize count() const {
    return getMap<TAssetData>().getMetas().size();
  }

  template <typename TAssetData> constexpr auto &getAll() const {
    return getMap<TAssetData>().getMetas();
  }

  template <typename TAssetData> constexpr void clear() {
    return getMap<TAssetData>().clear();
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

  template <typename TAssetData> constexpr auto &getMap() const {
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
