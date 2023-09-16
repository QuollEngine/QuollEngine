#include "quoll/core/Base.h"
#include "quoll/input/KeyMappings.h"

#include "AssetCache.h"

namespace quoll {

static Result<bool> checkBoolean(YAML::Node node) {
  if (node.IsNull()) {
    return Result<bool>::Ok(true);
  }

  auto value = node.as<String>("");

  if (!input::exists(value)) {
    return Result<bool>::Error("Binding item value is invalid: " + value);
  }

  if (input::getKeyDataType(value) != InputDataType::Boolean) {
    return Result<bool>::Error("Binding item value must be boolean: " + value);
  }

  return Result<bool>::Ok(true);
}

static Result<bool> checkAxis1d(YAML::Node node) {
  if (node.IsNull()) {
    return Result<bool>::Ok(true);
  }

  if (node.IsScalar()) {
    auto value = node.as<String>("");

    if (!input::exists(value)) {
      return Result<bool>::Error("Binding item value is invalid: " + value);
    }

    if (input::getKeyDataType(value) != InputDataType::Axis1d) {
      return Result<bool>::Error("Binding item value must be axis-1d: " +
                                 value);
    }

    return Result<bool>::Ok(true);
  }

  if (node.IsSequence() && node.size() == 2) {
    for (auto segment : node) {
      auto res = checkBoolean(segment);
      if (res.hasError()) {
        return res;
      }
    }

    return Result<bool>::Ok(true);
  }

  return Result<bool>::Error("Invalid binding item value");
}

static Result<bool> checkAxis2d(YAML::Node node) {
  if (node.IsNull()) {
    return Result<bool>::Ok(true);
  }

  if (node.IsScalar()) {
    auto value = node.as<String>("");

    if (!input::exists(value)) {
      return Result<bool>::Error("Binding item value is invalid: " + value);
    }

    if (input::getKeyDataType(value) != InputDataType::Axis2d) {
      return Result<bool>::Error("Binding item value must be axis-2d: " +
                                 value);
    }

    return Result<bool>::Ok(true);
  }

  if (node.IsMap()) {
    if (!node["x"] || !node["y"]) {
      return Result<bool>::Error(
          "Both `x` and `y` values must be provided for expanded axis-2d");
    }

    auto resX = checkAxis1d(node["x"]);
    if (resX.hasError()) {
      return resX;
    }

    auto resY = checkAxis1d(node["y"]);
    if (resY.hasError()) {
      return resY;
    }

    return Result<bool>::Ok(true);
  }

  return Result<bool>::Error("Invalid binding item value");
}

Result<InputMapAssetHandle> AssetCache::loadInputMap(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  std::ifstream stream(filePath);
  auto root = YAML::Load(stream);
  stream.close();

  // Validation
  if (root["type"].as<String>("") != "inputmap") {
    return Result<InputMapAssetHandle>::Error("Type must be input map");
  }

  if (root["version"].as<String>("") != "0.1") {
    return Result<InputMapAssetHandle>::Error("Version is not supported");
  }

  if (!root["schemes"] || !root["schemes"].IsSequence()) {
    return Result<InputMapAssetHandle>::Error("Schemes field must be a list");
  }

  if (!root["commands"] || !root["commands"].IsSequence()) {
    return Result<InputMapAssetHandle>::Error("Commands field must be a list");
  }

  if (!root["bindings"] || !root["bindings"].IsSequence()) {
    return Result<InputMapAssetHandle>::Error("Bindings field must be a list");
  }

  std::set<String> duplicateNames;
  for (auto scheme : root["schemes"]) {
    if (!scheme.IsMap()) {
      return Result<InputMapAssetHandle>::Error("Scheme item must be a map");
    }

    auto name = scheme["name"].as<String>("");
    if (name == "") {
      return Result<InputMapAssetHandle>::Error(
          "Scheme item name must be a string and cannot be empty");
    }

    if (duplicateNames.contains(name)) {
      return Result<InputMapAssetHandle>::Error(
          "Scheme item with same name is found");
    }

    duplicateNames.insert(name);
  }

  duplicateNames.clear();

  for (auto command : root["commands"]) {
    if (!command.IsMap()) {
      return Result<InputMapAssetHandle>::Error("Command item must be a map");
    }

    auto name = command["name"].as<String>("");

    if (name == "") {
      return Result<InputMapAssetHandle>::Error(
          "Command item name must be a string and cannot be empty");
    }

    if (duplicateNames.contains(name)) {
      return Result<InputMapAssetHandle>::Error(
          "Scheme item with same name is found");
    }

    duplicateNames.insert(name);

    auto type = command["type"].as<String>("");
    if (type != "boolean" && type != "axis-2d") {
      return Result<InputMapAssetHandle>::Error(
          "Command item type must be \"boolean\" or \"axis-2d\"");
    }
  }

  for (auto binding : root["bindings"]) {
    if (!binding.IsMap()) {
      return Result<InputMapAssetHandle>::Error("Binding item must be a map");
    }

    auto scheme = binding["scheme"].as<int32_t>(-1);
    if (scheme == -1) {
      return Result<InputMapAssetHandle>::Error(
          "Binding item scheme is invalid");
    }

    auto command = binding["command"].as<int32_t>(-1);
    if (command == -1) {
      return Result<InputMapAssetHandle>::Error(
          "Binding item command is invalid");
    }

    if (scheme >= static_cast<int32_t>(root["schemes"].size())) {
      return Result<InputMapAssetHandle>::Error(
          "Binding item scheme does not point to scheme item");
    }

    if (command >= static_cast<int32_t>(root["commands"].size())) {
      return Result<InputMapAssetHandle>::Error(
          "Binding item command does not point to command item");
    }

    auto value = binding["binding"];

    auto commandType =
        root["commands"][binding["command"].as<size_t>()]["type"].as<String>(
            "");

    if (commandType == "boolean") {
      auto p = value.as<String>("");
      if (!value.IsNull() && p == "") {
        return Result<InputMapAssetHandle>::Error(
            "Binding item value cannot be used with boolean command");
      }

      if (!value.IsNull() && !input::exists(p)) {
        return Result<InputMapAssetHandle>::Error(
            "Binding item value is invalid: " + p);
      }
    }

    if (commandType == "axis-2d") {
      auto res = checkAxis2d(value);

      if (res.hasError()) {
        return Result<InputMapAssetHandle>::Error(res.getError());
      }
    }
  }

  // Load
  auto meta = getAssetMeta(uuid);

  AssetData<InputMapAsset> asset{};
  asset.type = AssetType::InputMap;
  asset.name = meta.name;
  asset.path = filePath;
  asset.uuid = Uuid(filePath.stem().string());

  asset.data.schemes.reserve(root["schemes"].size());
  asset.data.commands.reserve(root["commands"].size());
  asset.data.bindings.reserve(root["bindings"].size());

  for (auto node : root["schemes"]) {
    InputMapScheme scheme{};
    scheme.name = node["name"].as<String>("");
    asset.data.schemes.push_back(scheme);
  }

  for (auto node : root["commands"]) {
    InputMapCommand command{};
    command.name = node["name"].as<String>("");

    auto type = node["type"].as<String>("");
    if (type == "axis-2d") {
      command.type = InputDataType::Axis2d;
    } else if (type == "boolean") {
      command.type = InputDataType::Boolean;
    }

    asset.data.commands.push_back(command);
  }

  for (auto node : root["bindings"]) {
    InputMapBinding binding{};
    binding.command = node["command"].as<size_t>(0);
    binding.scheme = node["scheme"].as<size_t>(0);

    auto &command = asset.data.commands.at(binding.command);

    if (node["binding"].IsNull()) {
      binding.value = -1;
    }

    if (node["binding"].IsScalar()) {
      binding.value = input::get(node["binding"].as<String>(""));
    } else if (node["binding"]["x"] && node["binding"]["y"]) {
      InputMapAxis2dValue value2d{};
      if (node["binding"]["x"].IsScalar()) {
        value2d.x = input::get(node["binding"]["x"].as<String>(""));
      } else if (node["binding"]["x"].IsSequence()) {
        value2d.x =
            std::array{input::get(node["binding"]["x"][0].as<String>("")),
                       input::get(node["binding"]["x"][1].as<String>(""))};
      }

      if (node["binding"]["y"].IsScalar()) {
        value2d.y = input::get(node["binding"]["y"].as<String>(""));
      } else if (node["binding"]["y"].IsSequence()) {
        value2d.y =
            std::array{input::get(node["binding"]["y"][0].as<String>("")),
                       input::get(node["binding"]["y"][1].as<String>(""))};
      }

      binding.value = value2d;
    }

    if (command.type == InputDataType::Boolean) {
    }

    asset.data.bindings.push_back(binding);
  }

  auto handle = mRegistry.getInputMaps().findHandleByUuid(uuid);

  if (handle == InputMapAssetHandle::Null) {
    auto newHandle = mRegistry.getInputMaps().addAsset(asset);
    return Result<InputMapAssetHandle>::Ok(newHandle);
  }

  mRegistry.getInputMaps().updateAsset(handle, asset);

  return Result<InputMapAssetHandle>::Ok(handle);
}

Result<Path> AssetCache::createInputMapFromSource(const Path &sourcePath,
                                                  const Uuid &uuid) {
  if (uuid.isEmpty()) {
    QuollAssert(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  using co = std::filesystem::copy_options;

  auto assetPath = getPathFromUuid(uuid);

  if (!std::filesystem::copy_file(sourcePath, assetPath,
                                  co::overwrite_existing)) {
    return Result<Path>::Error("Cannot create input map from source: " +
                               sourcePath.stem().string());
  }

  auto metaRes = createAssetMeta(AssetType::InputMap,
                                 sourcePath.filename().string(), assetPath);

  if (!metaRes.hasData()) {
    std::filesystem::remove(assetPath);
    return Result<Path>::Error("Cannot create input map from source: " +
                               sourcePath.stem().string());
  }

  return Result<Path>::Ok(assetPath);
}

} // namespace quoll
