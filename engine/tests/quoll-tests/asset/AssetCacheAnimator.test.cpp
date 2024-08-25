#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/yaml/Yaml.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"
#include <random>

static const quoll::Path FilePath =
    std::filesystem::current_path() / "cache" / "test.animator";

class AssetCacheAnimatorTest : public AssetCacheTestBase {
public:
};

TEST_F(AssetCacheAnimatorTest, CreatesAnimatorFromSource) {
  auto uuid = quoll::Uuid::generate();
  auto filePath =
      cache.createAnimatorFromSource(FixturesPath / "test.animator", uuid);
  EXPECT_TRUE(filePath.hasData());
  EXPECT_FALSE(filePath.hasError());
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.getData().filename().string().size(), 38);

  auto meta = cache.getAssetMeta(uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::Animator);
  EXPECT_EQ(meta.name, "test.animator");
}

TEST_F(AssetCacheAnimatorTest, CreatesAnimatorFileFromAsset) {
  quoll::AssetData<quoll::AnimationAsset> animData{};
  animData.uuid = quoll::Uuid("idle");
  auto idle = cache.getRegistry().getAnimations().addAsset(animData);

  animData.uuid = quoll::Uuid("walk");
  auto walk = cache.getRegistry().getAnimations().addAsset(animData);

  animData.uuid = quoll::Uuid("run");
  auto run = cache.getRegistry().getAnimations().addAsset(animData);

  quoll::AssetData<quoll::AnimatorAsset> asset{};
  asset.data.initialState = 1;
  asset.name = "my-animator.animator";
  asset.uuid = quoll::Uuid::generate();

  quoll::AnimationState stateIdle;
  stateIdle.name = "idle";
  stateIdle.animation = idle;
  stateIdle.transitions.push_back({"WALK", 1});
  stateIdle.transitions.push_back({"RUN", 2});

  quoll::AnimationState stateWalk;
  stateWalk.name = "walk";
  stateWalk.animation = walk;
  stateWalk.transitions.push_back({"IDLE", 0});
  stateWalk.transitions.push_back({"RUN", 2});
  stateWalk.speed = 0.5f;
  stateWalk.loopMode = quoll::AnimationLoopMode::Linear;

  quoll::AnimationState stateRun;
  stateRun.name = "run";
  stateRun.animation = run;
  stateRun.transitions.push_back({"IDLE", 0});
  stateRun.transitions.push_back({"WALK", 1});

  asset.data.states.push_back(stateIdle);
  asset.data.states.push_back(stateWalk);
  asset.data.states.push_back(stateRun);

  auto filePath = cache.createAnimatorFromAsset(asset);

  EXPECT_TRUE(filePath.hasData());
  EXPECT_FALSE(filePath.hasError());
  EXPECT_FALSE(filePath.hasWarnings());

  auto meta = cache.getAssetMeta(asset.uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::Animator);
  EXPECT_EQ(meta.name, "my-animator.animator");

  auto path = filePath.getData();

  std::ifstream stream(path);
  auto node = YAML::Load(stream);
  stream.close();

  EXPECT_EQ(node["version"].as<quoll::String>(""), "0.1");
  EXPECT_EQ(node["type"].as<quoll::String>(""), "animator");
  EXPECT_EQ(node["initial"].as<quoll::String>(""), "walk");
  EXPECT_TRUE(node["states"].IsMap());

  // idle
  {
    auto state = node["states"]["idle"];

    EXPECT_TRUE(state.IsMap());

    auto output = state["output"];
    EXPECT_TRUE(output.IsMap());
    EXPECT_EQ(output["type"].as<quoll::String>(""), "animation");
    EXPECT_EQ(output["animation"].as<quoll::String>(""), "idle");
    EXPECT_EQ(output["speed"].as<f32>(-2.0f), 1.0f);
    EXPECT_EQ(output["loopMode"].as<quoll::String>(""), "none");

    auto on = state["on"];
    EXPECT_TRUE(on.IsSequence());
    EXPECT_EQ(on.size(), 2);

    auto t1 = on[0];
    EXPECT_TRUE(t1.IsMap());
    EXPECT_EQ(t1["type"].as<quoll::String>(""), "event");
    EXPECT_EQ(t1["event"].as<quoll::String>(""), "WALK");
    EXPECT_EQ(t1["target"].as<quoll::String>(""), "walk");

    auto t2 = on[1];
    EXPECT_TRUE(t2.IsMap());
    EXPECT_EQ(t2["type"].as<quoll::String>(""), "event");
    EXPECT_EQ(t2["event"].as<quoll::String>(""), "RUN");
    EXPECT_EQ(t2["target"].as<quoll::String>(""), "run");
  }

  // walk
  {
    auto state = node["states"]["walk"];

    EXPECT_TRUE(state.IsMap());

    auto output = state["output"];
    EXPECT_TRUE(output.IsMap());
    EXPECT_EQ(output["type"].as<quoll::String>(""), "animation");
    EXPECT_EQ(output["animation"].as<quoll::String>(""), "walk");
    EXPECT_EQ(output["speed"].as<f32>(-2.0f), 0.5f);
    EXPECT_EQ(output["loopMode"].as<quoll::String>(""), "linear");

    auto on = state["on"];
    EXPECT_TRUE(on.IsSequence());
    EXPECT_EQ(on.size(), 2);

    auto t1 = on[0];
    EXPECT_TRUE(t1.IsMap());
    EXPECT_EQ(t1["type"].as<quoll::String>(""), "event");
    EXPECT_EQ(t1["event"].as<quoll::String>(""), "IDLE");
    EXPECT_EQ(t1["target"].as<quoll::String>(""), "idle");

    auto t2 = on[1];
    EXPECT_TRUE(t2.IsMap());
    EXPECT_EQ(t2["type"].as<quoll::String>(""), "event");
    EXPECT_EQ(t2["event"].as<quoll::String>(""), "RUN");
    EXPECT_EQ(t2["target"].as<quoll::String>(""), "run");
  }

  // run
  {
    auto state = node["states"]["run"];

    EXPECT_TRUE(state.IsMap());

    auto output = state["output"];
    EXPECT_TRUE(output.IsMap());
    EXPECT_EQ(output["type"].as<quoll::String>(""), "animation");
    EXPECT_EQ(output["animation"].as<quoll::String>(""), "run");
    EXPECT_EQ(output["speed"].as<f32>(-2.0f), 1.0f);
    EXPECT_EQ(output["loopMode"].as<quoll::String>(""), "none");

    auto on = state["on"];
    EXPECT_TRUE(on.IsSequence());
    EXPECT_EQ(on.size(), 2);

    auto t1 = on[0];
    EXPECT_TRUE(t1.IsMap());
    EXPECT_EQ(t1["type"].as<quoll::String>(""), "event");
    EXPECT_EQ(t1["event"].as<quoll::String>(""), "IDLE");
    EXPECT_EQ(t1["target"].as<quoll::String>(""), "idle");

    auto t2 = on[1];
    EXPECT_TRUE(t2.IsMap());
    EXPECT_EQ(t2["type"].as<quoll::String>(""), "event");
    EXPECT_EQ(t2["event"].as<quoll::String>(""), "WALK");
    EXPECT_EQ(t2["target"].as<quoll::String>(""), "walk");
  }
}

TEST_F(AssetCacheAnimatorTest,
       LoadAnimatorFailsIfRequiredPropertiesAreInvalid) {
  {
    auto uuid = quoll::Uuid::generate();
    auto filePath = cache.getPathFromUuid(uuid);

    YAML::Node node;
    node["version"] = "0.2";
    node["type"] = "animator";
    node["states"] = YAML::Node(YAML::NodeType::Sequence);
    std::ofstream stream(filePath);
    stream << node;
    stream.close();

    auto res = cache.loadAnimator(uuid);
    EXPECT_FALSE(res.hasData());
    EXPECT_TRUE(res.hasError());
    EXPECT_FALSE(res.hasWarnings());
  }

  {
    auto uuid = quoll::Uuid::generate();
    auto filePath = cache.getPathFromUuid(uuid);

    YAML::Node node;
    node["version"] = "0.1";
    node["type"] = "not-animator";
    node["states"] = YAML::Node(YAML::NodeType::Sequence);

    std::ofstream stream(filePath);
    stream << node;
    stream.close();

    auto res = cache.loadAnimator(uuid);
    EXPECT_FALSE(res.hasData());
    EXPECT_TRUE(res.hasError());
    EXPECT_FALSE(res.hasWarnings());
  }

  {
    auto uuid = quoll::Uuid::generate();
    auto filePath = cache.getPathFromUuid(uuid);

    YAML::Node node;
    node["version"] = "0.1";
    node["type"] = "animator";
    node["states"] = "test";

    std::ofstream stream(filePath);
    stream << node;
    stream.close();

    auto res = cache.loadAnimator(uuid);
    EXPECT_FALSE(res.hasData());
    EXPECT_TRUE(res.hasError());
    EXPECT_FALSE(res.hasWarnings());
  }
}

TEST_F(AssetCacheAnimatorTest, LoadAnimatorIgnoresStatesThatHaveInvalidData) {
  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Null),
      YAML::Node(YAML::NodeType::Scalar),
      YAML::Node(YAML::NodeType::Sequence),
  };

  std::vector<quoll::String> invalidStateNames{"invalid0", "invalid1",
                                               "invalid2"};

  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "animator";
  for (usize i = 0; i < invalidStateNames.size(); ++i) {
    node["states"][invalidStateNames[i]] = invalidNodes[i];
  }
  node["states"]["valid"] = YAML::Node(YAML::NodeType::Map);

  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.getPathFromUuid(uuid);
  std::ofstream stream(filePath);
  stream << node;
  stream.close();

  auto res = cache.loadAnimator(uuid);
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
            quoll::AssetHandle<quoll::AnimationAsset>());
  EXPECT_EQ(animator.data.states.at(0).transitions.size(), 0);
}

TEST_F(AssetCacheAnimatorTest, LoadAnimatorAddsDummyStateIfNoValidState) {
  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Null),
      YAML::Node(YAML::NodeType::Scalar),
      YAML::Node(YAML::NodeType::Sequence),
  };

  std::vector<quoll::String> invalidStateNames{"invalid0", "invalid1",
                                               "invalid2"};

  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "animator";
  node["initial"] = "invalid1";
  for (usize i = 0; i < invalidStateNames.size(); ++i) {
    node["states"][invalidStateNames[i]] = invalidNodes[i];
  }

  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.getPathFromUuid(uuid);
  std::ofstream stream(filePath);
  stream << node;
  stream.close();

  auto res = cache.loadAnimator(uuid);
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
            quoll::AssetHandle<quoll::AnimationAsset>());
  EXPECT_EQ(animator.data.states.at(0).transitions.size(), 0);
}

TEST_F(AssetCacheAnimatorTest,
       LoadAnimatorSetsFirstItemAsInitialStateIfInitialStateIsInvalid) {
  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Null), YAML::Node(YAML::NodeType::Scalar),
      YAML::Node(YAML::NodeType::Sequence), YAML::Node("test")};

  for (auto invalidNode : invalidNodes) {
    YAML::Node node;
    node["version"] = "0.1";
    node["type"] = "animator";
    node["initial"] = invalidNode;
    node["states"]["valid"] = YAML::Node(YAML::NodeType::Map);

    auto uuid = quoll::Uuid::generate();
    auto filePath = cache.getPathFromUuid(uuid);
    std::ofstream stream(filePath);
    stream << node;
    stream.close();

    auto res = cache.loadAnimator(uuid);
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

TEST_F(AssetCacheAnimatorTest, LoadAnimatorSetsInitialState) {
  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "animator";
  node["initial"] = "valid2";
  node["states"]["valid1"] = YAML::Node(YAML::NodeType::Map);
  node["states"]["valid2"] = YAML::Node(YAML::NodeType::Map);

  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.getPathFromUuid(uuid);

  std::ofstream stream(filePath);
  stream << node;
  stream.close();

  auto res = cache.loadAnimator(uuid);
  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());

  auto handle = res.getData();
  EXPECT_TRUE(cache.getRegistry().getAnimators().hasAsset(handle));
  const auto &animator = cache.getRegistry().getAnimators().getAsset(handle);

  EXPECT_EQ(animator.data.initialState, 1);
  EXPECT_EQ(animator.data.states.size(), 2);
}

TEST_F(AssetCacheAnimatorTest,
       LoadAnimatorSetsDefaultValuesForInvalidSpeedAndTime) {
  std::vector<YAML::Node> invalidNodes{YAML::Node(YAML::NodeType::Null),
                                       YAML::Node(YAML::NodeType::Map),
                                       YAML::Node(YAML::NodeType::Scalar),
                                       YAML::Node(YAML::NodeType::Sequence),
                                       YAML::Node("Test"),
                                       YAML::Node(-1.0f)};

  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "animator";

  for (usize i = 0; i < invalidNodes.size(); ++i) {
    auto invalidNode = invalidNodes.at(i);
    auto state = "state_" + std::to_string(i);

    node["states"][state]["output"]["loopMode"] = invalidNode;
    node["states"][state]["output"]["speed"] = invalidNode;
  }

  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.getPathFromUuid(uuid);
  std::ofstream stream(filePath);
  stream << node;
  stream.close();

  auto res = cache.loadAnimator(uuid);
  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());

  auto handle = res.getData();
  EXPECT_TRUE(cache.getRegistry().getAnimators().hasAsset(handle));
  const auto &animator = cache.getRegistry().getAnimators().getAsset(handle);

  EXPECT_EQ(animator.data.states.size(), invalidNodes.size());
  for (auto state : animator.data.states) {
    EXPECT_EQ(state.loopMode, quoll::AnimationLoopMode::None);
    EXPECT_EQ(state.speed, 1.0f);
  }
}

TEST_F(AssetCacheAnimatorTest, LoadAnimatorIgnoresInvalidTransitions) {
  std::vector<YAML::Node> invalidNodes{
      YAML::Node(YAML::NodeType::Null),
      YAML::Node(YAML::NodeType::Scalar),
      YAML::Node(YAML::NodeType::Sequence),
  };

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

  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.getPathFromUuid(uuid);
  std::ofstream stream(filePath);
  stream << node;
  stream.close();

  auto res = cache.loadAnimator(uuid);
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
            quoll::AssetHandle<quoll::AnimationAsset>());
  EXPECT_EQ(animator.data.states.at(0).transitions.size(), 1);
  EXPECT_EQ(animator.data.states.at(0).transitions.at(0).eventName,
            "NEW_EVENT");
  EXPECT_EQ(animator.data.states.at(0).transitions.at(0).target, 1);
}

TEST_F(AssetCacheAnimatorTest, LoadsAnimatorWithAlreadyLoadedAnimations) {
  quoll::AssetData<quoll::AnimationAsset> animData{};
  animData.uuid = quoll::Uuid("my-animation");

  auto animationHandle = cache.getRegistry().getAnimations().addAsset(animData);

  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "animator";

  auto state = node["states"]["idle"];
  state["output"]["type"] = "animation";
  state["output"]["animation"] = "my-animation";

  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.getPathFromUuid(uuid);
  std::ofstream stream(filePath);
  stream << node;
  stream.close();

  auto res = cache.loadAnimator(uuid);
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

TEST_F(AssetCacheAnimatorTest,
       LoadAnimatorLoadsAnimationsBeforeLoadingAnimator) {
  quoll::AssetData<quoll::AnimationAsset> animData{};
  animData.uuid = quoll::Uuid::generate();
  auto path = cache.createAnimationFromAsset(animData);

  YAML::Node node;
  node["version"] = "0.1";
  node["type"] = "animator";

  auto state = node["states"]["idle"];
  state["output"]["type"] = "animation";
  state["output"]["animation"] = path.getData().stem().string();
  state["output"]["loopMode"] = "linear";
  state["output"]["speed"] = 0.5f;

  std::ofstream stream(FilePath);
  stream << node;
  stream.close();

  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.createAnimatorFromSource(FilePath, uuid);

  auto res = cache.loadAnimator(uuid);
  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());

  auto handle = res.getData();
  EXPECT_TRUE(cache.getRegistry().getAnimators().hasAsset(handle));
  const auto &animator = cache.getRegistry().getAnimators().getAsset(handle);

  EXPECT_EQ(animator.name, "test.animator");
  EXPECT_EQ(animator.data.states.size(), 1);
  EXPECT_EQ(animator.data.states.at(0).name, "idle");
  EXPECT_EQ(animator.data.states.at(0).loopMode,
            quoll::AnimationLoopMode::Linear);
  EXPECT_EQ(animator.data.states.at(0).speed, 0.5f);
  EXPECT_NE(animator.data.states.at(0).animation,
            quoll::AssetHandle<quoll::AnimationAsset>{0});
}

TEST_F(AssetCacheAnimatorTest,
       UpdatesExistingAnimatorIfAssetWithUuidAlreadyExists) {
  quoll::AssetData<quoll::AnimatorAsset> animData{};
  animData.name = "old-name";
  animData.uuid = quoll::Uuid::generate();
  animData.data.states.push_back({});
  auto animatorPath = cache.createAnimatorFromAsset(animData);

  auto result = cache.loadAnimator(animData.uuid);
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result.hasData());

  auto handle = result.getData();

  {
    auto &animator = cache.getRegistry().getAnimators().getAsset(handle);
    EXPECT_EQ(animator.type, quoll::AssetType::Animator);
    EXPECT_EQ(animator.name, "old-name");
  }

  animData.name = "new-name";
  cache.createAnimatorFromAsset(animData);

  {
    auto result = cache.loadAnimator(animData.uuid);
    EXPECT_EQ(result.getData(), handle);

    auto &animator = cache.getRegistry().getAnimators().getAsset(handle);
    EXPECT_EQ(animator.type, quoll::AssetType::Animator);
    EXPECT_EQ(animator.name, "new-name");
  }
}
