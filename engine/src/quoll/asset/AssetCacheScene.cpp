#include "quoll/core/Base.h"
#include "AssetCache.h"

namespace quoll {

Result<Path> AssetCache::createSceneFromSource(const Path &sourcePath,
                                               const Uuid &uuid) {
  using co = std::filesystem::copy_options;

  auto assetPath = getPathFromUuid(uuid);

  std::error_code code;
  if (!std::filesystem::copy_file(sourcePath, assetPath, co::overwrite_existing,
                                  code)) {
    return Result<Path>::Error(
        "Cannot create scene from source: " + sourcePath.stem().string() +
        "; " + code.message());
  }

  auto metaRes = createAssetMeta(AssetType::Scene,
                                 sourcePath.filename().string(), assetPath);

  if (!metaRes.hasData()) {
    std::filesystem::remove(assetPath);
    return Result<Path>::Error("Cannot create scene from source: " +
                               sourcePath.stem().string());
  }

  return Result<Path>::Ok(assetPath);
}

Result<AssetHandle<SceneAsset>> AssetCache::loadScene(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  std::ifstream stream(filePath);
  auto root = YAML::Load(stream);
  stream.close();

  if (root["type"].as<String>("") != "scene") {
    return Result<AssetHandle<SceneAsset>>::Error("Type must be scene");
  }

  if (root["version"].as<String>("") != "0.1") {
    return Result<AssetHandle<SceneAsset>>::Error("Version is not supported");
  }

  if (root["name"].as<String>("").length() == 0) {
    return Result<AssetHandle<SceneAsset>>::Error("`name` cannot be empty");
  }

  if (root["zones"].Type() != YAML::NodeType::Sequence) {
    return Result<AssetHandle<SceneAsset>>::Error("`zones` field is invalid");
  }

  if (root["entities"].Type() != YAML::NodeType::Sequence) {
    return Result<AssetHandle<SceneAsset>>::Error(
        "`entities` field is invalid");
  }

  auto meta = getAssetMeta(uuid);

  AssetData<SceneAsset> asset{};
  asset.type = AssetType::Scene;
  asset.name = meta.name;
  asset.path = filePath;
  asset.uuid = Uuid(filePath.stem().string());
  asset.data.data = root;

  auto handle = mRegistry.add(asset);
  return Result<AssetHandle<SceneAsset>>::Ok(handle);
}

} // namespace quoll
