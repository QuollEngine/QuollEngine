#include "quoll/core/Base.h"
#include "AssetCache.h"

namespace quoll {

Result<SceneAsset> AssetCache::loadScene(const Path &path) {
  std::ifstream stream(path);
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
