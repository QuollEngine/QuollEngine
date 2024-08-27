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

  Result<Path> createTextureFromSource(const Path &sourcePath,
                                       const Uuid &uuid);

  Result<Path> createTextureFromAsset(const AssetData<TextureAsset> &asset);

  Result<AssetHandle<TextureAsset>> loadTexture(const Uuid &uuid);

  Result<Path> createFontFromSource(const Path &sourcePath, const Uuid &uuid);

  Result<AssetHandle<FontAsset>> loadFont(const Uuid &uuid);

  Result<Path> createMaterialFromAsset(const AssetData<MaterialAsset> &asset);

  Result<AssetHandle<MaterialAsset>> loadMaterial(const Uuid &uuid);

  Result<Path> createMeshFromAsset(const AssetData<MeshAsset> &asset);

  Result<AssetHandle<MeshAsset>> loadMesh(const Uuid &uuid);

  Result<Path> createSkeletonFromAsset(const AssetData<SkeletonAsset> &asset);

  Result<AssetHandle<SkeletonAsset>> loadSkeleton(const Uuid &uuid);

  Result<Path> createAnimationFromAsset(const AssetData<AnimationAsset> &asset);

  Result<AssetHandle<AnimationAsset>> loadAnimation(const Uuid &uuid);

  Result<Path> createAnimatorFromSource(const Path &sourcePath,
                                        const Uuid &uuid);

  Result<Path> createAnimatorFromAsset(const AssetData<AnimatorAsset> &asset);

  Result<AssetHandle<InputMapAsset>> loadInputMap(const Uuid &uuid);

  Result<Path> createInputMapFromSource(const Path &sourcePath,
                                        const Uuid &uuid);

  Result<AssetHandle<AnimatorAsset>> loadAnimator(const Uuid &uuid);

  Result<Path> createAudioFromSource(const Path &sourcePath, const Uuid &uuid);

  Result<AssetHandle<AudioAsset>> loadAudio(const Uuid &uuid);

  Result<Path> createPrefabFromAsset(const AssetData<PrefabAsset> &asset);

  Result<AssetHandle<PrefabAsset>> loadPrefab(const Uuid &uuid);

  Result<Path>
  createEnvironmentFromAsset(const AssetData<EnvironmentAsset> &asset);

  Result<AssetHandle<EnvironmentAsset>> loadEnvironment(const Uuid &uuid);

  Result<Path> createLuaScriptFromSource(const Path &sourcePath,
                                         const Uuid &uuid);

  Result<AssetHandle<LuaScriptAsset>> loadLuaScript(const Uuid &uuid);

  Result<Path> createSceneFromSource(const Path &sourcePath, const Uuid &uuid);

  Result<AssetHandle<SceneAsset>> loadScene(const Uuid &uuid);

  inline AssetRegistry &getRegistry() { return mRegistry; }

  inline const Path &getAssetsPath() const { return mAssetsPath; }

  Result<bool> preloadAssets(RenderStorage &renderStorage);

  AssetMeta getAssetMeta(const Uuid &uuid) const;

  Path getPathFromUuid(const Uuid &uuid) const;

private:
  template <typename TAssetData>
  Uuid getAssetUuid(AssetHandle<TAssetData> handle) {
    if (handle) {
      return mRegistry.getMeta<TAssetData>(handle).uuid;
    }

    return Uuid{};
  }

  Result<Path> createAssetMeta(AssetType type, String name, Path path);

  Result<bool> loadAsset(const Path &path);

private:
  Result<AssetHandle<MaterialAsset>>
  loadMaterialDataFromInputStream(const Path &path, const Uuid &uuid,
                                  const AssetMeta &meta);

  Result<AssetHandle<MeshAsset>>
  loadMeshDataFromInputStream(const Path &path, const Uuid &uuid,
                              const AssetMeta &meta);

  Result<AssetHandle<SkeletonAsset>>
  loadSkeletonDataFromInputStream(const Path &path, const Uuid &uuid,
                                  const AssetMeta &meta);
  Result<AssetHandle<AnimationAsset>>
  loadAnimationDataFromInputStream(const Path &path, const Uuid &uuid,
                                   const AssetMeta &meta);

  Result<AssetHandle<EnvironmentAsset>>
  loadEnvironmentDataFromInputStream(const Path &path, const Uuid &uuid,
                                     const AssetMeta &meta);

  Result<AssetHandle<PrefabAsset>>
  loadPrefabDataFromInputStream(const Path &path, const Uuid &uuid,
                                const AssetMeta &meta);

private:
  Result<AssetHandle<TextureAsset>> getOrLoadTexture(const Uuid &uuid);

  Result<AssetHandle<MaterialAsset>> getOrLoadMaterial(const Uuid &uuid);

  Result<AssetHandle<MeshAsset>> getOrLoadMesh(const Uuid &uuid);

  Result<AssetHandle<SkeletonAsset>> getOrLoadSkeleton(const Uuid &uuid);

  Result<AssetHandle<AnimationAsset>> getOrLoadAnimation(const Uuid &uuid);

  Result<AssetHandle<AnimatorAsset>> getOrLoadAnimator(const Uuid &uuid);

private:
  AssetRegistry mRegistry;
  Path mAssetsPath;
};

} // namespace quoll
