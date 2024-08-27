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
    return Error("Cannot create scene from source: " +
                 sourcePath.stem().string() + "; " + code.message());
  }

  auto metaRes = createAssetMeta(AssetType::Scene,
                                 sourcePath.filename().string(), assetPath);

  if (!metaRes) {
    std::filesystem::remove(assetPath);
    return Error("Cannot create scene from source: " +
                 sourcePath.stem().string());
  }

  return assetPath;
}

Result<SceneAsset> AssetCache::loadScene(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  std::ifstream stream(filePath);
  auto root = YAML::Load(stream);
  stream.close();

  if (root["type"].as<String>("") != "scene") {
    return Error("Type must be scene");
  }

  if (root["version"].as<String>("") != "0.1") {
    return Error("Version is not supported");
  }

  if (root["name"].as<String>("").length() == 0) {
    return Error("`name` cannot be empty");
  }

  if (root["zones"].Type() != YAML::NodeType::Sequence) {
    return Error("`zones` field is invalid");
  }

  if (root["entities"].Type() != YAML::NodeType::Sequence) {
    return Error("`entities` field is invalid");
  }

  return SceneAsset{.data = root};
}

} // namespace quoll
