#pragma once

#include "AssetFileHeader.h"
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

  Result<TextureAssetHandle> loadTexture(const Uuid &uuid);

  Result<Path> createFontFromSource(const Path &sourcePath, const Uuid &uuid);

  Result<FontAssetHandle> loadFont(const Uuid &uuid);

  Result<Path> createMaterialFromAsset(const AssetData<MaterialAsset> &asset);

  Result<MaterialAssetHandle> loadMaterial(const Uuid &uuid);

  Result<Path> createMeshFromAsset(const AssetData<MeshAsset> &asset);

  Result<MeshAssetHandle> loadMesh(const Uuid &uuid);

  Result<Path> createSkeletonFromAsset(const AssetData<SkeletonAsset> &asset);

  Result<SkeletonAssetHandle> loadSkeleton(const Uuid &uuid);

  Result<Path> createAnimationFromAsset(const AssetData<AnimationAsset> &asset);

  Result<AnimationAssetHandle> loadAnimation(const Uuid &uuid);

  Result<Path> createAnimatorFromSource(const Path &sourcePath,
                                        const Uuid &uuid);

  Result<Path> createAnimatorFromAsset(const AssetData<AnimatorAsset> &asset);

  Result<InputMapAssetHandle> loadInputMap(const Uuid &uuid);

  Result<Path> createInputMapFromSource(const Path &sourcePath,
                                        const Uuid &uuid);

  Result<AnimatorAssetHandle> loadAnimator(const Uuid &uuid);

  Result<Path> createAudioFromSource(const Path &sourcePath, const Uuid &uuid);

  Result<AudioAssetHandle> loadAudio(const Uuid &uuid);

  Result<Path> createPrefabFromAsset(const AssetData<PrefabAsset> &asset);

  Result<PrefabAssetHandle> loadPrefab(const Uuid &uuid);

  Result<Path>
  createEnvironmentFromAsset(const AssetData<EnvironmentAsset> &asset);

  Result<EnvironmentAssetHandle> loadEnvironment(const Uuid &uuid);

  Result<Path> createLuaScriptFromSource(const Path &sourcePath,
                                         const Uuid &uuid);

  Result<LuaScriptAssetHandle> loadLuaScript(const Uuid &uuid);

  Result<Path> createSceneFromSource(const Path &sourcePath, const Uuid &uuid);

  Result<SceneAssetHandle> loadScene(const Uuid &uuid);

  inline AssetRegistry &getRegistry() { return mRegistry; }

  inline const Path &getAssetsPath() const { return mAssetsPath; }

  Result<bool> preloadAssets(RenderStorage &renderStorage);

  AssetMeta getAssetMeta(const Uuid &uuid) const;

  Path getPathFromUuid(const Uuid &uuid);

private:
  template <class TAssetMap>
  Uuid getAssetUuid(TAssetMap &map, typename TAssetMap::Handle handle) {
    if (handle != TAssetMap::Handle::Null) {
      return map.getAsset(handle).uuid;
    }

    return Uuid{};
  }

  Result<AssetFileHeader> checkAssetFile(InputBinaryStream &file,
                                         const Path &filePath,
                                         AssetType assetType);

  Result<Path> createAssetMeta(AssetType type, String name, Path path);

  Result<bool> loadAsset(const Path &path);

private:
  Result<MaterialAssetHandle>
  loadMaterialDataFromInputStream(InputBinaryStream &stream,
                                  const Path &filePath,
                                  const AssetFileHeader &header);

  Result<MeshAssetHandle>
  loadMeshDataFromInputStream(InputBinaryStream &stream, const Path &filePath,
                              const AssetFileHeader &header);

  Result<SkeletonAssetHandle>
  loadSkeletonDataFromInputStream(InputBinaryStream &stream,
                                  const Path &filePath,
                                  const AssetFileHeader &header);
  Result<AnimationAssetHandle>
  loadAnimationDataFromInputStream(InputBinaryStream &stream,
                                   const Path &filePath,
                                   const AssetFileHeader &header);

  Result<EnvironmentAssetHandle>
  loadEnvironmentDataFromInputStream(InputBinaryStream &stream,
                                     const Path &filePath,
                                     const AssetFileHeader &header);

  Result<PrefabAssetHandle>
  loadPrefabDataFromInputStream(InputBinaryStream &stream, const Path &filePath,
                                const AssetFileHeader &header);

private:
  Result<TextureAssetHandle> getOrLoadTexture(const Uuid &uuid);

  Result<MaterialAssetHandle> getOrLoadMaterial(const Uuid &uuid);

  Result<MeshAssetHandle> getOrLoadMesh(const Uuid &uuid);

  Result<SkeletonAssetHandle> getOrLoadSkeleton(const Uuid &uuid);

  Result<AnimationAssetHandle> getOrLoadAnimation(const Uuid &uuid);

  Result<AnimatorAssetHandle> getOrLoadAnimator(const Uuid &uuid);

private:
  AssetRegistry mRegistry;
  Path mAssetsPath;
};

} // namespace quoll
