#include "liquid/core/Base.h"
#include "liquid/audio/AudioSystem.h"

#include "liquid-tests/Testing.h"

struct FakeAudioData {};

class TestAudioBackend {
  struct AudioStatus {
    bool playing = true;
  };

public:
  void *playSound(void *data) {
    auto *sound = new FakeAudioData;
    mInstances.insert_or_assign(sound, AudioStatus{true});
    return sound;
  }

  bool isPlaying(void *sound) {
    return mInstances.at(static_cast<FakeAudioData *>(sound)).playing;
  }

  void destroySound(void *sound) {
    auto *fakeAudio = static_cast<FakeAudioData *>(sound);
    mInstances.erase(fakeAudio);
    delete fakeAudio;
  }

  void setStatus(void *sound, bool playing) {
    mInstances.at(static_cast<FakeAudioData *>(sound)).playing = playing;
  }

  bool hasSound(void *sound) {
    return mInstances.find(static_cast<FakeAudioData *>(sound)) !=
           mInstances.end();
  }

  inline const std::unordered_map<FakeAudioData *, AudioStatus> &
  getInstances() {
    return mInstances;
  }

private:
  std::unordered_map<FakeAudioData *, AudioStatus> mInstances;
};

class AudioSystemTest : public ::testing::Test {
public:
  AudioSystemTest() : audioSystem(assetRegistry) {}

  liquid::AudioAssetHandle createFakeAudio() {
    auto *data = new FakeAudioData;
    liquid::AssetData<liquid::AudioAsset> asset{};
    asset.data.data = data;
    return assetRegistry.getAudios().addAsset(asset);
  }

  liquid::AssetRegistry assetRegistry;
  liquid::AudioSystem<TestAudioBackend> audioSystem;
  liquid::EntityDatabase entityDatabase;
};

TEST_F(AudioSystemTest,
       DoesNothingIfThereAreNoEntitiesWithAudioSourceComponents) {
  auto e1 = entityDatabase.createEntity();
  audioSystem.output(entityDatabase);

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::AudioStatusComponent>(e1));

  entityDatabase.setComponent<liquid::AudioStartComponent>(e1, {});
  audioSystem.output(entityDatabase);

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::AudioStatusComponent>(e1));
}

TEST_F(AudioSystemTest,
       DoesNothingIfThereAreNoEntitiesWithAudioStartComponents) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.createEntity();

  entityDatabase.setComponent<liquid::AudioSourceComponent>(e1, {handle});
  audioSystem.output(entityDatabase);

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::AudioStatusComponent>(e1));
}

TEST_F(
    AudioSystemTest,
    CreatesAudioSourceComponentsForEntitiesThatHaveAudioSourceAndStartComponents) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::AudioStartComponent>(e1, {});
  entityDatabase.setComponent<liquid::AudioSourceComponent>(e1, {handle});

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::AudioStatusComponent>(e1));

  audioSystem.output(entityDatabase);

  EXPECT_TRUE(entityDatabase.hasComponent<liquid::AudioStatusComponent>(e1));
}

TEST_F(AudioSystemTest,
       RemovesAudioStartComponentFromAllEntitiesWithAudioStartComponents) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::AudioStartComponent>(e1, {});
  entityDatabase.setComponent<liquid::AudioSourceComponent>(e1, {handle});

  audioSystem.output(entityDatabase);

  EXPECT_FALSE(entityDatabase.hasComponent<liquid::AudioStartComponent>(e1));
}

TEST_F(AudioSystemTest, DeletesAudioStatusComponentFromFinishedAudios) {
  auto &backend = audioSystem.getBackend();

  auto handle = createFakeAudio();

  auto e1 = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::AudioStartComponent>(e1, {});
  entityDatabase.setComponent<liquid::AudioSourceComponent>(e1, {handle});

  audioSystem.output(entityDatabase);

  auto *sound =
      entityDatabase.getComponent<liquid::AudioStatusComponent>(e1).instance;
  EXPECT_TRUE(backend.hasSound(sound));

  backend.setStatus(sound, false);
  audioSystem.output(entityDatabase);

  EXPECT_FALSE(backend.hasSound(sound));
}
