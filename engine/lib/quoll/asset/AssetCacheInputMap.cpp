#include "quoll/core/Base.h"
#include "quoll/input/KeyMappings.h"
#include "AssetCache.h"

namespace quoll {

namespace {

Result<void> checkBoolean(YAML::Node node) {
  if (node.IsNull()) {
    return Ok();
  }

  auto value = node.as<String>("");

  if (!input::exists(value)) {
    return Error("Binding item value is invalid: " + value);
  }

  if (input::getKeyDataType(value) != InputDataType::Boolean) {
    return Error("Binding item value must be boolean: " + value);
  }

  return Ok();
}

Result<void> checkAxis1d(YAML::Node node) {
  if (node.IsNull()) {
    return Ok();
  }

  if (node.IsScalar()) {
    auto value = node.as<String>("");

    if (!input::exists(value)) {
      return Error("Binding item value is invalid: " + value);
    }

    if (input::getKeyDataType(value) != InputDataType::Axis1d) {
      return Error("Binding item value must be axis-1d: " + value);
    }

    return Ok();
  }

  if (node.IsSequence() && node.size() == 2) {
    for (auto segment : node) {
      auto res = checkBoolean(segment);
      if (!res) {
        return res;
      }
    }

    return Ok();
  }

  return Error("Invalid binding item value");
}

Result<void> checkAxis2d(YAML::Node node) {
  if (node.IsNull()) {
    return Ok();
  }

  if (node.IsScalar()) {
    auto value = node.as<String>("");

    if (!input::exists(value)) {
      return Error("Binding item value is invalid: " + value);
    }

    if (input::getKeyDataType(value) != InputDataType::Axis2d) {
      return Error("Binding item value must be axis-2d: " + value);
    }

    return Ok();
  }

  if (node.IsMap()) {
    if (!node["x"] || !node["y"]) {
      return Error(
          "Both `x` and `y` values must be provided for expanded axis-2d");
    }

    auto resX = checkAxis1d(node["x"]);
    if (!resX) {
      return resX;
    }

    auto resY = checkAxis1d(node["y"]);
    if (!resY) {
      return resY;
    }

    return Ok();
  }

  return Error("Invalid binding item value");
}

} // namespace

Result<InputMapAsset> AssetCache::loadInputMap(const Path &path) {
  std::ifstream stream(path);
  auto root = YAML::Load(stream);
  stream.close();

  // Validation
  if (root["type"].as<String>("") != "inputmap") {
    return Error("Type must be input map");
  }

  if (root["version"].as<String>("") != "0.1") {
    return Error("Version is not supported");
  }

  if (!root["schemes"] || !root["schemes"].IsSequence()) {
    return Error("Schemes field must be a list");
  }

  if (!root["commands"] || !root["commands"].IsSequence()) {
    return Error("Commands field must be a list");
  }

  if (!root["bindings"] || !root["bindings"].IsSequence()) {
    return Error("Bindings field must be a list");
  }

  std::set<String> duplicateNames;
  for (auto scheme : root["schemes"]) {
    if (!scheme.IsMap()) {
      return Error("Scheme item must be a map");
    }

    auto name = scheme["name"].as<String>("");
    if (name == "") {
      return Error("Scheme item name must be a string and cannot be empty");
    }

    if (duplicateNames.contains(name)) {
      return Error("Scheme item with same name is found");
    }

    duplicateNames.insert(name);
  }

  duplicateNames.clear();

  for (auto command : root["commands"]) {
    if (!command.IsMap()) {
      return Error("Command item must be a map");
    }

    auto name = command["name"].as<String>("");

    if (name == "") {
      return Error("Command item name must be a string and cannot be empty");
    }

    if (duplicateNames.contains(name)) {
      return Error("Scheme item with same name is found");
    }

    duplicateNames.insert(name);

    auto type = command["type"].as<String>("");
    if (type != "boolean" && type != "axis-2d") {
      return Error("Command item type must be \"boolean\" or \"axis-2d\"");
    }
  }

  for (auto binding : root["bindings"]) {
    if (!binding.IsMap()) {
      return Error("Binding item must be a map");
    }

    auto scheme = binding["scheme"].as<i32>(-1);
    if (scheme == -1) {
      return Error("Binding item scheme is invalid");
    }

    auto command = binding["command"].as<i32>(-1);
    if (command == -1) {
      return Error("Binding item command is invalid");
    }

    if (scheme >= static_cast<i32>(root["schemes"].size())) {
      return Error("Binding item scheme does not point to scheme item");
    }

    if (command >= static_cast<i32>(root["commands"].size())) {
      return Error("Binding item command does not point to command item");
    }

    auto value = binding["binding"];

    auto commandType =
        root["commands"][binding["command"].as<usize>()]["type"].as<String>("");

    if (commandType == "boolean") {
      auto p = value.as<String>("");
      if (!value.IsNull() && p == "") {
        return Error("Binding item value cannot be used with boolean command");
      }

      if (!value.IsNull() && !input::exists(p)) {
        return Error("Binding item value is invalid: " + p);
      }
    }

    if (commandType == "axis-2d") {
      auto res = checkAxis2d(value);

      if (!res) {
        return res.error();
      }
    }
  }

  // Load
  InputMapAsset asset{};
  asset.schemes.reserve(root["schemes"].size());
  asset.commands.reserve(root["commands"].size());
  asset.bindings.reserve(root["bindings"].size());

  for (auto node : root["schemes"]) {
    InputMapScheme scheme{};
    scheme.name = node["name"].as<String>("");
    asset.schemes.push_back(scheme);
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

    asset.commands.push_back(command);
  }

  for (auto node : root["bindings"]) {
    InputMapBinding binding{};
    binding.command = node["command"].as<usize>(0);
    binding.scheme = node["scheme"].as<usize>(0);

    auto &command = asset.commands.at(binding.command);

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

    asset.bindings.push_back(binding);
  }

  return asset;
}

} // namespace quoll
