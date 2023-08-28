#pragma once

#include "Asset.h"
#include "AssetMap.h"

#include "MaterialAsset.h"
#include "TextureAsset.h"
#include "FontAsset.h"
#include "MeshAsset.h"
#include "SkeletonAsset.h"
#include "AnimationAsset.h"
#include "AnimatorAsset.h"
#include "AudioAsset.h"
#include "PrefabAsset.h"
#include "EnvironmentAsset.h"
#include "LuaScriptAsset.h"
#include "SceneAsset.h"

#include "liquid/rhi/RenderDevice.h"

#include "liquid/renderer/RenderStorage.h"

namespace liquid {

/**
 * @brief Asset registry
 *
 * Stores all the loaded assets
 * in the engine
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

  struct DefaultObjects {
    MeshAssetHandle cube = MeshAssetHandle::Null;
    MaterialAssetHandle defaultMaterial = MaterialAssetHandle::Null;
    FontAssetHandle defaultFont = FontAssetHandle::Null;
  };

public:
  /**
   * @brief Create asset registry
   */
  AssetRegistry() = default;

  /**
   * @brief Destroy asset registry
   */
  ~AssetRegistry() = default;

  AssetRegistry(const AssetRegistry &) = delete;
  AssetRegistry &operator=(const AssetRegistry &) = delete;
  AssetRegistry(AssetRegistry &&) = delete;
  AssetRegistry &operator=(AssetRegistry &&) = delete;

  /**
   * @brief Create default objects
   *
   * Handles for default objects are
   * stored for easy access
   */
  void createDefaultObjects();

  /**
   * @brief Get default objects
   *
   * @return Default objects
   */
  inline const DefaultObjects &getDefaultObjects() const {
    return mDefaultObjects;
  }

  /**
   * @brief Synchronize assets with device
   *
   * @param renderStorage Render storage
   */
  void syncWithDevice(RenderStorage &renderStorage);

  /**
   * @brief Get textures
   *
   * @return Texture asset map
   */
  inline TextureMap &getTextures() { return mTextures; }

  /**
   * @brief Get fonts
   *
   * @return Font asset map
   */
  inline FontMap &getFonts() { return mFonts; }

  /**
   * @brief Get materials
   *
   * @return Material asset map
   */
  inline MaterialMap &getMaterials() { return mMaterials; }

  /**
   * @brief Get meshes
   *
   * @return Mesh asset map
   */
  inline MeshMap &getMeshes() { return mMeshes; }

  /**
   * @brief Get skeletons
   *
   * @return Skeleton asset map
   */
  inline SkeletonMap &getSkeletons() { return mSkeletons; }

  /**
   * @brief Get animations
   *
   * @return Animation asset map
   */
  inline AnimationMap &getAnimations() { return mAnimations; }

  /**
   * @brief Get animators
   *
   * @return Animator asset map
   */
  inline AnimatorMap &getAnimators() { return mAnimators; }

  /**
   * @brief Get audios
   *
   * @return Audio asset map
   */
  inline AudioMap &getAudios() { return mAudios; }

  /**
   * @brief Get prefabs
   *
   * @return Prefab asset map
   */
  inline PrefabMap &getPrefabs() { return mPrefabs; }

  /**
   * @brief Get Lua scripts
   *
   * @return Lua script asset map
   */
  inline LuaScriptMap &getLuaScripts() { return mLuaScripts; }

  /**
   * @brief Get environments
   *
   * @return Environment asset map
   */
  inline EnvironmentMap &getEnvironments() { return mEnvironments; }

  /**
   * @brief Get scenes
   *
   * @return Scenes asset map
   */
  inline SceneMap &getScenes() { return mScenes; }

  /**
   * @brief Get asset by uuid
   *
   * @param uuid Asset uuid
   * @retval {AssetType::None, 0} Asset does not exist
   * @return Asset type and ID
   */
  std::pair<AssetType, uint32_t> getAssetByUuid(const Uuid &uuid);

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

  DefaultObjects mDefaultObjects;
};

} // namespace liquid
