#pragma once

#include "AssetHandle.h"
#include "AssetMeta.h"
#include "AssetRef.h"
#include "AssetRegistry.h"
#include "Result.h"

namespace quoll {

/**
 * Asset cache
 *
 * Loads and creates engine optimized assets
 * for usage within engine functionality
 */
class AssetCache {
public:
  AssetCache(const Path &assetsPath, bool createDefaultObjects = false);

  template <typename TAssetData>
  Result<AssetRef<TAssetData>> request(const Uuid &uuid) {
    if (uuid.isEmpty()) {
      return Error("Invalid uuid");
    }

    auto handle = mRegistry.findHandleByUuid<TAssetData>(uuid);
    if (handle) {
      return AssetRef(mRegistry.getMap<TAssetData>(), handle);
    }

    auto res = load<TAssetData>(uuid);
    if (!res) {
      return res.error();
    }

    return Result(AssetRef(mRegistry.getMap<TAssetData>(), res.data()),
                  res.warnings());
  }

  template <typename TAssetData>
  Result<AssetHandle<TAssetData>> load(const Uuid &uuid) {
    Result<TAssetData> data;

    auto meta = getAssetMeta(uuid);
    if (meta.type != getAssetType<TAssetData>()) {
      return Error("Asset type is not " +
                   getAssetTypeString(getAssetType<TAssetData>()));
    }

    auto path = getPathFromUuid(uuid);

    if constexpr (std::is_same_v<TAssetData, TextureAsset>) {
      data = loadTexture(path);
    } else if constexpr (std::is_same_v<TAssetData, FontAsset>) {
      data = loadFont(path);
    } else if constexpr (std::is_same_v<TAssetData, MaterialAsset>) {
      data = loadMaterial(path);
    } else if constexpr (std::is_same_v<TAssetData, MeshAsset>) {
      data = loadMesh(path);
    } else if constexpr (std::is_same_v<TAssetData, SkeletonAsset>) {
      data = loadSkeleton(path);
    } else if constexpr (std::is_same_v<TAssetData, AnimationAsset>) {
      data = loadAnimation(path);
    } else if constexpr (std::is_same_v<TAssetData, AnimatorAsset>) {
      data = loadAnimator(path);
    } else if constexpr (std::is_same_v<TAssetData, AudioAsset>) {
      data = loadAudio(path);
    } else if constexpr (std::is_same_v<TAssetData, PrefabAsset>) {
      data = loadPrefab(path);
    } else if constexpr (std::is_same_v<TAssetData, LuaScriptAsset>) {
      data = loadLuaScript(path);
    } else if constexpr (std::is_same_v<TAssetData, EnvironmentAsset>) {
      data = loadEnvironment(path);
    } else if constexpr (std::is_same_v<TAssetData, SceneAsset>) {
      data = loadScene(path);
    } else if constexpr (std::is_same_v<TAssetData, InputMapAsset>) {
      data = loadInputMap(path);
    }

    if (!data) {
      return data.error();
    }

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

    auto handle = mRegistry.findHandleByUuid<TAssetData>(uuid);
    if (handle) {
      auto loadRes = load<TAssetData>(uuid);
      if (!loadRes) {
        return loadRes.error();
      }
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

    auto handle = mRegistry.findHandleByUuid<TAssetData>(info.uuid);
    if (handle) {
      auto loadRes = load<TAssetData>(info.uuid);
      if (!loadRes) {
        return res.error();
      }
    }

    return path;
  }

  inline AssetRegistry &getRegistry() { return mRegistry; }

  inline const Path &getAssetsPath() const { return mAssetsPath; }

  Result<void> preloadAssets();

  AssetMeta getAssetMeta(const Uuid &uuid) const;

  Path getPathFromUuid(const Uuid &uuid) const;

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

private:
  Result<TextureAsset> loadTexture(const Path &path);
  Result<void> createTextureFromData(const TextureAsset &data,
                                     const Path &assetPath);

  Result<FontAsset> loadFont(const Path &Path);

  Result<MaterialAsset> loadMaterial(const Path &Path);
  Result<void> createMaterialFromData(const MaterialAsset &data,
                                      const Path &assetPath);

  Result<MeshAsset> loadMesh(const Path &Path);
  Result<void> createMeshFromData(const MeshAsset &data, const Path &assetPath);

  Result<SkeletonAsset> loadSkeleton(const Path &Path);
  Result<void> createSkeletonFromData(const SkeletonAsset &data,
                                      const Path &assetPath);

  Result<AnimationAsset> loadAnimation(const Path &Path);
  Result<void> createAnimationFromData(const AnimationAsset &data,
                                       const Path &assetPath);

  Result<AnimatorAsset> loadAnimator(const Path &Path);
  Result<void> createAnimatorFromData(const AnimatorAsset &data,
                                      const Path &assetPath);

  Result<InputMapAsset> loadInputMap(const Path &Path);

  Result<AudioAsset> loadAudio(const Path &Path);

  Result<PrefabAsset> loadPrefab(const Path &Path);
  Result<void> createPrefabFromData(const PrefabAsset &data,
                                    const Path &assetPath);

  Result<EnvironmentAsset> loadEnvironment(const Path &Path);
  Result<void> createEnvironmentFromData(const EnvironmentAsset &data,
                                         const Path &assetPath);

  Result<LuaScriptAsset> loadLuaScript(const Path &Path);

  Result<SceneAsset> loadScene(const Path &path);

private:
  template <typename TAssetData>
  Uuid getAssetUuid(AssetHandle<TAssetData> handle) {
    if (handle) {
      return mRegistry.getMeta<TAssetData>(handle).uuid;
    }

    return Uuid{};
  }

  template <typename TAssetData>
  Uuid getAssetUuid(const AssetRef<TAssetData> &asset) {
    if (asset) {
      return asset.meta().uuid;
    }

    return Uuid();
  }

  Result<Path> createAssetMeta(AssetType type, String name, Path path);

  Result<void> loadAsset(const Path &path);

private:
  AssetRegistry mRegistry;
  Path mAssetsPath;
};

} // namespace quoll
