#include "quoll/core/Base.h"
#include "quoll/yaml/Yaml.h"
#include "quoll/asset/AssetRevision.h"

#include "AssetManager.h"
#include "GLTFImporter.h"
#include "HDRIImporter.h"

#include <stb/stb_image.h>
#include <cryptopp/sha.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>

namespace quoll::editor {

static const String LuaScriptTemplate = R"""(function start()
  -- Initialize your data here
end

function update(dt)
  -- Update data here
end
)""";

static const String AnimatorTemplate = R"""(version: 0.1
type: animator
initial: EMPTY
states:
  EMPTY:
    output: {}
    on: []
)""";

const std::vector<String> AssetManager::TextureExtensions{"png", "jpg", "jpeg",
                                                          "bmp", "tga", "ktx2"};
const std::vector<String> AssetManager::ScriptExtensions{"lua"};
const std::vector<String> AssetManager::AudioExtensions{"wav"};
const std::vector<String> AssetManager::FontExtensions{"ttf", "otf"};
const std::vector<String> AssetManager::PrefabExtensions{"gltf", "glb"};
const std::vector<String> AssetManager::EnvironmentExtensions{"hdr"};
const std::vector<String> AssetManager::AnimatorExtensions{"animator"};
const std::vector<String> AssetManager::SceneExtensions{"scene"};

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
                           RenderStorage &renderStorage, bool optimize,
                           bool createDefaultObjects)
    : mRenderStorage(renderStorage), mAssetsPath(assetsPath),
      mAssetCache(assetsCachePath, createDefaultObjects),
      mImageLoader(mAssetCache, renderStorage),
      mHDRIImporter(mAssetCache, renderStorage), mOptimize(optimize) {}

Result<Path> AssetManager::importAsset(const Path &source,
                                       const Path &targetAssetDirectory) {
  auto createdDirectory = createDirectoriesRecursive(targetAssetDirectory);

  auto targetAssetPath =
      getUniquePath(targetAssetDirectory / source.filename());

  if (targetAssetPath.extension() == ".gltf" ||
      targetAssetPath.extension() == ".glb") {
    auto res = GLTFImporter::createEmbeddedGlb(source, targetAssetPath);
    if (res.hasError()) {
      return Result<Path>::Error(res.getError());
    }

    targetAssetPath = res.getData();
  } else {
    std::filesystem::copy_file(source, targetAssetPath, co::overwrite_existing);
  }

  auto res = loadSourceAsset(targetAssetPath, {});

  if (res.hasError()) {
    std::filesystem::remove(targetAssetPath);
    if (createdDirectory.has_value()) {
      std::filesystem::remove_all(createdDirectory.value());
    }
  }

  if (res.hasError()) {
    return Result<Path>::Error(res.getError());
  }

  return Result<Path>::Ok(targetAssetPath, res.getWarnings());
}

void AssetManager::generatePreview(const Path &sourceAssetPath,
                                   RenderStorage &renderStorage) {
  const auto &uuid = findRootAssetUuid(sourceAssetPath);
  const auto &res = mAssetCache.getRegistry().getAssetByUuid(uuid);
  auto handle = res.second;

  if (res.first == AssetType::Environment) {
    auto &asset = mAssetCache.getRegistry().getEnvironments().getAsset(
        static_cast<quoll::EnvironmentAssetHandle>(handle));
    if (!rhi::isHandleValid(asset.preview)) {
      asset.preview =
          mHDRIImporter.loadFromPathToDevice(sourceAssetPath, renderStorage);
    }
  } else if (res.first == AssetType::Texture) {
    auto &asset = mAssetCache.getRegistry().getTextures().getAsset(
        static_cast<quoll::TextureAssetHandle>(handle));

    if (!rhi::isHandleValid(asset.preview)) {
      asset.preview = asset.data.deviceHandle;
    }
  }
}

Uuid AssetManager::findRootAssetUuid(const Path &sourceAssetPath) {
  auto it = mAssetCacheMap.find(sourceAssetPath.string());
  if (it != mAssetCacheMap.end()) {
    return it->second;
  }

  auto metaFilePath = getMetaFilePath(sourceAssetPath);
  std::ifstream stream(metaFilePath);

  if (!stream.good()) {
    return Uuid{};
  }

  auto node = YAML::Load(stream);
  stream.close();

  auto uuid = node["uuid"]["root"].as<Uuid>(Uuid{});

  mAssetCacheMap.insert_or_assign(
      std::filesystem::canonical(sourceAssetPath).string(), uuid);

  return uuid;
}

Result<bool> AssetManager::createDirectory(const Path &directory) {
  if (std::filesystem::create_directory(directory)) {
    return Result<bool>::Ok(true);
  }

  return Result<bool>::Error("Could not create directory");
}

Result<Path> AssetManager::createLuaScript(const Path &assetPath) {
  createDirectoriesRecursive(assetPath.parent_path());

  auto sourceAssetPath = assetPath;
  sourceAssetPath.replace_extension("lua");
  std::ofstream stream(sourceAssetPath);
  stream << LuaScriptTemplate;
  stream.close();

  auto res = loadSourceAsset(sourceAssetPath, {});

  if (res.hasError()) {
    return Result<Path>::Error(res.getError());
  }

  return Result<Path>::Ok(sourceAssetPath, res.getWarnings());
}

Result<Path> AssetManager::createAnimator(const Path &assetPath) {
  createDirectoriesRecursive(assetPath.parent_path());

  auto sourceAssetPath = assetPath;
  sourceAssetPath.replace_extension("animator");
  std::ofstream stream(sourceAssetPath);
  stream << AnimatorTemplate;
  stream.close();

  auto res = loadSourceAsset(sourceAssetPath, {});
  if (res.hasError()) {
    return Result<Path>::Error(res.getError());
  }

  return Result<Path>::Ok(sourceAssetPath, res.getWarnings());
}

Result<bool> AssetManager::reloadAssets() {
  LIQUID_PROFILE_EVENT("AssetManager::reloadAssets");

  std::vector<String> warnings;

  std::unordered_map<String, bool> allLoadedUuids{};

  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(mAssetsPath)) {
    if (!entry.is_regular_file() || entry.path().extension() == ".meta") {
      continue;
    }

    auto res = loadSourceIfChanged(entry.path());

    if (res.hasData()) {
      for (const auto &[_, uuid] : res.getData()) {
        allLoadedUuids.insert_or_assign(uuid.toString(), true);
      }
    }

    if (res.hasError()) {
      warnings.push_back(res.getError());
    } else {
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    }
  }

  for (const auto &entry : std::filesystem::recursive_directory_iterator(
           mAssetCache.getAssetsPath())) {
    if (!entry.is_regular_file()) {
      continue;
    }

    auto uuid = entry.path().stem().string();
    auto it = allLoadedUuids.find(uuid);
    if (it == allLoadedUuids.end()) {
      std::filesystem::remove(entry.path());
      warnings.push_back(entry.path().filename().string() +
                         " is removed because it did not refer to any asset");
    }
  }

  return Result<bool>::Ok(true, warnings);
}

Result<bool>
AssetManager::validateAndPreloadAssets(RenderStorage &renderStorage) {
  LIQUID_PROFILE_EVENT("AssetManager::validateAndPreloadAssets");
  auto reloadRes = reloadAssets();

  auto res = mAssetCache.preloadAssets(renderStorage);
  auto warnings = res.getWarnings();

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

Result<UUIDMap> AssetManager::loadSourceAsset(const Path &sourceAssetPath,
                                              const UUIDMap &uuids) {
  auto res = Result<UUIDMap>::Error("Unsupported asset format: " +
                                    sourceAssetPath.filename().string());

  auto type = getAssetTypeFromExtension(sourceAssetPath);

  if (type == AssetType::Texture) {
    res = loadSourceTexture(sourceAssetPath, uuids);
  } else if (type == AssetType::LuaScript) {
    res = loadSourceScript(sourceAssetPath, uuids);
  } else if (type == AssetType::Audio) {
    res = loadSourceAudio(sourceAssetPath, uuids);
  } else if (type == AssetType::Font) {
    res = loadSourceFont(sourceAssetPath, uuids);
  } else if (type == AssetType::Prefab) {
    res = loadSourcePrefab(sourceAssetPath, uuids);
  } else if (type == AssetType::Environment) {
    res = loadSourceEnvironment(sourceAssetPath, uuids);
  } else if (type == AssetType::Animator) {
    res = loadSourceAnimator(sourceAssetPath, uuids);
  } else if (type == AssetType::Scene) {
    res = loadSourceScene(sourceAssetPath, uuids);
  }

  if (res.hasData()) {
    createMetaFile(sourceAssetPath, res.getData(),
                   getRevisionForAssetType(type));
  }

  return res;
}

Result<UUIDMap> AssetManager::loadSourceIfChanged(const Path &sourceAssetPath) {
  auto uuids = getUuidsFromMeta(sourceAssetPath);
  if (isAssetChanged(sourceAssetPath)) {
    return loadSourceAsset(sourceAssetPath, uuids);
  }

  return Result<UUIDMap>::Ok(uuids);
}

Result<UUIDMap> AssetManager::loadSourceTexture(const Path &sourceAssetPath,
                                                const UUIDMap &uuids) {
  int32_t width = 0;
  int32_t height = 0;
  int32_t channels = 0;

  if (sourceAssetPath.extension() == ".ktx2") {
    auto uuid = getOrCreateUuidFromMap(uuids, "root");
    auto createRes = mAssetCache.createTextureFromSource(sourceAssetPath, uuid);
    if (!createRes.hasData()) {
      return Result<UUIDMap>::Error(createRes.getError());
    }

    auto res = mAssetCache.loadTexture(uuid);

    if (res.hasData()) {
      auto uuid =
          mAssetCache.getRegistry().getTextures().getAsset(res.getData()).uuid;
      return Result<UUIDMap>::Ok({{"root", uuid}});
    }

    return Result<UUIDMap>::Error(res.getError());
  }

  auto uuid = mImageLoader.loadFromPath(sourceAssetPath,
                                        getOrCreateUuidFromMap(uuids, "root"),
                                        mOptimize, rhi::Format::Rgba8Srgb);

  if (uuid.hasError()) {
    return Result<UUIDMap>::Error(uuid.getError());
  }

  return Result<UUIDMap>::Ok({{"root", uuid.getData()}});
}

Result<UUIDMap> AssetManager::loadSourceAudio(const Path &sourceAssetPath,
                                              const UUIDMap &uuids) {
  auto uuid = getOrCreateUuidFromMap(uuids, "root");
  auto createRes = mAssetCache.createAudioFromSource(sourceAssetPath, uuid);
  if (!createRes.hasData()) {
    return Result<UUIDMap>::Error(createRes.getError());
  }

  auto res = mAssetCache.loadAudio(uuid);

  if (res.hasData()) {
    return Result<UUIDMap>::Ok({{"root", uuid}});
  }

  return Result<UUIDMap>::Error(res.getError());
}

Result<UUIDMap> AssetManager::loadSourceScript(const Path &sourceAssetPath,
                                               const UUIDMap &uuids) {
  auto uuid = getOrCreateUuidFromMap(uuids, "root");
  auto createRes = mAssetCache.createLuaScriptFromSource(sourceAssetPath, uuid);
  if (!createRes.hasData()) {
    return Result<UUIDMap>::Error(createRes.getError());
  }

  auto res = mAssetCache.loadLuaScript(uuid);

  if (res.hasData()) {
    auto uuid =
        mAssetCache.getRegistry().getLuaScripts().getAsset(res.getData()).uuid;
    return Result<UUIDMap>::Ok({{"root", uuid}});
  }

  return Result<UUIDMap>::Error(res.getError());
}

Result<UUIDMap> AssetManager::loadSourceFont(const Path &sourceAssetPath,
                                             const UUIDMap &uuids) {
  auto uuid = getOrCreateUuidFromMap(uuids, "root");
  auto createRes = mAssetCache.createFontFromSource(sourceAssetPath, uuid);
  if (!createRes.hasData()) {
    return Result<UUIDMap>::Error(createRes.getError());
  }

  auto res = mAssetCache.loadFont(uuid);

  if (res.hasData()) {
    auto uuid =
        mAssetCache.getRegistry().getFonts().getAsset(res.getData()).uuid;
    return Result<UUIDMap>::Ok({{"root", uuid}});
  }

  return Result<UUIDMap>::Error(res.getError());
}

Result<UUIDMap> AssetManager::loadSourceAnimator(const Path &sourceAssetPath,
                                                 const UUIDMap &uuids) {
  auto uuid = getOrCreateUuidFromMap(uuids, "root");
  auto createRes = mAssetCache.createAnimatorFromSource(sourceAssetPath, uuid);
  if (!createRes.hasData()) {
    return Result<UUIDMap>::Error(createRes.getError());
  }

  auto res = mAssetCache.loadAnimator(uuid);

  if (res.hasData()) {
    auto uuid =
        mAssetCache.getRegistry().getAnimators().getAsset(res.getData()).uuid;
    return Result<UUIDMap>::Ok({{"root", uuid}});
  }

  return Result<UUIDMap>::Error(res.getError());
}

Result<UUIDMap> AssetManager::loadSourcePrefab(const Path &sourceAssetPath,
                                               const UUIDMap &uuids) {
  GLTFImporter importer(mAssetCache, mImageLoader, mOptimize);
  return importer.loadFromPath(sourceAssetPath, uuids);
}

Result<UUIDMap> AssetManager::loadSourceEnvironment(const Path &sourceAssetPath,
                                                    const UUIDMap &uuids) {
  return mHDRIImporter.loadFromPath(sourceAssetPath, uuids);
}

Result<UUIDMap> AssetManager::loadSourceScene(const Path &sourceAssetPath,
                                              const UUIDMap &uuids) {
  auto uuid = getOrCreateUuidFromMap(uuids, "root");
  auto createRes = mAssetCache.createSceneFromSource(sourceAssetPath, uuid);
  if (!createRes.hasData()) {
    return Result<UUIDMap>::Error(createRes.getError());
  }

  auto res = mAssetCache.loadScene(uuid);

  if (res.hasData()) {
    return Result<UUIDMap>::Ok({{"root", uuid}});
  }

  return Result<UUIDMap>::Error(res.getError());
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

UUIDMap AssetManager::getUuidsFromMeta(const Path &sourceAssetPath) const {
  auto metaFilePath = getMetaFilePath(sourceAssetPath);

  std::ifstream stream(metaFilePath);
  if (!stream.good()) {
    return {};
  }

  auto node = YAML::Load(stream);
  stream.close();

  if (!node["uuid"] || !node["uuid"].IsMap()) {
    return {};
  }

  UUIDMap uuidMap;
  for (const auto &pair : node["uuid"]) {
    if (pair.second.IsScalar()) {
      uuidMap.insert_or_assign(pair.first.as<String>(),
                               pair.second.as<Uuid>(Uuid{}));
    } else {
      uuidMap.insert_or_assign(pair.first.as<String>(), Uuid{});
    }
  }

  return uuidMap;
}

Result<Path> AssetManager::createMetaFile(const Path &sourceAssetPath,
                                          const UUIDMap &uuids,
                                          AssetRevision revision) {
  auto filename = sourceAssetPath.filename();
  auto metaFilePath = getMetaFilePath(sourceAssetPath);

  YAML::Node node;
  node["sourceHash"] = getFileHash(sourceAssetPath);

  for (const auto &pair : uuids) {
    node["uuid"][pair.first] = pair.second;
  }

  node["revision"] = static_cast<uint32_t>(revision);

  std::ofstream stream(metaFilePath);
  stream << node;
  stream.close();

  mAssetCacheMap.erase(std::filesystem::canonical(sourceAssetPath).string());

  return Result<Path>::Ok(metaFilePath);
}

Path AssetManager::getMetaFilePath(const Path &sourceAssetPath) const {
  auto temp = sourceAssetPath;
  auto newExtension = sourceAssetPath.extension().string() + ".meta";
  return temp.replace_extension(newExtension);
}

bool AssetManager::isAssetChanged(const Path &assetFilePath) const {
  auto metaFilePath = getMetaFilePath(assetFilePath);

  if (!std::filesystem::exists(metaFilePath)) {
    return true;
  }

  std::ifstream stream(metaFilePath);
  auto node = YAML::Load(stream);
  stream.close();

  auto sourceAssetHash = node["sourceHash"].as<String>("");
  auto revision = AssetRevision{node["revision"].as<uint32_t>(0)};

  if (!node["uuid"] || !node["uuid"].IsMap()) {
    return {};
  }

  for (const auto &pair : node["uuid"]) {
    if (!pair.second.IsScalar()) {
      return true;
    }

    auto uuid = pair.second.as<String>("");
    if (uuid.empty()) {
      return true;
    }

    auto engineAssetPath =
        (mAssetCache.getAssetsPath() / uuid).replace_extension("asset");

    if (!std::filesystem::exists(engineAssetPath) ||
        !std::filesystem::is_regular_file(engineAssetPath)) {
      return true;
    }
  }

  auto type = getAssetTypeFromExtension(assetFilePath);

  return getFileHash(assetFilePath) != sourceAssetHash ||
         getRevisionForAssetType(type) != revision;
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
  if (isExtension(PrefabExtensions)) {
    return AssetType::Prefab;
  }
  if (isExtension(EnvironmentExtensions)) {
    return AssetType::Environment;
  }
  if (isExtension(AnimatorExtensions)) {
    return AssetType::Animator;
  }
  if (isExtension(SceneExtensions)) {
    return AssetType::Scene;
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

} // namespace quoll::editor
