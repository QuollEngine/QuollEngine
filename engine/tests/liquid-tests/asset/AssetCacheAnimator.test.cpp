#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"
#include "liquid/yaml/Yaml.h"

#include "liquid-tests/Testing.h"

class AssetCacheTest : public ::testing::Test {
public:
  AssetCacheTest() : cache(FixturesPath) {}

  liquid::AssetCache cache;
};

using AssetCacheDeathTest = AssetCacheTest;

TEST_F(AssetCacheTest, CreatesAnimatorFileFromAsset) {
  liquid::AssetData<liquid::AnimationAsset> animData{};
  animData.path = cache.getAssetsPath() / "idle.lqanim";
  auto idle = cache.getRegistry().getAnimations().addAsset(animData);

  animData.path = cache.getAssetsPath() / "walk.lqanim";
  auto walk = cache.getRegistry().getAnimations().addAsset(animData);

  animData.path = cache.getAssetsPath() / "run.lqanim";
  auto run = cache.getRegistry().getAnimations().addAsset(animData);

  liquid::AssetData<liquid::AnimatorAsset> asset{};
  asset.name = "test.animator";
  asset.relativePath = "test.animator";
  asset.data.initialState = 1;

  liquid::AnimationState stateIdle;
  stateIdle.name = "idle";
  stateIdle.animation = idle;
  stateIdle.transitions.push_back({"WALK", 1});
  stateIdle.transitions.push_back({"RUN", 2});

  liquid::AnimationState stateWalk;
  stateWalk.name = "walk";
  stateWalk.animation = walk;
  stateWalk.transitions.push_back({"IDLE", 0});
  stateWalk.transitions.push_back({"RUN", 2});

  liquid::AnimationState stateRun;
  stateRun.name = "run";
  stateRun.animation = run;
  stateRun.transitions.push_back({"IDLE", 0});
  stateRun.transitions.push_back({"WALK", 1});

  asset.data.states.push_back(stateIdle);
  asset.data.states.push_back(stateWalk);
  asset.data.states.push_back(stateRun);

  auto res = cache.createAnimatorFromAsset(asset);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());

  auto path = res.getData();

  std::ifstream stream(path);
  auto node = YAML::Load(stream);
  stream.close();

  EXPECT_EQ(node["version"].as<liquid::String>(""), "0.1");
  EXPECT_EQ(node["type"].as<liquid::String>(""), "animator");
  EXPECT_EQ(node["initial"].as<liquid::String>(""), "walk");
  EXPECT_TRUE(node["states"].IsMap());

  // idle
  {
    auto state = node["states"]["idle"];

    EXPECT_TRUE(state.IsMap());

    auto output = state["output"];
    EXPECT_TRUE(output.IsMap());
    EXPECT_EQ(output["type"].as<liquid::String>(""), "animation");
    EXPECT_EQ(output["animation"].as<liquid::String>(""), "idle.lqanim");

    auto on = state["on"];
    EXPECT_TRUE(on.IsSequence());
    EXPECT_EQ(on.size(), 2);

    auto t1 = on[0];
    EXPECT_TRUE(t1.IsMap());
    EXPECT_EQ(t1["type"].as<liquid::String>(""), "event");
    EXPECT_EQ(t1["event"].as<liquid::String>(""), "WALK");
    EXPECT_EQ(t1["target"].as<liquid::String>(""), "walk");

    auto t2 = on[1];
    EXPECT_TRUE(t2.IsMap());
    EXPECT_EQ(t2["type"].as<liquid::String>(""), "event");
    EXPECT_EQ(t2["event"].as<liquid::String>(""), "RUN");
    EXPECT_EQ(t2["target"].as<liquid::String>(""), "run");
  }

  // walk
  {
    auto state = node["states"]["walk"];

    EXPECT_TRUE(state.IsMap());

    auto output = state["output"];
    EXPECT_TRUE(output.IsMap());
    EXPECT_EQ(output["type"].as<liquid::String>(""), "animation");
    EXPECT_EQ(output["animation"].as<liquid::String>(""), "walk.lqanim");

    auto on = state["on"];
    EXPECT_TRUE(on.IsSequence());
    EXPECT_EQ(on.size(), 2);

    auto t1 = on[0];
    EXPECT_TRUE(t1.IsMap());
    EXPECT_EQ(t1["type"].as<liquid::String>(""), "event");
    EXPECT_EQ(t1["event"].as<liquid::String>(""), "IDLE");
    EXPECT_EQ(t1["target"].as<liquid::String>(""), "idle");

    auto t2 = on[1];
    EXPECT_TRUE(t2.IsMap());
    EXPECT_EQ(t2["type"].as<liquid::String>(""), "event");
    EXPECT_EQ(t2["event"].as<liquid::String>(""), "RUN");
    EXPECT_EQ(t2["target"].as<liquid::String>(""), "run");
  }

  // run
  {
    auto state = node["states"]["run"];

    EXPECT_TRUE(state.IsMap());

    auto output = state["output"];
    EXPECT_TRUE(output.IsMap());
    EXPECT_EQ(output["type"].as<liquid::String>(""), "animation");
    EXPECT_EQ(output["animation"].as<liquid::String>(""), "run.lqanim");

    auto on = state["on"];
    EXPECT_TRUE(on.IsSequence());
    EXPECT_EQ(on.size(), 2);

    auto t1 = on[0];
    EXPECT_TRUE(t1.IsMap());
    EXPECT_EQ(t1["type"].as<liquid::String>(""), "event");
    EXPECT_EQ(t1["event"].as<liquid::String>(""), "IDLE");
    EXPECT_EQ(t1["target"].as<liquid::String>(""), "idle");

    auto t2 = on[1];
    EXPECT_TRUE(t2.IsMap());
    EXPECT_EQ(t2["type"].as<liquid::String>(""), "event");
    EXPECT_EQ(t2["event"].as<liquid::String>(""), "WALK");
    EXPECT_EQ(t2["target"].as<liquid::String>(""), "walk");
  }
}

TEST_F(AssetCacheTest, LoadAnimatorFailsIfRequiredPropertiesAreInvalid) {
  static const liquid::Path FilePath =
      std::filesystem::current_path() / "test.animator";

  {
    YAML::Node node;
    node["version"] = "0.2";
    node["type"] = "animator";
    node["states"] = YAML::Node(YAML::NodeType::Sequence);
    std::ofstream stream(FilePath);
    stream << node;
    stream.close();

    auto res = cache.loadAnimatorFromFile(FilePath);
    EXPECT_FALSE(res.hasData());
    EXPECT_TRUE(res.hasError());
    EXPECT_FALSE(res.hasWarnings());
  }

  {
    YAML::Node node;
    node["version"] = "0.1";
    node["type"] = "not-animator";
    node["states"] = YAML::Node(YAML::NodeType::Sequence);

    std::ofstream stream(FilePath);
    stream << node;
    stream.close();

    auto res = cache.loadAnimatorFromFile(FilePath);
    EXPECT_FALSE(res.hasData());
    EXPECT_TRUE(res.hasError());
    EXPECT_FALSE(res.hasWarnings());
  }

  {
    YAML::Node node;
    node["version"] = "0.1";
    node["type"] = "animator";
    node["states"] = "test";

    std::ofstream stream(FilePath);
    stream << node;
    stream.close();

    auto res = cache.loadAnimatorFromFile(FilePath);
    EXPECT_FALSE(res.hasData());
    EXPECT_TRUE(res.hasError());
    EXPECT_FALSE(res.hasWarnings());
  }
}

TEST_F(AssetCacheTest, LoadAnimatorIgnoresStatesThatHaveInvalidData) {
  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Null),
      YAML::Node(YAML::NodeType::Scalar),
      YAML::Node(YAML::NodeType::Sequence),
  };

  std::vector<liquid::String> invalidStateNames{"invalid0", "invalid1",
                                                "invalid2"};

  static const liquid::Path FilePath =
      std::filesystem::current_path() / "test.animator";

  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "animator";
  for (size_t i = 0; i < invalidStateNames.size(); ++i) {
    node["states"][invalidStateNames[i]] = invalidNodes[i];
  }
  node["states"]["valid"] = YAML::Node(YAML::NodeType::Map);

  std::ofstream stream(FilePath);
  stream << node;
  stream.close();

  auto res = cache.loadAnimatorFromFile(FilePath);
  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_TRUE(res.hasWarnings());

  EXPECT_EQ(res.getWarnings().size(), invalidStateNames.size());

  auto handle = res.getData();
  EXPECT_TRUE(cache.getRegistry().getAnimators().hasAsset(handle));
  const auto &animator = cache.getRegistry().getAnimators().getAsset(handle);

  EXPECT_EQ(animator.data.states.size(), 1);
  EXPECT_EQ(animator.data.states.at(0).name, "valid");
  EXPECT_EQ(animator.data.states.at(0).animation,
            liquid::AnimationAssetHandle::Invalid);
  EXPECT_EQ(animator.data.states.at(0).transitions.size(), 0);
}

TEST_F(AssetCacheTest, LoadAnimatorAddsDummyStateIfNoValidState) {
  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Null),
      YAML::Node(YAML::NodeType::Scalar),
      YAML::Node(YAML::NodeType::Sequence),
  };

  std::vector<liquid::String> invalidStateNames{"invalid0", "invalid1",
                                                "invalid2"};

  static const liquid::Path FilePath =
      std::filesystem::current_path() / "test.animator";

  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "animator";
  node["initial"] = "invalid1";
  for (size_t i = 0; i < invalidStateNames.size(); ++i) {
    node["states"][invalidStateNames[i]] = invalidNodes[i];
  }

  std::ofstream stream(FilePath);
  stream << node;
  stream.close();

  auto res = cache.loadAnimatorFromFile(FilePath);
  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_TRUE(res.hasWarnings());

  EXPECT_EQ(res.getWarnings().size(), invalidStateNames.size() + 2);

  auto handle = res.getData();
  EXPECT_TRUE(cache.getRegistry().getAnimators().hasAsset(handle));
  const auto &animator = cache.getRegistry().getAnimators().getAsset(handle);

  EXPECT_EQ(animator.data.initialState, 0);
  EXPECT_EQ(animator.data.states.size(), 1);
  EXPECT_EQ(animator.data.states.at(0).name, "INITIAL");
  EXPECT_EQ(animator.data.states.at(0).animation,
            liquid::AnimationAssetHandle::Invalid);
  EXPECT_EQ(animator.data.states.at(0).transitions.size(), 0);
}

TEST_F(AssetCacheTest,
       LoadAnimatorSetsFirstItemAsInitialStateIfInitialStateIsInvalid) {
  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Null), YAML::Node(YAML::NodeType::Scalar),
      YAML::Node(YAML::NodeType::Sequence), YAML::Node("test")};

  static const liquid::Path FilePath =
      std::filesystem::current_path() / "test.animator";

  for (auto invalidNode : invalidNodes) {
    YAML::Node node;
    node["version"] = "0.1";
    node["type"] = "animator";
    node["initial"] = invalidNode;
    node["states"]["valid"] = YAML::Node(YAML::NodeType::Map);

    std::ofstream stream(FilePath);
    stream << node;
    stream.close();

    auto res = cache.loadAnimatorFromFile(FilePath);
    EXPECT_TRUE(res.hasData());
    EXPECT_FALSE(res.hasError());
    EXPECT_TRUE(res.hasWarnings());
    EXPECT_EQ(res.getWarnings().size(), 1);

    auto handle = res.getData();
    EXPECT_TRUE(cache.getRegistry().getAnimators().hasAsset(handle));
    const auto &animator = cache.getRegistry().getAnimators().getAsset(handle);

    EXPECT_EQ(animator.data.initialState, 0);
    EXPECT_EQ(animator.data.states.size(), 1);
  }
}

TEST_F(AssetCacheTest, LoadAnimatorSetsInitialState) {
  static const liquid::Path FilePath =
      std::filesystem::current_path() / "test.animator";

  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "animator";
  node["initial"] = "valid2";
  node["states"]["valid1"] = YAML::Node(YAML::NodeType::Map);
  node["states"]["valid2"] = YAML::Node(YAML::NodeType::Map);

  std::ofstream stream(FilePath);
  stream << node;
  stream.close();

  auto res = cache.loadAnimatorFromFile(FilePath);
  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());

  auto handle = res.getData();
  EXPECT_TRUE(cache.getRegistry().getAnimators().hasAsset(handle));
  const auto &animator = cache.getRegistry().getAnimators().getAsset(handle);

  EXPECT_EQ(animator.data.initialState, 1);
  EXPECT_EQ(animator.data.states.size(), 2);
}

TEST_F(AssetCacheTest, LoadAnimatorIgnoresInvalidTransitions) {
  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Null),
      YAML::Node(YAML::NodeType::Scalar),
      YAML::Node(YAML::NodeType::Sequence),
  };

  static const liquid::Path FilePath =
      std::filesystem::current_path() / "test.animator";

  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "animator";

  auto state = node["states"]["idle"];
  for (auto invalidNode : invalidNodes) {
    state["on"].push_back(invalidNode);
  }

  YAML::Node transitionWithoutType;
  transitionWithoutType["event"] = "NEW_EVENT";
  transitionWithoutType["target"] = "walk";
  state["on"].push_back(transitionWithoutType);

  YAML::Node transitionWithNonEventType;
  transitionWithNonEventType["type"] = "test";
  transitionWithNonEventType["event"] = "NEW_EVENT";
  transitionWithNonEventType["target"] = "walk";
  state["on"].push_back(transitionWithNonEventType);

  YAML::Node transitionWithoutEventName;
  transitionWithoutEventName["type"] = "event";
  transitionWithoutEventName["target"] = "walk";
  state["on"].push_back(transitionWithoutEventName);

  YAML::Node transitionWithEmptyEventName;
  transitionWithEmptyEventName["type"] = "event";
  transitionWithEmptyEventName["event"] = "";
  transitionWithEmptyEventName["target"] = "walk";
  state["on"].push_back(transitionWithEmptyEventName);

  YAML::Node transitionWithoutTarget;
  transitionWithoutTarget["type"] = "event";
  transitionWithoutTarget["event"] = "NEW_EVENT";
  state["on"].push_back(transitionWithoutTarget);

  YAML::Node transitionWithUnknownTarget;
  transitionWithUnknownTarget["type"] = "event";
  transitionWithUnknownTarget["event"] = "NEW_EVENT";
  transitionWithUnknownTarget["target"] = "non-existent-target";
  state["on"].push_back(transitionWithUnknownTarget);

  YAML::Node validTransition;
  validTransition["type"] = "event";
  validTransition["event"] = "NEW_EVENT";
  validTransition["target"] = "walk";
  state["on"].push_back(validTransition);

  node["states"]["walk"] = YAML::Node(YAML::NodeType::Map);

  std::ofstream stream(FilePath);
  stream << node;
  stream.close();

  auto res = cache.loadAnimatorFromFile(FilePath);
  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_TRUE(res.hasWarnings());

  EXPECT_EQ(res.getWarnings().size(), 9);

  auto handle = res.getData();
  EXPECT_TRUE(cache.getRegistry().getAnimators().hasAsset(handle));
  const auto &animator = cache.getRegistry().getAnimators().getAsset(handle);

  EXPECT_EQ(animator.data.states.size(), 2);
  EXPECT_EQ(animator.data.states.at(0).name, "idle");
  EXPECT_EQ(animator.data.states.at(0).animation,
            liquid::AnimationAssetHandle::Invalid);
  EXPECT_EQ(animator.data.states.at(0).transitions.size(), 1);
  EXPECT_EQ(animator.data.states.at(0).transitions.at(0).eventName,
            "NEW_EVENT");
  EXPECT_EQ(animator.data.states.at(0).transitions.at(0).target, 1);
}

TEST_F(AssetCacheTest, LoadsAnimatorWithAlreadyLoadedAnimations) {
  static const liquid::Path FilePath =
      std::filesystem::current_path() / "test.animator";

  liquid::AssetData<liquid::AnimationAsset> animData{};
  animData.name = "my-animation.lqanim";
  animData.relativePath = "my-animation.lqanim";
  animData.path = cache.getAssetsPath() / "my-animation.lqanim";

  auto animationHandle = cache.getRegistry().getAnimations().addAsset(animData);

  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "animator";

  auto state = node["states"]["idle"];
  state["output"]["type"] = "animation";
  state["output"]["animation"] = "my-animation.lqanim";

  std::ofstream stream(FilePath);
  stream << node;
  stream.close();

  auto res = cache.loadAnimatorFromFile(FilePath);
  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());

  auto handle = res.getData();
  EXPECT_TRUE(cache.getRegistry().getAnimators().hasAsset(handle));
  const auto &animator = cache.getRegistry().getAnimators().getAsset(handle);

  EXPECT_EQ(animator.data.states.size(), 1);
  EXPECT_EQ(animator.data.states.at(0).name, "idle");
  EXPECT_EQ(animator.data.states.at(0).animation, animationHandle);
}

TEST_F(AssetCacheTest, LoadAnimatorLoadsAnimationsBeforeLoadingAnimator) {
  static const liquid::Path FilePath =
      std::filesystem::current_path() / "test.animator";

  liquid::AssetData<liquid::AnimationAsset> animData{};
  animData.name = "my-animation";

  auto path = cache.createAnimationFromAsset(animData);

  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "animator";

  auto state = node["states"]["idle"];
  state["output"]["type"] = "animation";
  state["output"]["animation"] = "my-animation.lqanim";

  std::ofstream stream(FilePath);
  stream << node;
  stream.close();

  auto res = cache.loadAnimatorFromFile(FilePath);
  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());

  auto handle = res.getData();
  EXPECT_TRUE(cache.getRegistry().getAnimators().hasAsset(handle));
  const auto &animator = cache.getRegistry().getAnimators().getAsset(handle);

  EXPECT_EQ(animator.data.states.size(), 1);
  EXPECT_EQ(animator.data.states.at(0).name, "idle");
  EXPECT_NE(animator.data.states.at(0).animation,
            liquid::AnimationAssetHandle{0});
}

TEST_F(AssetCacheTest, UpdatesExistingAnimatorIfHandleExists) {
  liquid::AssetData<liquid::AnimatorAsset> animData{};
  animData.relativePath = "my-animator.animator";
  animData.data.states.push_back({});
  auto animatorPath = cache.createAnimatorFromAsset(animData);

  auto result = cache.loadAnimatorFromFile(animatorPath.getData());
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result.hasData());

  auto handle = result.getData();

  animData.relativePath = "my-animator-2.animator";
  auto animatorPath2 = cache.createAnimatorFromAsset(animData);

  // Load script to update the handle
  auto res2 = cache.loadAnimatorFromFile(animatorPath2.getData(), handle);
  EXPECT_EQ(res2.getData(), handle);

  auto &animator = cache.getRegistry().getAnimators().getAsset(handle);
  EXPECT_EQ(animator.relativePath, "my-animator-2.animator");
  EXPECT_EQ(animator.path, cache.getAssetsPath() / animator.relativePath);
  EXPECT_EQ(animator.type, liquid::AssetType::Animator);
}

TEST_F(AssetCacheDeathTest, UpdateAnimatorFailsIfProvidedHandleDoesNotExist) {
  liquid::AssetData<liquid::AnimatorAsset> animData{};
  animData.relativePath = "my-animator.animator";
  animData.data.states.push_back({});
  auto animatorPath = cache.createAnimatorFromAsset(animData);

  EXPECT_DEATH(cache.loadAnimatorFromFile(animatorPath.getData(),
                                          liquid::AnimatorAssetHandle{25}),
               ".*");
}