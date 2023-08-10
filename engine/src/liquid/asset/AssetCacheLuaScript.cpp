#include "liquid/core/Base.h"

#include "liquid/scripting/LuaInterpreter.h"
#include "liquid/scripting/LuaMessages.h"
#include "AssetCache.h"

namespace liquid {

/**
 * @brief Store text file contents in a buffer
 *
 * @param stream Input stream
 * @return Vector of characters
 */
static std::vector<uint8_t> readFileIntoBuffer(std::ifstream &stream) {
  std::ostringstream ss;
  ss << stream.rdbuf();
  const std::string &s = ss.str();
  std::vector<uint8_t> bytes(s.begin(), s.end());

  return bytes;
}

/**
 * @brief Inject varibale register functions
 *
 * @param scope Lua scope
 * @param data Lua script asset data
 */
static void injectInputVarsInterface(LuaScope &scope, LuaScriptAsset &data) {
  scope.setGlobal<LuaUserData>("__asset", {static_cast<void *>(&data)});

  auto table = scope.createTable(1);

  table.set("register", [](void *state) {
    LuaScope scope(state);
    if (!scope.is<String>(1) || !scope.is<uint32_t>(2)) {
      scope.error(LuaMessages::invalidArguments<String, uint32_t>("input_vars",
                                                                  "register"));
      return 0;
    }

    auto name = scope.get<String>(1);
    auto type = scope.get<uint32_t>(2);

    if (type >= static_cast<uint32_t>(LuaScriptVariableType::Invalid)) {
      scope.error("Variable \"" + name + "\" has invalid type");
      return 0;
    }

    auto &data = *static_cast<LuaScriptAsset *>(
        scope.getGlobal<LuaUserData>("__asset").pointer);

    if (data.variables.find(name) != data.variables.end()) {
      scope.error("Variable \"" + name + "\" already registered");
      return 0;
    }

    LuaScriptVariable var{};
    var.name = name;
    var.type = static_cast<LuaScriptVariableType>(type);
    data.variables.insert({name, var});

    return 0;
  });

  auto typesTable = scope.createTable(3);
  typesTable.set("Null", static_cast<uint32_t>(LuaScriptVariableType::Invalid));
  typesTable.set("String",
                 static_cast<uint32_t>(LuaScriptVariableType::String));
  typesTable.set("AssetPrefab",
                 static_cast<uint32_t>(LuaScriptVariableType::AssetPrefab));
  typesTable.set("AssetTexture",
                 static_cast<uint32_t>(LuaScriptVariableType::AssetTexture));
  table.set("types", typesTable);
  scope.setPreviousValueAsGlobal("input_vars");
}

Result<Path>
liquid::AssetCache::createLuaScriptFromSource(const Path &sourcePath,
                                              const String &uuid) {
  using co = std::filesystem::copy_options;

  auto assetPath = createAssetPath(uuid);

  if (!std::filesystem::copy_file(sourcePath, assetPath,
                                  co::overwrite_existing)) {
    return Result<Path>::Error("Cannot create Lua script from source: " +
                               sourcePath.stem().string());
  }

  auto metaRes = createMetaFile(AssetType::LuaScript,
                                sourcePath.filename().string(), assetPath);

  if (!metaRes.hasData()) {
    std::filesystem::remove(assetPath);
    return Result<Path>::Error("Cannot create Lua script from source: " +
                               sourcePath.stem().string());
  }

  return Result<Path>::Ok(assetPath);
}

Result<LuaScriptAssetHandle>
AssetCache::loadLuaScriptFromFile(const Path &filePath,
                                  LuaScriptAssetHandle handle) {
  std::ifstream stream(filePath);

  if (!stream.good()) {
    return Result<LuaScriptAssetHandle>::Error(
        "File cannot be opened for reading: " + filePath.string());
  }

  auto meta = getMetaFromUuid(filePath.stem().string());

  AssetData<LuaScriptAsset> asset;
  asset.path = filePath;
  asset.name = meta.name;
  asset.type = AssetType::LuaScript;
  asset.uuid = filePath.stem().string();
  asset.data.bytes = readFileIntoBuffer(stream);

  stream.close();

  LuaInterpreter interpreter;
  auto scope = interpreter.createScope();

  injectInputVarsInterface(scope, asset.data);
  bool success = interpreter.evaluate(asset.data.bytes, scope);

  if (!success) {
    auto message = scope.get<String>(-1);
    interpreter.destroyScope(scope);
    return Result<LuaScriptAssetHandle>::Error(message);
  }

  if (!scope.hasFunction("start")) {
    interpreter.destroyScope(scope);
    return Result<LuaScriptAssetHandle>::Error(
        "`start` function is missing from script");
  }

  if (!scope.hasFunction("update")) {
    interpreter.destroyScope(scope);
    return Result<LuaScriptAssetHandle>::Error(
        "`update` function is missing from script");
  }

  interpreter.destroyScope(scope);

  if (handle == LuaScriptAssetHandle::Null) {
    auto newHandle = mRegistry.getLuaScripts().addAsset(asset);
    return Result<LuaScriptAssetHandle>::Ok(newHandle);
  }

  mRegistry.getLuaScripts().updateAsset(handle, asset);

  return Result<LuaScriptAssetHandle>::Ok(handle);
}

} // namespace liquid
