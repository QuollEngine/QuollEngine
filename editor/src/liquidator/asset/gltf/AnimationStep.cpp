#include "liquid/core/Base.h"
// TODO: Remove engine logger
#include "liquid/core/Engine.h"

#include "AnimationStep.h"
#include "Buffer.h"

namespace liquidator {

void loadAnimations(GLTFImportData &importData) {
  auto &assetCache = importData.assetCache;
  const auto &targetPath = importData.targetPath;
  const auto &model = importData.model;

  for (size_t i = 0; i < model.animations.size(); ++i) {
    const auto &gltfAnimation = model.animations.at(i);

    struct SamplerInfo {
      std::vector<float> times;
      std::vector<glm::vec4> values;
      liquid::KeyframeSequenceAssetInterpolation interpolation =
          liquid::KeyframeSequenceAssetInterpolation::Linear;
    };

    std::vector<SamplerInfo> samplers(gltfAnimation.samplers.size());

    float maxTime = 0.0f;

    for (size_t i = 0; i < gltfAnimation.samplers.size(); ++i) {
      const auto &sampler = gltfAnimation.samplers.at(i);
      const auto &input = getBufferMetaForAccessor(model, sampler.input);
      const auto &output = getBufferMetaForAccessor(model, sampler.output);

      if (input.accessor.type != TINYGLTF_TYPE_SCALAR) {
        liquid::Engine::getLogger().warning()
            << "Animation time accessor must be in SCALAR format. Skipping...";
        continue;
      }

      if (input.accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
        liquid::Engine::getLogger().warning()
            << "Animation time accessor component type must be FLOAT";
        continue;
      }

      if (input.accessor.count != output.accessor.count) {
        liquid::Engine::getLogger().warning()
            << "Sampler input and output must have the same number of items";
        continue;
      }

      if (output.accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
        liquid::Engine::getLogger().warning()
            << "Animation output accessor component type must be FLOAT";
        continue;
      }

      std::vector<float> &times = samplers.at(i).times;
      times.resize(input.accessor.count);

      std::vector<glm::vec4> &values = samplers.at(i).values;
      values.resize(output.accessor.count);

      if (sampler.interpolation == "LINEAR") {
        samplers.at(i).interpolation =
            liquid::KeyframeSequenceAssetInterpolation::Linear;
      } else if (sampler.interpolation == "STEP") {
        samplers.at(i).interpolation =
            liquid::KeyframeSequenceAssetInterpolation::Step;
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

    liquid::AssetData<liquid::AnimationAsset> animation;
    animation.name = targetPath.string() + "/" + gltfAnimation.name;
    animation.data.time = maxTime;

    int32_t targetNode = -1;
    int32_t targetSkin = -1;

    for (const auto &channel : gltfAnimation.channels) {
      const auto &sampler = samplers.at(channel.sampler);

      if (channel.target_node == -1) {
        // Ignore channel if target node is not specified
        continue;
      }

      auto target = liquid::KeyframeSequenceAssetTarget::Position;
      if (channel.target_path == "rotation") {
        target = liquid::KeyframeSequenceAssetTarget::Rotation;
      } else if (channel.target_path == "scale") {
        target = liquid::KeyframeSequenceAssetTarget::Scale;
      } else if (channel.target_path == "position") {
        target = liquid::KeyframeSequenceAssetTarget::Position;
      }

      uint32_t targetJoint = 0;

      auto it = importData.skeletons.jointSkinMap.find(channel.target_node);
      bool skinFound = it != importData.skeletons.jointSkinMap.end();
      if (targetSkin == -1 && skinFound) {
        targetSkin = static_cast<int32_t>(it->second);
        targetJoint = importData.skeletons.gltfToNormalizedJointMap.at(
            channel.target_node);
      } else if (skinFound) {
        LIQUID_ASSERT(
            it->second == targetSkin,
            "All channels in animation must point to the same target skin");
        targetJoint = importData.skeletons.gltfToNormalizedJointMap.at(
            channel.target_node);
      }

      if (targetSkin == -1 && targetNode == -1) {
        targetNode = channel.target_node;
      } else {
        LIQUID_ASSERT(targetNode == -1, "All channels in animation must either "
                                        "animate skin or node, not both");
        LIQUID_ASSERT(
            targetNode == -1 || targetNode == channel.target_node,
            "All channels in animation must point to the same target node");
      }

      LIQUID_ASSERT(targetSkin == -1 || targetNode == -1,
                    "A channel must point to a node or a skin");

      liquid::KeyframeSequenceAsset sequence;
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

    LIQUID_ASSERT(targetNode >= 0 || targetSkin >= 0,
                  "Animation must have a target node or skin");
    auto filePath = assetCache.createAnimationFromAsset(animation);
    auto handle = assetCache.loadAnimationFromFile(filePath.getData());

    if (targetSkin >= 0) {
      if (importData.animations.skinAnimationMap.find(targetSkin) ==
          importData.animations.skinAnimationMap.end()) {
        importData.animations.skinAnimationMap.insert(
            {static_cast<uint32_t>(targetSkin), {}});
      }
      importData.animations.skinAnimationMap.at(targetSkin)
          .push_back(handle.getData());
    } else {
      if (importData.animations.nodeAnimationMap.find(targetSkin) ==
          importData.animations.nodeAnimationMap.end()) {
        importData.animations.nodeAnimationMap.insert(
            {static_cast<uint32_t>(targetNode), {}});
      }

      importData.animations.nodeAnimationMap.at(targetNode)
          .push_back(handle.getData());
    }
  }
}
} // namespace liquidator
