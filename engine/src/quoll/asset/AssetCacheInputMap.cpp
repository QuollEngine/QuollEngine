#include "quoll/core/Base.h"
#include "quoll/input/KeyMappings.h"

#include "AssetCache.h"

namespace quoll {

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

  for (auto scheme : root["schemes"]) {
    if (!scheme.IsMap()) {
      return Result<InputMapAssetHandle>::Error("Scheme item must be a map");
    }

    if (scheme["name"].as<String>("") == "") {
      return Result<InputMapAssetHandle>::Error(
          "Scheme item name must be a string and cannot be empty");
    }
  }

  for (auto command : root["commands"]) {
    if (!command.IsMap()) {
      return Result<InputMapAssetHandle>::Error("Command item must be a map");
    }

    if (command["name"].as<String>("") == "") {
      return Result<InputMapAssetHandle>::Error(
          "Command item name must be a string and cannot be empty");
    }

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
      if (value.size() != 4) {
        return Result<InputMapAssetHandle>::Error(
            "Binding item value cannot be used with axis 2d command");
      }

      for (auto param : value) {
        auto p = param.as<String>("");

        if (!param.IsNull() && p == "") {
          return Result<InputMapAssetHandle>::Error(
              "Binding item value cannot be used with axis 2d command");
        }

        if (!param.IsNull() && !input::exists(p)) {
          return Result<InputMapAssetHandle>::Error(
              "Binding item value is invalid: " + p);
        }
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
      command.type = InputMapCommandType::Axis2d;
    } else if (type == "boolean") {
      command.type = InputMapCommandType::Boolean;
    }

    asset.data.commands.push_back(command);
  }

  for (auto node : root["bindings"]) {
    InputMapBinding binding{};
    binding.command = node["command"].as<size_t>(0);
    binding.scheme = node["scheme"].as<size_t>(0);

    if (node["binding"].IsSequence()) {
      for (auto b : node["binding"]) {
        binding.binding.push_back({input::get(b.as<String>(""))});
      }
    } else {
      binding.binding.push_back({input::get(node["binding"].as<String>(""))});
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
