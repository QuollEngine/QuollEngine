#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"

#include "AssetManager.h"
#include "GLTFImporter.h"
#include "HDRIImporter.h"

#include <stb/stb_image.h>
#include <cryptopp/sha.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>

namespace liquid::editor {

const std::vector<String> AssetManager::TextureExtensions{"png", "jpg", "jpeg",
                                                          "bmp", "tga", "ktx2"};
const std::vector<String> AssetManager::ScriptExtensions{"lua"};
const std::vector<String> AssetManager::AudioExtensions{"wav", "mp3"};
const std::vector<String> AssetManager::FontExtensions{"ttf", "otf"};
const std::vector<String> AssetManager::SceneExtensions{"gltf", "glb"};
const std::vector<String> AssetManager::EnvironmentExtensions{"hdr"};

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
static Path getUniquePath(Path path) {
  auto ext = path.extension();

  auto tmpPath = path;

  uint32_t index = 1;
  while (std::filesystem::exists(tmpPath)) {
    String uniqueSuffix = "-" + std::to_string(index++);
    tmpPath = path;
    tmpPath.replace_extension();
    tmpPath += uniqueSuffix;
    tmpPath.replace_extension(ext);
  }

  return tmpPath;
}

AssetManager::AssetManager(const Path &assetsPath, const Path &assetsCachePath,
                           liquid::rhi::RenderDevice *device, bool optimize,
                           bool createDefaultObjects)
    : mAssetsPath(assetsPath),
      mAssetCache(assetsCachePath, createDefaultObjects),
      mImageLoader(mAssetCache, device), mHDRIImporter(mAssetCache, device),
      mOptimize(optimize) {}

Result<Path> AssetManager::importAsset(const Path &source,
                                       const Path &targetAssetDirectory) {
  auto createdDirectory = createDirectoriesRecursive(targetAssetDirectory);

  auto targetAssetPath =
      getUniquePath(targetAssetDirectory / source.filename());

  if (targetAssetPath.extension() == ".gltf") {
    GLTFImporter importer(mAssetCache, mImageLoader, mOptimize);
    auto res = importer.saveBinary(source, targetAssetPath);
    if (res.hasError()) {
      return Result<Path>::Error(res.getError());
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
    return Result<Path>::Error("Cannot import " + source.string() + ": " +
                               res.getError());
  }

  return Result<Path>::Ok(targetAssetPath);
}

void AssetManager::generatePreview(const Path &originalAssetPath,
                                   RenderStorage &renderStorage) {
  const auto &engineAssetPath = findEngineAssetPath(originalAssetPath);
  const auto &res = mAssetCache.getRegistry().getAssetByPath(engineAssetPath);
  auto handle = res.second;

  if (res.first == AssetType::Environment) {
    auto &asset = mAssetCache.getRegistry().getEnvironments().getAsset(
        static_cast<liquid::EnvironmentAssetHandle>(handle));
    if (!rhi::isHandleValid(asset.preview)) {
      asset.preview =
          mHDRIImporter.loadFromPathToDevice(originalAssetPath, renderStorage);
    }
  } else if (res.first == AssetType::Texture) {
    auto &asset = mAssetCache.getRegistry().getTextures().getAsset(
        static_cast<liquid::TextureAssetHandle>(handle));

    if (!rhi::isHandleValid(asset.preview)) {
      asset.preview = asset.data.deviceHandle;
    }
  }
}

Path AssetManager::findEngineAssetPath(const Path &originalAssetPath) {
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
  auto engineAssetPathStr = node["engineAssetPath"].as<String>();
  stream.close();

  auto engineAssetPath =
      (mAssetCache.getAssetsPath() / engineAssetPathStr).make_preferred();

  mAssetCacheMap.insert_or_assign(
      std::filesystem::canonical(originalAssetPath).string(), engineAssetPath);

  return engineAssetPath;
}

Result<bool> AssetManager::createDirectory(const Path &directory) {
  if (std::filesystem::create_directory(directory) &&
      std::filesystem::create_directory(
          convertToCacheRelativePath(directory))) {
    return Result<bool>::Ok(true);
  }

  return Result<bool>::Error("Could not create directory");
}

Result<Path> AssetManager::createLuaScript(const Path &assetPath) {
  createDirectoriesRecursive(assetPath.parent_path());
  auto createdDirectory = createDirectoriesRecursive(
      convertToCacheRelativePath(assetPath.parent_path()));

  auto originalAssetPath = assetPath;
  originalAssetPath.replace_extension("lua");
  std::ofstream stream(originalAssetPath);
  stream.close();

  auto res = loadOriginalAsset(originalAssetPath);

  if (res.hasError()) {
    return Result<Path>::Error(res.getError());
  }

  return Result<Path>::Ok(originalAssetPath, res.getWarnings());
}

Result<bool>
AssetManager::validateAndPreloadAssets(RenderStorage &renderStorage) {
  LIQUID_PROFILE_EVENT("AssetManager::validateAndPreloadAssets");
  std::vector<String> warnings;

  std::vector<liquid::Path> multiAssetDirs;
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
      auto engineAssetPathStr = node["engineAssetPath"].as<String>();
      stream.close();

      auto engineAssetPath =
          (mAssetCache.getAssetsPath() / engineAssetPathStr).make_preferred();

      if (engineAssetPath.extension() == ".lqprefab" ||
          engineAssetPath.extension() == ".lqenv") {
        multiAssetDirs.push_back(engineAssetPath.parent_path());
      }

      std::filesystem::remove(engineAssetPath);
      std::filesystem::remove(hashFilePath);
    }
  }

  for (const auto &path : multiAssetDirs) {
    std::filesystem::remove_all(path);
  }

  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(mAssetsPath)) {
    if (!entry.is_regular_file()) {
      continue;
    }

    auto res = loadOriginalIfChanged(entry.path());
    warnings.insert(warnings.end(), res.getWarnings().begin(),
                    res.getWarnings().end());
  }

  auto res = mAssetCache.preloadAssets(renderStorage);

  if (res.hasError())
    return res;

  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(mAssetsPath)) {
    if (!entry.is_regular_file()) {
      continue;
    }

    generatePreview(entry.path(), renderStorage);
  }

  warnings.insert(warnings.end(), res.getWarnings().begin(),
                  res.getWarnings().end());

  return Result<bool>::Ok(true, warnings);
}

Result<bool> AssetManager::loadOriginalAsset(const Path &originalAssetPath) {
  auto createdDirectory = createDirectoriesRecursive(
      convertToCacheRelativePath(originalAssetPath.parent_path()));

  auto res = Result<Path>::Error("Unsupported asset format");

  auto type = getAssetTypeFromExtension(originalAssetPath);

  if (type == AssetType::Texture) {
    res = loadOriginalTexture(originalAssetPath);
  } else if (type == AssetType::LuaScript) {
    res = loadOriginalScript(originalAssetPath);
  } else if (type == AssetType::Audio) {
    res = loadOriginalAudio(originalAssetPath);
  } else if (type == AssetType::Font) {
    res = loadOriginalFont(originalAssetPath);
  } else if (type == AssetType::Prefab) {
    res = loadOriginalPrefab(originalAssetPath);
  } else if (type == AssetType::Environment) {
    res = loadOriginalEnvironment(originalAssetPath);
  }

  if (res.hasData()) {
    createHashFile(originalAssetPath, res.getData());
    return Result<bool>::Ok(true, res.getWarnings());
  }

  if (createdDirectory.has_value()) {
    std::filesystem::remove_all(createdDirectory.value());
  }

  return Result<bool>::Error(res.getError());
}

Result<bool>
AssetManager::loadOriginalIfChanged(const Path &originalAssetPath) {
  if (isAssetChanged(originalAssetPath)) {
    return loadOriginalAsset(originalAssetPath);
  }

  return Result<bool>::Ok(true);
}

Result<Path> AssetManager::loadOriginalTexture(const Path &originalAssetPath) {
  int32_t width = 0;
  int32_t height = 0;
  int32_t channels = 0;

  if (originalAssetPath.extension() == ".ktx2") {
    auto engineAssetPath = convertToCacheRelativePath(originalAssetPath);

    if (std::filesystem::copy_file(originalAssetPath, engineAssetPath,
                                   co::overwrite_existing)) {
      auto res = mAssetCache.loadAsset(engineAssetPath);
      if (res.hasData()) {
        return Result<Path>::Ok(engineAssetPath);
      }

      std::filesystem::remove(engineAssetPath);
      return Result<Path>::Error(res.getError());
    }
    return Result<Path>::Error("Cannot load KTX2 texture");
  }

  auto engineAssetPath = getOriginalAssetName(originalAssetPath);
  return mImageLoader.loadFromPath(originalAssetPath, engineAssetPath,
                                   mOptimize);
}

Result<Path> AssetManager::loadOriginalAudio(const Path &originalAssetPath) {
  auto engineAssetPath = convertToCacheRelativePath(originalAssetPath);

  if (std::filesystem::copy_file(originalAssetPath, engineAssetPath,
                                 co::overwrite_existing)) {
    auto res = mAssetCache.loadAsset(engineAssetPath);
    if (res.hasData()) {
      return Result<Path>::Ok(engineAssetPath);
    }

    std::filesystem::remove(engineAssetPath);
  }

  return Result<Path>::Error("Cannot load audio file");
}

Result<Path> AssetManager::loadOriginalScript(const Path &originalAssetPath) {
  auto engineAssetPath = convertToCacheRelativePath(originalAssetPath);

  if (std::filesystem::copy_file(originalAssetPath, engineAssetPath,
                                 co::overwrite_existing)) {
    auto res = mAssetCache.loadAsset(engineAssetPath);
    if (res.hasData()) {
      return Result<Path>::Ok(engineAssetPath);
    }
    std::filesystem::remove(engineAssetPath);
  }

  return Result<Path>::Error("Cannot load script file");
}

Result<Path> AssetManager::loadOriginalFont(const Path &originalAssetPath) {
  auto engineAssetPath = convertToCacheRelativePath(originalAssetPath);

  if (std::filesystem::copy_file(originalAssetPath, engineAssetPath,
                                 co::overwrite_existing)) {
    auto res = mAssetCache.loadAsset(engineAssetPath);
    if (res.hasData()) {
      return Result<Path>::Ok(engineAssetPath);
    }
    std::filesystem::remove(engineAssetPath);
  }

  return Result<Path>::Error("Cannot load font file");
}

Result<Path> AssetManager::loadOriginalPrefab(const Path &originalAssetPath) {
  auto engineAssetPath = convertToCacheRelativePath(originalAssetPath);

  GLTFImporter importer(mAssetCache, mImageLoader, mOptimize);
  return importer.loadFromPath(originalAssetPath, engineAssetPath);
}

Result<Path>
AssetManager::loadOriginalEnvironment(const Path &originalAssetPath) {
  auto engineAssetPath = convertToCacheRelativePath(originalAssetPath);

  return mHDRIImporter.loadFromPath(originalAssetPath, engineAssetPath);
}

String AssetManager::getFileHash(const Path &path) {
  std::ifstream stream(path, std::ios::binary);

  if (!stream.good()) {
    return "";
  }

  String string;
  CryptoPP::SHA256 sha256;
  CryptoPP::FileSource source(
      stream, true,
      new CryptoPP::HashFilter(
          sha256, new CryptoPP::HexEncoder(new CryptoPP::StringSink(string))));

  stream.close();

  return string;
}

Result<Path> AssetManager::createHashFile(const Path &originalAssetPath,
                                          const Path &engineAssetPath) {
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

  return Result<Path>::Ok(hashFilePath);
}

Path AssetManager::convertToCacheRelativePath(const Path &path) const {
  return mAssetCache.getAssetsPath() /
         std::filesystem::relative(path, mAssetsPath);
}

Path AssetManager::getHashFilePath(const Path &originalAssetPath) const {
  auto temp = originalAssetPath;
  auto newExtension = originalAssetPath.extension().string() + ".lqhash";
  return convertToCacheRelativePath(temp.replace_extension(newExtension));
}

String AssetManager::getOriginalAssetName(const Path &path) {
  return std::filesystem::relative(path, mAssetsPath).string();
}

bool AssetManager::isAssetChanged(const Path &assetFilePath) const {
  auto hashFilePath = getHashFilePath(assetFilePath);

  // Checksum is invalid if hash or engine files
  // do not exist
  if (!std::filesystem::exists(hashFilePath)) {
    return true;
  }

  std::ifstream stream(hashFilePath);
  auto node = YAML::Load(stream);
  auto originalAssetHash = node["originalAssetHash"].as<String>();
  auto engineAssetPathStr = node["engineAssetPath"].as<String>();
  stream.close();

  auto engineAssetPath =
      (mAssetCache.getAssetsPath() / engineAssetPathStr).make_preferred();

  if (!std::filesystem::exists(engineAssetPath)) {
    return true;
  }

  return getFileHash(assetFilePath) != originalAssetHash;
}

AssetType AssetManager::getAssetTypeFromExtension(const Path &path) {
  auto ext = path.extension().string();
  ext.erase(0, 1);

  auto isExtension = [&ext](const auto &extensions) {
    return std::find(extensions.begin(), extensions.end(), ext) !=
           extensions.end();
  };

  if (isExtension(TextureExtensions)) {
    return AssetType::Texture;
  }
  if (isExtension(ScriptExtensions)) {
    return AssetType::LuaScript;
  }
  if (isExtension(AudioExtensions)) {
    return AssetType::Audio;
  }
  if (isExtension(FontExtensions)) {
    return AssetType::Font;
  }
  if (isExtension(SceneExtensions)) {
    return AssetType::Prefab;
  }
  if (isExtension(EnvironmentExtensions)) {
    return AssetType::Environment;
  }

  return AssetType::None;
}

std::optional<Path> AssetManager::createDirectoriesRecursive(const Path &path) {
  auto p = path;
  std::optional<Path> lastExisting;
  while (!std::filesystem::exists(p)) {
    lastExisting = p;
    p = p.parent_path();
  }

  std::filesystem::create_directories(path);

  return lastExisting;
}

} // namespace liquid::editor
