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

  template <typename TAssetData>
  Result<Path> createFromSource(const Path &sourcePath, const Uuid &uuid) {
    if (uuid.isEmpty()) {
      QuollAssert(false, "Invalid uuid provided");
      return Error("Invalid uuid provided");
    }

    auto path = getPathFromUuid(uuid);

    auto assetType = getAssetType<TAssetData>();
    auto metaPath =
        createAssetMeta(assetType, sourcePath.filename().string(), path);
    if (!metaPath) {
      return metaPath.error();
    }

    using co = std::filesystem::copy_options;

    if (!std::filesystem::copy_file(sourcePath, path, co::overwrite_existing)) {
      std::filesystem::remove(metaPath);
      return Error("Cannot create " + getAssetTypeString(assetType) +
                   " from source: " + sourcePath.stem().string());
    }

    return path;
  }

  template <typename TAssetData>
  Result<Path> createFromData(const AssetData<TAssetData> &info) {
    auto path = getPathFromUuid(info.uuid);

    auto metaPath =
        createAssetMeta(getAssetType<TAssetData>(), info.name, path);
    if (!metaPath) {
      return metaPath.error();
    }

    Result<void> res;

    if constexpr (std::is_same_v<TAssetData, TextureAsset>) {
      res = createTextureFromData(info.data, path);
    } else if constexpr (std::is_same_v<TAssetData, MaterialAsset>) {
      res = createMaterialFromData(info.data, path);
    } else if constexpr (std::is_same_v<TAssetData, MeshAsset>) {
      res = createMeshFromData(info.data, path);
    } else if constexpr (std::is_same_v<TAssetData, SkeletonAsset>) {
      res = createSkeletonFromData(info.data, path);
    } else if constexpr (std::is_same_v<TAssetData, AnimationAsset>) {
      res = createAnimationFromData(info.data, path);
    } else if constexpr (std::is_same_v<TAssetData, AnimatorAsset>) {
      res = createAnimatorFromData(info.data, path);
    } else if constexpr (std::is_same_v<TAssetData, PrefabAsset>) {
      res = createPrefabFromData(info.data, path);
    } else if constexpr (std::is_same_v<TAssetData, EnvironmentAsset>) {
      res = createEnvironmentFromData(info.data, path);
    }

    if (!res) {
      std::filesystem::remove(metaPath);
      return res.error();
    }

    return path;
  }

  inline AssetRegistry &getRegistry() { return mRegistry; }

  inline const Path &getAssetsPath() const { return mAssetsPath; }

  Result<void> preloadAssets(RenderStorage &renderStorage);

  AssetMeta getAssetMeta(const Uuid &uuid) const;

  Path getPathFromUuid(const Uuid &uuid) const;

private:
  template <typename TAssetData> static constexpr AssetType getAssetType() {
    if constexpr (std::is_same_v<TAssetData, TextureAsset>) {
      return AssetType::Texture;
    } else if constexpr (std::is_same_v<TAssetData, FontAsset>) {
      return AssetType::Font;
    } else if constexpr (std::is_same_v<TAssetData, MaterialAsset>) {
      return AssetType::Material;
    } else if constexpr (std::is_same_v<TAssetData, MeshAsset>) {
      return AssetType::Mesh;
    } else if constexpr (std::is_same_v<TAssetData, SkeletonAsset>) {
      return AssetType::Skeleton;
    } else if constexpr (std::is_same_v<TAssetData, AnimationAsset>) {
      return AssetType::Animation;
    } else if constexpr (std::is_same_v<TAssetData, AnimatorAsset>) {
      return AssetType::Animator;
    } else if constexpr (std::is_same_v<TAssetData, AudioAsset>) {
      return AssetType::Audio;
    } else if constexpr (std::is_same_v<TAssetData, PrefabAsset>) {
      return AssetType::Prefab;
    } else if constexpr (std::is_same_v<TAssetData, LuaScriptAsset>) {
      return AssetType::LuaScript;
    } else if constexpr (std::is_same_v<TAssetData, EnvironmentAsset>) {
      return AssetType::Environment;
    } else if constexpr (std::is_same_v<TAssetData, SceneAsset>) {
      return AssetType::Scene;
    } else if constexpr (std::is_same_v<TAssetData, InputMapAsset>) {
      return AssetType::InputMap;
    }
  }

  Result<TextureAsset> loadTexture(const Uuid &uuid);
  Result<void> createTextureFromData(const TextureAsset &data,
                                     const Path &assetPath);

  Result<FontAsset> loadFont(const Uuid &uuid);

  Result<MaterialAsset> loadMaterial(const Uuid &uuid);
  Result<void> createMaterialFromData(const MaterialAsset &data,
                                      const Path &assetPath);

  Result<MeshAsset> loadMesh(const Uuid &uuid);
  Result<void> createMeshFromData(const MeshAsset &data, const Path &assetPath);

  Result<SkeletonAsset> loadSkeleton(const Uuid &uuid);
  Result<void> createSkeletonFromData(const SkeletonAsset &data,
                                      const Path &assetPath);

  Result<AnimationAsset> loadAnimation(const Uuid &uuid);
  Result<void> createAnimationFromData(const AnimationAsset &data,
                                       const Path &assetPath);

  Result<AnimatorAsset> loadAnimator(const Uuid &uuid);
  Result<void> createAnimatorFromData(const AnimatorAsset &data,
                                      const Path &assetPath);

  Result<InputMapAsset> loadInputMap(const Uuid &uuid);

  Result<AudioAsset> loadAudio(const Uuid &uuid);

  Result<PrefabAsset> loadPrefab(const Uuid &uuid);
  Result<void> createPrefabFromData(const PrefabAsset &data,
                                    const Path &assetPath);

  Result<EnvironmentAsset> loadEnvironment(const Uuid &uuid);
  Result<void> createEnvironmentFromData(const EnvironmentAsset &data,
                                         const Path &assetPath);

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
