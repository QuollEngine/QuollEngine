#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"

#include "AssetManager.h"
#include "GLTFImporter.h"

#include <stb/stb_image.h>
#include <cryptopp/sha.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>

namespace liquidator {

const std::vector<liquid::String> AssetManager::TextureExtensions{
    "png", "jpg", "jpeg", "bmp", "tga"};
const std::vector<liquid::String> AssetManager::ScriptExtensions{"lua"};
const std::vector<liquid::String> AssetManager::AudioExtensions{"wav", "mp3"};
const std::vector<liquid::String> AssetManager::FontExtensions{"ttf", "otf"};
const std::vector<liquid::String> AssetManager::SceneExtensions{"gltf", "glb"};

using co = std::filesystem::copy_options;

/**
 * @brief Get unique path for a given path
 *
 * Appends incrementing number at the end of
 * file to until the file no longer exists in
 * directory
 *
 * @param path File path
 * @return Unique file path
 */
static liquid::Path getUniquePath(liquid::Path path) {
  auto ext = path.extension();

  auto tmpPath = path;

  uint32_t index = 1;
  while (std::filesystem::exists(tmpPath)) {
    liquid::String uniqueSuffix = "-" + std::to_string(index++);
    tmpPath = path;
    tmpPath.replace_extension();
    tmpPath += uniqueSuffix;
    tmpPath.replace_extension(ext);
  }

  return tmpPath;
}

AssetManager::AssetManager(const liquid::Path &assetsPath,
                           const liquid::Path &assetsCachePath,
                           bool createDefaultObjects)
    : mAssetsPath(assetsPath),
      mAssetCache(assetsCachePath, createDefaultObjects) {}

liquid::Result<bool>
AssetManager::importAsset(const liquid::Path &source,
                          const liquid::Path &targetAssetDirectory) {
  auto createdDirectory = createDirectoriesRecursive(targetAssetDirectory);

  auto targetAssetPath =
      getUniquePath(targetAssetDirectory / source.filename());

  if (targetAssetPath.extension() == ".gltf") {
    GLTFImporter importer(mAssetCache);
    auto res = importer.saveBinary(source, targetAssetPath);
    if (res.hasError()) {
      return liquid::Result<bool>::Error(res.getError());
    }

    targetAssetPath = res.getData();
  } else {
    std::filesystem::copy_file(source, targetAssetPath, co::overwrite_existing);
  }

  auto res = loadOriginalAsset(targetAssetPath);

  if (res.hasError()) {
    std::filesystem::remove(targetAssetPath);
    if (createdDirectory.has_value()) {
      std::filesystem::remove_all(createdDirectory.value());
    }
  }

  if (res.hasError()) {
    return liquid::Result<bool>::Error("Cannot import " + source.string() +
                                       ": " + res.getError());
  }
  return res;
}

liquid::Path
AssetManager::findEngineAssetPath(const liquid::Path &originalAssetPath) {
  auto it = mAssetCacheMap.find(originalAssetPath.string());
  if (it != mAssetCacheMap.end()) {
    return it->second;
  }

  auto hashFilePath = getHashFilePath(originalAssetPath);
  std::ifstream stream(hashFilePath);

  if (!stream.good()) {
    return "/";
  }

  auto node = YAML::Load(stream);
  auto engineAssetPathStr = node["engineAssetPath"].as<liquid::String>();
  stream.close();

  auto engineAssetPath =
      (mAssetCache.getAssetsPath() / engineAssetPathStr).make_preferred();

  mAssetCacheMap.insert_or_assign(
      std::filesystem::canonical(originalAssetPath).string(), engineAssetPath);

  return engineAssetPath;
}

liquid::Result<bool>
AssetManager::createDirectory(const liquid::Path &directory) {
  if (std::filesystem::create_directory(directory) &&
      std::filesystem::create_directory(
          convertToCacheRelativePath(directory))) {
    return liquid::Result<bool>::Ok(true);
  }

  return liquid::Result<bool>::Error("Could not create directory");
}

liquid::Result<liquid::Path>
AssetManager::createLuaScript(const liquid::Path &assetPath) {
  createDirectoriesRecursive(assetPath.parent_path());
  auto createdDirectory = createDirectoriesRecursive(
      convertToCacheRelativePath(assetPath.parent_path()));

  auto originalAssetPath = assetPath;
  originalAssetPath.replace_extension("lua");
  std::ofstream stream(originalAssetPath);
  stream.close();

  auto res = loadOriginalAsset(originalAssetPath);

  if (res.hasError()) {
    return liquid::Result<liquid::Path>::Error(res.getError());
  }

  return liquid::Result<liquid::Path>::Ok(originalAssetPath, res.getWarnings());
}

liquid::Result<bool>
AssetManager::validateAndPreloadAssets(liquid::rhi::RenderDevice *device) {
  LIQUID_PROFILE_EVENT("AssetManager::preloadAssets");
  std::vector<liquid::String> warnings;

  for (const auto &entry : std::filesystem::recursive_directory_iterator(
           mAssetCache.getAssetsPath())) {
    if (!entry.is_regular_file() || entry.path().extension() != ".lqhash") {
      continue;
    }

    auto hashFilePath = entry.path();

    auto assetPath =
        mAssetsPath /
        std::filesystem::relative(hashFilePath, mAssetCache.getAssetsPath());
    assetPath.replace_extension();

    if (!std::filesystem::exists(assetPath)) {
      std::ifstream stream(hashFilePath);
      auto node = YAML::Load(stream);
      auto engineAssetPathStr = node["engineAssetPath"].as<liquid::String>();
      stream.close();

      auto engineAssetPath =
          (mAssetCache.getAssetsPath() / engineAssetPathStr).make_preferred();

      if (engineAssetPath.extension() == ".lqprefab") {
        std::filesystem::remove_all(engineAssetPath.parent_path());
      } else {
        std::filesystem::remove(engineAssetPath);
      }
      std::filesystem::remove(hashFilePath);
    }
  }

  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(mAssetsPath)) {
    if (!entry.is_regular_file()) {
      continue;
    }

    if (isAssetChanged(entry.path())) {
      auto res = loadOriginalAsset(entry.path());
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    }
  }

  return mAssetCache.preloadAssets(device);
}

liquid::Result<bool>
AssetManager::loadOriginalAsset(const liquid::Path &originalAssetPath) {
  auto createdDirectory = createDirectoriesRecursive(
      convertToCacheRelativePath(originalAssetPath.parent_path()));

  auto res = liquid::Result<liquid::Path>::Error("Unsupported asset format");

  auto type = getAssetTypeFromExtension(originalAssetPath);

  if (type == liquid::AssetType::Texture) {
    res = loadOriginalTexture(originalAssetPath);
  } else if (type == liquid::AssetType::LuaScript) {
    res = loadOriginalScript(originalAssetPath);
  } else if (type == liquid::AssetType::Audio) {
    res = loadOriginalAudio(originalAssetPath);
  } else if (type == liquid::AssetType::Font) {
    res = loadOriginalFont(originalAssetPath);
  } else if (type == liquid::AssetType::Prefab) {
    res = loadOriginalPrefab(originalAssetPath);
  }

  if (res.hasData()) {
    createHashFile(originalAssetPath, res.getData());
    return liquid::Result<bool>::Ok(true);
  }

  if (createdDirectory.has_value()) {
    std::filesystem::remove_all(createdDirectory.value());
  }

  return liquid::Result<bool>::Error(res.getError());
}

liquid::Result<bool>
AssetManager::loadOriginalIfChanged(const liquid::Path &originalAssetPath) {
  if (isAssetChanged(originalAssetPath)) {
    return loadOriginalAsset(originalAssetPath);
  }

  return liquid::Result<bool>::Ok(true);
}

liquid::Result<liquid::Path>
AssetManager::loadOriginalTexture(const liquid::Path &originalAssetPath) {
  int32_t width = 0;
  int32_t height = 0;
  int32_t channels = 0;

  auto *data = stbi_load(originalAssetPath.string().c_str(), &width, &height,
                         &channels, STBI_rgb_alpha);

  if (!data) {
    return liquid::Result<liquid::Path>::Error(stbi_failure_reason());
  }

  liquid::AssetData<liquid::TextureAsset> asset{};
  asset.name = getOriginalAssetName(originalAssetPath);
  asset.size = width * height * 4;
  asset.data.data = data;
  asset.data.height = height;
  asset.data.width = width;
  asset.data.layers = 1;
  asset.data.format = liquid::rhi::Format::Rgba8Srgb;

  auto createdFileRes = mAssetCache.createTextureFromAsset(asset);
  stbi_image_free(data);

  if (createdFileRes.hasError()) {
    return createdFileRes;
  }

  auto loadRes = mAssetCache.loadAsset(createdFileRes.getData());
  if (loadRes.hasError()) {
    return liquid::Result<liquid::Path>::Error(loadRes.getError());
  }

  return createdFileRes;
}

liquid::Result<liquid::Path>
AssetManager::loadOriginalAudio(const liquid::Path &originalAssetPath) {
  auto engineAssetPath = convertToCacheRelativePath(originalAssetPath);

  if (std::filesystem::copy_file(originalAssetPath, engineAssetPath,
                                 co::overwrite_existing)) {
    auto res = mAssetCache.loadAsset(engineAssetPath);
    if (res.hasData()) {
      return liquid::Result<liquid::Path>::Ok(engineAssetPath);
    }

    std::filesystem::remove(engineAssetPath);
  }

  return liquid::Result<liquid::Path>::Error("Cannot load audio file");
}

liquid::Result<liquid::Path>
AssetManager::loadOriginalScript(const liquid::Path &originalAssetPath) {
  auto engineAssetPath = convertToCacheRelativePath(originalAssetPath);

  if (std::filesystem::copy_file(originalAssetPath, engineAssetPath,
                                 co::overwrite_existing)) {
    auto res = mAssetCache.loadAsset(engineAssetPath);
    if (res.hasData()) {
      return liquid::Result<liquid::Path>::Ok(engineAssetPath);
    }
    std::filesystem::remove(engineAssetPath);
  }

  return liquid::Result<liquid::Path>::Error("Cannot load script file");
}

liquid::Result<liquid::Path>
AssetManager::loadOriginalFont(const liquid::Path &originalAssetPath) {
  auto engineAssetPath = convertToCacheRelativePath(originalAssetPath);

  if (std::filesystem::copy_file(originalAssetPath, engineAssetPath,
                                 co::overwrite_existing)) {
    auto res = mAssetCache.loadAsset(engineAssetPath);
    if (res.hasData()) {
      return liquid::Result<liquid::Path>::Ok(engineAssetPath);
    }
    std::filesystem::remove(engineAssetPath);
  }

  return liquid::Result<liquid::Path>::Error("Cannot load font file");
}

liquid::Result<liquid::Path>
AssetManager::loadOriginalPrefab(const liquid::Path &originalAssetPath) {
  auto engineAssetPath = convertToCacheRelativePath(originalAssetPath);

  GLTFImporter importer(mAssetCache);
  return importer.loadFromPath(originalAssetPath, engineAssetPath);
}

liquid::String AssetManager::getFileHash(const liquid::Path &path) {
  std::ifstream stream(path, std::ios::binary);

  if (!stream.good()) {
    return "";
  }

  liquid::String string;
  CryptoPP::SHA256 sha256;
  CryptoPP::FileSource source(
      stream, true,
      new CryptoPP::HashFilter(
          sha256, new CryptoPP::HexEncoder(new CryptoPP::StringSink(string))));

  stream.close();

  return string;
}

liquid::Result<liquid::Path>
AssetManager::createHashFile(const liquid::Path &originalAssetPath,
                             const liquid::Path &engineAssetPath) {
  auto filename = originalAssetPath.filename();
  auto hashFilePath = getHashFilePath(originalAssetPath);

  auto engineAssetPathStr =
      std::filesystem::relative(engineAssetPath, mAssetCache.getAssetsPath())
          .string();
  std::replace(engineAssetPathStr.begin(), engineAssetPathStr.end(), '\\', '/');

  YAML::Node node;
  node["originalAssetHash"] = getFileHash(originalAssetPath);
  node["engineAssetHash"] = getFileHash(engineAssetPath);
  node["engineAssetPath"] = engineAssetPathStr;

  std::ofstream stream(hashFilePath);
  stream << node;
  stream.close();

  return liquid::Result<liquid::Path>::Ok(hashFilePath);
}

liquid::Path
AssetManager::convertToCacheRelativePath(const liquid::Path &path) const {
  return mAssetCache.getAssetsPath() /
         std::filesystem::relative(path, mAssetsPath);
}

liquid::Path
AssetManager::getHashFilePath(const liquid::Path &originalAssetPath) const {
  auto temp = originalAssetPath;
  auto newExtension = originalAssetPath.extension().string() + ".lqhash";
  return convertToCacheRelativePath(temp.replace_extension(newExtension));
}

liquid::String AssetManager::getOriginalAssetName(const liquid::Path &path) {
  return std::filesystem::relative(path, mAssetsPath).string();
}

bool AssetManager::isAssetChanged(const liquid::Path &assetFilePath) const {
  auto hashFilePath = getHashFilePath(assetFilePath);

  // Checksum is invalid if hash or engine files
  // do not exist
  if (!std::filesystem::exists(hashFilePath)) {
    return true;
  }

  std::ifstream stream(hashFilePath);
  auto node = YAML::Load(stream);
  auto originalAssetHash = node["originalAssetHash"].as<liquid::String>();
  auto engineAssetPathStr = node["engineAssetPath"].as<liquid::String>();
  stream.close();

  auto engineAssetPath =
      (mAssetCache.getAssetsPath() / engineAssetPathStr).make_preferred();

  if (!std::filesystem::exists(engineAssetPath)) {
    return true;
  }

  return getFileHash(assetFilePath) != originalAssetHash;
}

liquid::AssetType
AssetManager::getAssetTypeFromExtension(const liquid::Path &path) {
  auto ext = path.extension().string();
  ext.erase(0, 1);

  auto isExtension = [&ext](const auto &extensions) {
    return std::find(extensions.begin(), extensions.end(), ext) !=
           extensions.end();
  };

  if (isExtension(TextureExtensions)) {
    return liquid::AssetType::Texture;
  }
  if (isExtension(ScriptExtensions)) {
    return liquid::AssetType::LuaScript;
  }
  if (isExtension(AudioExtensions)) {
    return liquid::AssetType::Audio;
  }
  if (isExtension(FontExtensions)) {
    return liquid::AssetType::Font;
  }
  if (isExtension(SceneExtensions)) {
    return liquid::AssetType::Prefab;
  }

  return liquid::AssetType::None;
}

std::optional<liquid::Path>
AssetManager::createDirectoriesRecursive(const liquid::Path &path) {
  auto p = path;
  std::optional<liquid::Path> lastExisting;
  while (!std::filesystem::exists(p)) {
    lastExisting = p;
    p = p.parent_path();
  }

  std::filesystem::create_directories(path);

  return lastExisting;
}

} // namespace liquidator
