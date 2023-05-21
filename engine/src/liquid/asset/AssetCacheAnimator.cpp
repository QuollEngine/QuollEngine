#include "liquid/core/Base.h"
#include "liquid/core/Version.h"
#include "liquid/yaml/Yaml.h"

#include "AssetCache.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

Result<AnimatorAssetHandle>
AssetCache::getOrLoadAnimatorFromPath(StringView relativePath) {
  if (relativePath.empty()) {
    return Result<AnimatorAssetHandle>::Ok(AnimatorAssetHandle::Null);
  }

  Path fullPath = (mAssetsPath / relativePath).make_preferred();

  for (auto &[handle, asset] : mRegistry.getAnimators().getAssets()) {
    if (asset.path == fullPath) {
      return Result<AnimatorAssetHandle>::Ok(handle);
    }
  }

  return loadAnimatorFromFile(fullPath);
}

Result<Path>
AssetCache::createAnimatorFromAsset(const AssetData<AnimatorAsset> &asset) {
  YAML::Node root;
  root["version"] = "0.1";
  root["type"] = "animator";
  root["initial"] = asset.data.states.at(asset.data.initialState).name;

  auto statesNode = root["states"];

  for (const auto &state : asset.data.states) {
    auto stateNode = statesNode[state.name];
    stateNode["output"]["type"] = "animation";
    stateNode["output"]["animation"] =
        getAssetRelativePath(mRegistry.getAnimations(), state.animation);

    for (const auto &transition : state.transitions) {
      YAML::Node transitionNode(YAML::NodeType::Map);
      transitionNode["type"] = "event";
      transitionNode["event"] = transition.eventName;
      transitionNode["target"] = asset.data.states.at(transition.target).name;

      stateNode["on"].push_back(transitionNode);
    }
  }

  std::ofstream stream(mAssetsPath / asset.relativePath);
  stream << root;
  stream.close();

  return Result<Path>::Ok(mAssetsPath / asset.relativePath);
}

Result<AnimatorAssetHandle>
AssetCache::loadAnimatorFromFile(const Path &filePath,
                                 AnimatorAssetHandle handle) {
  std::ifstream stream(filePath);
  auto root = YAML::Load(stream);
  stream.close();

  if (root["type"].as<String>("") != "animator") {
    return Result<AnimatorAssetHandle>::Error("Type must be animator");
  }

  if (root["version"].as<String>("") != "0.1") {
    return Result<AnimatorAssetHandle>::Error("Version is not supported");
  }

  if (!root["states"] || !root["states"].IsMap()) {
    return Result<AnimatorAssetHandle>::Error("`states` field must be a map");
  }

  AssetData<AnimatorAsset> asset{};
  asset.relativePath = std::filesystem::relative(filePath, mAssetsPath);
  asset.type = AssetType::Animator;
  asset.path = filePath;

  std::vector<String> warnings;

  std::vector<YAML::Node> transitionNodes;

  for (auto stateNodePair : root["states"]) {
    auto name = stateNodePair.first.as<String>("");
    auto stateNode = stateNodePair.second;

    if (!stateNode || !stateNode.IsMap()) {
      warnings.push_back("State value for " + name +
                         " is ignored because it is not a map");
      continue;
    }

    AnimationState state{};
    state.name = name;

    auto output = stateNode["output"];
    if (output["type"] && output["type"].as<String>("") == "animation") {
      auto animation = output["animation"].as<String>("");
      if (!animation.empty()) {
        auto res = getOrLoadAnimationFromPath(animation);
        if (res.hasData()) {
          state.animation = res.getData();
        }

        if (res.hasWarnings()) {
          warnings.insert(warnings.end(), res.getWarnings().begin(),
                          res.getWarnings().end());
        }

        if (res.hasError()) {
          warnings.push_back(res.getError());
        }
      }
    }

    transitionNodes.push_back(stateNode["on"]);
    asset.data.states.push_back(state);
  }

  for (size_t i = 0; i < transitionNodes.size(); ++i) {
    auto &state = asset.data.states.at(i);
    auto &stateOn = transitionNodes.at(i);
    if (!stateOn || !stateOn.IsSequence()) {
      continue;
    }

    for (size_t j = 0; j < stateOn.size(); ++j) {
      auto transitionIndex =
          "Transition at index " + std::to_string(j) + " of " + state.name;

      auto transitionNode = stateOn[j];
      if (!transitionNode.IsMap()) {
        warnings.push_back(transitionIndex +
                           " is ignored because it is not a map");
        continue;
      }

      if (!transitionNode["type"]) {
        warnings.push_back(transitionIndex +
                           " is ignored because `type` does not exist");
        continue;
      }

      if (transitionNode["type"].as<String>("") != "event") {
        warnings.push_back(transitionIndex +
                           " is ignored because type is not \"event\"");
        continue;
      }

      if (!transitionNode["event"]) {
        warnings.push_back(transitionIndex +
                           " is ignored because `event` does not exist");
        continue;
      }

      if (transitionNode["event"].as<String>("") == "") {
        warnings.push_back(transitionIndex +
                           " is ignored because `event` is empty");
        continue;
      }

      if (!transitionNode["target"] ||
          transitionNode["target"].as<String>("") == "") {
        warnings.push_back(transitionIndex +
                           " is ignored because `target` is empty");
        continue;
      }

      auto target = transitionNode["target"].as<String>("");
      size_t i = 0;
      for (; i < asset.data.states.size() &&
             asset.data.states.at(i).name != target;
           ++i) {
      }

      if (i < asset.data.states.size()) {
        AnimationStateTransition transition{};
        transition.eventName = transitionNode["event"].as<String>("");
        transition.target = i;
        state.transitions.push_back(transition);
      } else {
        warnings.push_back(transitionIndex + " is ignored because \"" + target +
                           "\" state does not exist");
      }
    }
  }

  if (asset.data.states.empty()) {
    AnimationState dummyState{};
    dummyState.name = "INITIAL";
    asset.data.states.push_back(dummyState);
    warnings.push_back(
        "Dummy state added because no valid states in the state machine");
  }

  if (root["initial"]) {
    auto initial = root["initial"].as<String>("");
    size_t i = 0;
    for (; i < asset.data.states.size() &&
           asset.data.states.at(i).name != initial;
         ++i) {
    }

    if (i < asset.data.states.size()) {
      asset.data.initialState = i;
    } else {
      asset.data.initialState = 0;
      warnings.push_back(
          "Initial state is set to first item because it was invalid");
    }
  }

  if (handle == AnimatorAssetHandle::Null) {
    auto newHandle = mRegistry.getAnimators().addAsset(asset);
    return Result<AnimatorAssetHandle>::Ok(newHandle, warnings);
  }

  mRegistry.getAnimators().updateAsset(handle, asset);

  return Result<AnimatorAssetHandle>::Ok(handle, warnings);
}

} // namespace liquid
