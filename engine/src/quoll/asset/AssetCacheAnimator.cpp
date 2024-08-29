#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/yaml/Yaml.h"
#include "AssetCache.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

static String serializeLoopMode(AnimationLoopMode loopMode) {
  switch (loopMode) {
  case AnimationLoopMode::Linear:
    return "linear";
  case AnimationLoopMode::None:
  default:
    return "none";
  }
}

static AnimationLoopMode deserializeLoopMode(String loopMode) {
  if (loopMode == "linear") {
    return AnimationLoopMode::Linear;
  }

  return AnimationLoopMode::None;
}

Result<void> AssetCache::createAnimatorFromData(const AnimatorAsset &data,
                                                const Path &assetPath) {
  YAML::Node root;
  root["version"] = "0.1";
  root["type"] = "animator";
  root["initial"] = data.states.at(data.initialState).name;

  auto statesNode = root["states"];

  for (const auto &state : data.states) {
    auto stateNode = statesNode[state.name];
    stateNode["output"]["type"] = "animation";
    stateNode["output"]["animation"] = getAssetUuid(state.animation);
    stateNode["output"]["speed"] = state.speed;
    stateNode["output"]["loopMode"] = serializeLoopMode(state.loopMode);

    for (const auto &transition : state.transitions) {
      YAML::Node transitionNode(YAML::NodeType::Map);
      transitionNode["type"] = "event";
      transitionNode["event"] = transition.eventName;
      transitionNode["target"] = data.states.at(transition.target).name;

      stateNode["on"].push_back(transitionNode);
    }
  }

  std::ofstream stream(assetPath);
  stream << root;
  stream.close();

  return Ok();
}

Result<AnimatorAsset> AssetCache::loadAnimator(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  std::ifstream stream(filePath);
  auto root = YAML::Load(stream);
  stream.close();

  if (root["type"].as<String>("") != "animator") {
    return Error("Type must be animator");
  }

  if (root["version"].as<String>("") != "0.1") {
    return Error("Version is not supported");
  }

  if (!root["states"] || !root["states"].IsMap()) {
    return Error("`states` field must be a map");
  }

  auto meta = getAssetMeta(uuid);

  AnimatorAsset asset{};

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
      auto animation = output["animation"].as<Uuid>(Uuid{});
      if (!animation.isEmpty()) {
        auto res = request<AnimationAsset>(animation);
        if (res) {
          state.animation = res.data().handle();
        }

        state.speed = std::max(output["speed"].as<f32>(1.0f), 0.0f);
        state.loopMode = deserializeLoopMode(output["loopMode"].as<String>(""));

        if (res.hasWarnings()) {
          warnings.insert(warnings.end(), res.warnings().begin(),
                          res.warnings().end());
        }

        if (!res) {
          warnings.push_back(res.error());
        }
      }
    }

    transitionNodes.push_back(stateNode["on"]);
    asset.states.push_back(state);
  }

  for (usize i = 0; i < transitionNodes.size(); ++i) {
    auto &state = asset.states.at(i);
    auto &stateOn = transitionNodes.at(i);
    if (!stateOn || !stateOn.IsSequence()) {
      continue;
    }

    for (usize j = 0; j < stateOn.size(); ++j) {
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
      usize i = 0;
      for (; i < asset.states.size() && asset.states.at(i).name != target;
           ++i) {
      }

      if (i < asset.states.size()) {
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

  if (asset.states.empty()) {
    AnimationState dummyState{};
    dummyState.name = "INITIAL";
    asset.states.push_back(dummyState);
    warnings.push_back(
        "Dummy state added because no valid states in the state machine");
  }

  if (root["initial"]) {
    auto initial = root["initial"].as<String>("");
    usize i = 0;
    for (; i < asset.states.size() && asset.states.at(i).name != initial; ++i) {
    }

    if (i < asset.states.size()) {
      asset.initialState = i;
    } else {
      asset.initialState = 0;
      warnings.push_back(
          "Initial state is set to first item because it was invalid");
    }
  }

  return {asset, warnings};
}

} // namespace quoll
