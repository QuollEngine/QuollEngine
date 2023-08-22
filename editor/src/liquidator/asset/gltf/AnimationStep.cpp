#include "liquid/core/Base.h"

#include "AnimationStep.h"
#include "Buffer.h"

namespace liquid::editor {

void loadAnimations(GLTFImportData &importData) {
  auto &assetCache = importData.assetCache;
  const auto &model = importData.model;

  std::map<uint32_t, std::vector<std::pair<AnimationAssetHandle, String>>>
      nodeAnimationMap;
  std::map<uint32_t, std::vector<std::pair<AnimationAssetHandle, String>>>
      skinAnimationMap;

  for (size_t i = 0; i < model.animations.size(); ++i) {
    const auto &gltfAnimation = model.animations.at(i);

    auto assetName = gltfAnimation.name.empty()
                         ? "animation" + std::to_string(i)
                         : gltfAnimation.name;
    assetName += ".anim";

    struct SamplerInfo {
      std::vector<float> times;
      std::vector<glm::vec4> values;
      KeyframeSequenceAssetInterpolation interpolation =
          KeyframeSequenceAssetInterpolation::Linear;
    };

    std::vector<SamplerInfo> samplers(gltfAnimation.samplers.size());

    float maxTime = 0.0f;
    bool animationValid = true;

    for (size_t i = 0; i < gltfAnimation.samplers.size() && animationValid;
         ++i) {
      const auto &sampler = gltfAnimation.samplers.at(i);
      const auto &input = getBufferMetaForAccessor(model, sampler.input);
      const auto &output = getBufferMetaForAccessor(model, sampler.output);

      if (input.accessor.type != TINYGLTF_TYPE_SCALAR) {
        importData.warnings.push_back(assetName +
                                      " skipped because it has invalid data");
        animationValid = false;
        continue;
      }

      if (input.accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
        importData.warnings.push_back(assetName +
                                      " skipped because it has invalid data");
        animationValid = false;
        continue;
      }

      if (input.accessor.count != output.accessor.count) {
        importData.warnings.push_back(assetName +
                                      " skipped because it has invalid data");
        animationValid = true;
        continue;
      }

      if (output.accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
        importData.warnings.push_back(assetName +
                                      " skipped because it has invalid data");
        animationValid = true;
        continue;
      }

      std::vector<float> &times = samplers.at(i).times;
      times.resize(input.accessor.count);

      std::vector<glm::vec4> &values = samplers.at(i).values;
      values.resize(output.accessor.count);

      if (sampler.interpolation == "LINEAR") {
        samplers.at(i).interpolation =
            KeyframeSequenceAssetInterpolation::Linear;
      } else if (sampler.interpolation == "STEP") {
        samplers.at(i).interpolation = KeyframeSequenceAssetInterpolation::Step;
      }

      float max = 0.0f;

      {
        const float *inputData = reinterpret_cast<const float *>(input.rawData);
        for (size_t i = 0; i < input.accessor.count; ++i) {
          times.at(i) = inputData[i];
          max = std::max(max, inputData[i]);
        }
      }

      maxTime = std::max(max, maxTime);

      // Normalize the time
      {
        for (size_t i = 0; i < times.size(); ++i) {
          times.at(i) = times.at(i) / max;
        }
      }

      if (output.accessor.type == TINYGLTF_TYPE_VEC3) {
        const glm::vec3 *outputData =
            reinterpret_cast<const glm::vec3 *>(output.rawData);
        for (size_t i = 0; i < output.accessor.count; ++i) {
          values.at(i) = glm::vec4(outputData[i], 0.0f);
        }
      } else if (output.accessor.type == TINYGLTF_TYPE_VEC4) {
        const glm::vec4 *outputData =
            reinterpret_cast<const glm::vec4 *>(output.rawData);
        for (size_t i = 0; i < output.accessor.count; ++i) {
          values.at(i) = outputData[i];
        }
      } else if (output.accessor.type == TINYGLTF_TYPE_SCALAR) {
        const float *outputData =
            reinterpret_cast<const float *>(output.rawData);
        for (size_t i = 0; i < output.accessor.count; ++i) {
          values.at(i) = glm::vec4(outputData[i], 0, 0, 0);
        }
      }
    }

    AssetData<AnimationAsset> animation;
    animation.name = getGLTFAssetName(importData, assetName);
    animation.data.time = maxTime;

    int32_t targetNode = -1;
    int32_t targetSkin = -1;

    for (size_t ci = 0; ci < gltfAnimation.channels.size() && animationValid;
         ++ci) {
      const auto &channel = gltfAnimation.channels.at(ci);
      const auto &sampler = samplers.at(channel.sampler);

      if (channel.target_node == -1) {
        // Ignore channel if target node is not specified
        continue;
      }

      auto target = KeyframeSequenceAssetTarget::Position;
      if (channel.target_path == "rotation") {
        target = KeyframeSequenceAssetTarget::Rotation;
      } else if (channel.target_path == "scale") {
        target = KeyframeSequenceAssetTarget::Scale;
      } else if (channel.target_path == "position") {
        target = KeyframeSequenceAssetTarget::Position;
      }

      uint32_t targetJoint = 0;

      auto it = importData.skeletons.jointSkinMap.find(channel.target_node);
      bool skinFound = it != importData.skeletons.jointSkinMap.end();
      if (targetSkin == -1 && skinFound) {
        targetSkin = static_cast<int32_t>(it->second);
        targetJoint = importData.skeletons.gltfToNormalizedJointMap.at(
            channel.target_node);
      } else if (skinFound) {
        animationValid = it->second == targetSkin;
        if (!animationValid) {
          importData.warnings.push_back(
              "Animation #" + std::to_string(i) +
              " skipped because all channels of animation must point to "
              "the same skin");
          continue;
        }

        targetJoint = importData.skeletons.gltfToNormalizedJointMap.at(
            channel.target_node);
      }

      if (targetSkin == -1 && targetNode == -1) {
        targetNode = channel.target_node;
      } else if (targetNode != -1 && targetSkin != -1) {
        importData.warnings.push_back(
            assetName +
            " skipped because a channels points to both skin and node");
        animationValid = false;
        continue;
      } else if (targetNode != -1 && targetNode != channel.target_node) {
        importData.warnings.push_back(
            assetName +
            " skipped because a channel points to a different target node");
        animationValid = false;
        continue;
      }

      KeyframeSequenceAsset sequence;
      sequence.interpolation = sampler.interpolation;
      sequence.target = target;

      if (targetSkin >= 0) {
        sequence.joint = targetJoint;
        sequence.jointTarget = true;
      }

      for (size_t i = 0; i < sampler.times.size(); ++i) {
        sequence.keyframeTimes.push_back(sampler.times.at(i));
        sequence.keyframeValues.push_back(sampler.values.at(i));
      }
      animation.data.keyframes.push_back(sequence);
    }

    if (targetNode == -1 && targetSkin == -1) {
      importData.warnings.push_back(
          assetName +
          " skipped because it does not have a target node or skin");
      animationValid = false;
      continue;
    }

    auto filePath = assetCache.createAnimationFromAsset(
        animation, getUUID(importData, assetName));
    auto handle = assetCache.loadAnimationFromFile(filePath.getData());
    importData.outputUuids.insert_or_assign(assetName,
                                            assetCache.getRegistry()
                                                .getAnimations()
                                                .getAsset(handle.getData())
                                                .uuid);

    if (!animationValid) {
      continue;
    }

    if (targetSkin >= 0) {
      if (skinAnimationMap.find(targetSkin) == skinAnimationMap.end()) {
        skinAnimationMap.insert({static_cast<uint32_t>(targetSkin), {}});
      }
      skinAnimationMap.at(targetSkin).push_back({handle.getData(), assetName});
    } else {
      if (nodeAnimationMap.find(targetSkin) == nodeAnimationMap.end()) {
        nodeAnimationMap.insert({static_cast<uint32_t>(targetNode), {}});
      }
      nodeAnimationMap.at(targetNode).push_back({handle.getData(), assetName});
    }
  }

  for (auto &[skin, animations] : skinAnimationMap) {
    auto animatorName = "animator-skin-" + std::to_string(skin);

    AssetData<AnimatorAsset> asset{};
    asset.name = asset.name = getGLTFAssetName(importData, animatorName);
    for (auto [handle, assetName] : animations) {
      AnimationState state{};
      state.name = assetName;

      state.animation = handle;
      asset.data.states.push_back(state);
    }

    for (size_t i = 0; i < asset.data.states.size(); ++i) {
      auto &state = asset.data.states.at(i);

      for (size_t j = 0; j < asset.data.states.size(); ++j) {
        if (i == j) {
          // Ignore transition to itself
          continue;
        }

        AnimationStateTransition transition{};
        transition.eventName = asset.data.states.at(j).name;
        transition.target = j;
        state.transitions.push_back(transition);
      }
    }

    auto path = assetCache.createAnimatorFromAsset(
        asset, getUUID(importData, animatorName));
    auto handle = assetCache.loadAnimatorFromFile(path.getData());
    importData.animations.skinAnimatorMap.insert_or_assign(skin,
                                                           handle.getData());

    importData.outputUuids.insert_or_assign(animatorName,
                                            assetCache.getRegistry()
                                                .getAnimators()
                                                .getAsset(handle.getData())
                                                .uuid);
  }

  for (auto &[node, animations] : nodeAnimationMap) {
    auto animatorName = "animator-node-" + std::to_string(node);

    AssetData<AnimatorAsset> asset{};
    asset.name = asset.name = getGLTFAssetName(importData, animatorName);

    for (auto [handle, assetName] : animations) {
      AnimationState state{};
      state.name = assetName;
      state.animation = handle;
      asset.data.states.push_back(state);
    }

    for (size_t i = 0; i < asset.data.states.size(); ++i) {
      auto &state = asset.data.states.at(i);

      for (size_t j = 0; j < asset.data.states.size(); ++j) {
        if (i == j) {
          // Ignore transition to itself
          continue;
        }

        AnimationStateTransition transition{};
        transition.eventName = asset.data.states.at(j).name;
        transition.target = j;
        state.transitions.push_back(transition);
      }
    }

    auto path = assetCache.createAnimatorFromAsset(
        asset, getUUID(importData, animatorName));
    auto handle = assetCache.loadAnimatorFromFile(path.getData());
    importData.animations.nodeAnimatorMap.insert_or_assign(node,
                                                           handle.getData());
    importData.outputUuids.insert_or_assign(animatorName,
                                            assetCache.getRegistry()
                                                .getAnimators()
                                                .getAsset(handle.getData())
                                                .uuid);
  }
}

} // namespace liquid::editor
