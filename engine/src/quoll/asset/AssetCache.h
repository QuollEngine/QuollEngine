#pragma once

#include "AssetHandle.h"
#include "AssetMeta.h"
#include "AssetRegistry.h"
#include "Result.h"

namespace quoll {

class InputBinaryStream;

/**
 * Asset cache
 *
 * Loads and creates engine optimized
 * assets for usage within engine
 * functionality
 */
class AssetCache {
public:
  AssetCache(const Path &assetsPath, bool createDefaultObjects = false);

  template <typename TAssetData>
  Result<AssetHandle<TAssetData>> getOrLoad(const Uuid &uuid) {
    if (uuid.isEmpty()) {
      return AssetHandle<TAssetData>();
    }

    auto handle = mRegistry.findHandleByUuid<TAssetData>(uuid);

    if (handle) {
      return handle;
    }

    return load<TAssetData>(uuid);
  }

  template <typename TAssetData>
  Result<AssetHandle<TAssetData>> load(const Uuid &uuid) {
    Result<TAssetData> data;

    if constexpr (std::is_same_v<TAssetData, TextureAsset>) {
      data = loadTexture(uuid);
    } else if constexpr (std::is_same_v<TAssetData, FontAsset>) {
      data = loadFont(uuid);
    } else if constexpr (std::is_same_v<TAssetData, MaterialAsset>) {
      data = loadMaterial(uuid);
    } else if constexpr (std::is_same_v<TAssetData, MeshAsset>) {
      data = loadMesh(uuid);
    } else if constexpr (std::is_same_v<TAssetData, SkeletonAsset>) {
      data = loadSkeleton(uuid);
    } else if constexpr (std::is_same_v<TAssetData, AnimationAsset>) {
      data = loadAnimation(uuid);
    } else if constexpr (std::is_same_v<TAssetData, AnimatorAsset>) {
      data = loadAnimator(uuid);
    } else if constexpr (std::is_same_v<TAssetData, AudioAsset>) {
      data = loadAudio(uuid);
    } else if constexpr (std::is_same_v<TAssetData, PrefabAsset>) {
      data = loadPrefab(uuid);
    } else if constexpr (std::is_same_v<TAssetData, LuaScriptAsset>) {
      data = loadLuaScript(uuid);
    } else if constexpr (std::is_same_v<TAssetData, EnvironmentAsset>) {
      data = loadEnvironment(uuid);
    } else if constexpr (std::is_same_v<TAssetData, SceneAsset>) {
      data = loadScene(uuid);
    } else if constexpr (std::is_same_v<TAssetData, InputMapAsset>) {
      data = loadInputMap(uuid);
    }

    if (!data) {
      return data.error();
    }

    auto meta = getAssetMeta(uuid);

    AssetData<TAssetData> asset;
    asset.uuid = uuid;
    asset.name = meta.name;
    asset.type = meta.type;
    asset.data = data.data();

    auto existing = mRegistry.findHandleByUuid<TAssetData>(uuid);
    if (existing) {
      mRegistry.update(existing, asset);
      return {existing, data.warnings()};
    }

    return {mRegistry.add(asset), data.warnings()};
  }

  Result<Path> createTextureFromSource(const Path &sourcePath,
                                       const Uuid &uuid);

  Result<Path> createTextureFromAsset(const AssetData<TextureAsset> &asset);

  Result<Path> createFontFromSource(const Path &sourcePath, const Uuid &uuid);

  Result<Path> createMaterialFromAsset(const AssetData<MaterialAsset> &asset);

  Result<Path> createMeshFromAsset(const AssetData<MeshAsset> &asset);

  Result<Path> createSkeletonFromAsset(const AssetData<SkeletonAsset> &asset);

  Result<Path> createAnimationFromAsset(const AssetData<AnimationAsset> &asset);

  Result<Path> createAnimatorFromSource(const Path &sourcePath,
                                        const Uuid &uuid);

  Result<Path> createAnimatorFromAsset(const AssetData<AnimatorAsset> &asset);

  Result<Path> createInputMapFromSource(const Path &sourcePath,
                                        const Uuid &uuid);

  Result<Path> createAudioFromSource(const Path &sourcePath, const Uuid &uuid);

  Result<Path> createPrefabFromAsset(const AssetData<PrefabAsset> &asset);

  Result<Path>
  createEnvironmentFromAsset(const AssetData<EnvironmentAsset> &asset);

  Result<Path> createLuaScriptFromSource(const Path &sourcePath,
                                         const Uuid &uuid);

  Result<Path> createSceneFromSource(const Path &sourcePath, const Uuid &uuid);

  inline AssetRegistry &getRegistry() { return mRegistry; }

  inline const Path &getAssetsPath() const { return mAssetsPath; }

  Result<void> preloadAssets(RenderStorage &renderStorage);

  AssetMeta getAssetMeta(const Uuid &uuid) const;

  Path getPathFromUuid(const Uuid &uuid) const;

private:
  Result<TextureAsset> loadTexture(const Uuid &uuid);
  Result<FontAsset> loadFont(const Uuid &uuid);
  Result<MaterialAsset> loadMaterial(const Uuid &uuid);
  Result<MeshAsset> loadMesh(const Uuid &uuid);
  Result<SkeletonAsset> loadSkeleton(const Uuid &uuid);
  Result<AnimationAsset> loadAnimation(const Uuid &uuid);
  Result<AnimatorAsset> loadAnimator(const Uuid &uuid);
  Result<InputMapAsset> loadInputMap(const Uuid &uuid);
  Result<AudioAsset> loadAudio(const Uuid &uuid);
  Result<PrefabAsset> loadPrefab(const Uuid &uuid);
  Result<EnvironmentAsset> loadEnvironment(const Uuid &uuid);
  Result<LuaScriptAsset> loadLuaScript(const Uuid &uuid);
  Result<SceneAsset> loadScene(const Uuid &uuid);

private:
  template <typename TAssetData>
  Uuid getAssetUuid(AssetHandle<TAssetData> handle) {
    if (handle) {
      return mRegistry.getMeta<TAssetData>(handle).uuid;
    }

    return Uuid{};
  }

  Result<Path> createAssetMeta(AssetType type, String name, Path path);

  Result<void> loadAsset(const Path &path);

private:
  Result<MaterialAsset> loadMaterialDataFromInputStream(const Path &path);

  Result<MeshAsset> loadMeshDataFromInputStream(const Path &path);

  Result<SkeletonAsset> loadSkeletonDataFromInputStream(const Path &path);

  Result<AnimationAsset> loadAnimationDataFromInputStream(const Path &path);

  Result<EnvironmentAsset> loadEnvironmentDataFromInputStream(const Path &path);

  Result<PrefabAsset> loadPrefabDataFromInputStream(const Path &path);

private:
  AssetRegistry mRegistry;
  Path mAssetsPath;
};

} // namespace quoll
