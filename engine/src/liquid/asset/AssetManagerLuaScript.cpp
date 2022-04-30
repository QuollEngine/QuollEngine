#include "liquid/core/Base.h"

#include "AssetManager.h"

namespace liquid {

/**
 * @brief Store text file contents in a buffer
 *
 * @param stream Input stream
 * @return Vector of characters
 */
static std::vector<char> readFileIntoBuffer(std::ifstream &stream) {
  std::ostringstream ss;
  ss << stream.rdbuf();
  const std::string &s = ss.str();
  std::vector<char> bytes(s.begin(), s.end());

  return bytes;
}

Result<LuaScriptAssetHandle>
AssetManager::loadLuaScriptFromFile(const std::filesystem::path &filePath) {
  std::ifstream stream(filePath);

  if (!stream.good()) {
    return Result<LuaScriptAssetHandle>::Error(
        "File cannot be opened for reading: " + filePath.string());
  }

  AssetData<LuaScriptAsset> asset;
  asset.path = filePath;
  asset.relativePath = std::filesystem::relative(filePath, mAssetsPath);
  asset.name = asset.relativePath.string();
  asset.type = AssetType::LuaScript;
  asset.data.bytes = readFileIntoBuffer(stream);

  stream.close();

  auto handle = mRegistry.getLuaScripts().addAsset(asset);

  return Result<LuaScriptAssetHandle>::Ok(handle);
}

} // namespace liquid
