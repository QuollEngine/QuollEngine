#include "liquid/core/Base.h"
#include "liquid/audio/AudioSystem.h"

#include "liquid-tests/Testing.h"

struct FakeAudioData {};

class TestAudioBackend {
  struct AudioStatus {
    bool playing = true;
  };

public:
  void *playSound(const liquid::AudioAsset &data) {
    auto *sound = new FakeAudioData;
    mInstances.insert_or_assign(sound, AudioStatus());

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
    std::vector<char> bytes;
    liquid::AssetData<liquid::AudioAsset> asset{};
    asset.data.bytes = bytes;
    asset.data.format = liquid::AudioAssetFormat::Wav;
    return assetRegistry.getAudios().addAsset(asset);
  }

  liquid::AssetRegistry assetRegistry;
  liquid::AudioSystem<TestAudioBackend> audioSystem;
  liquid::EntityDatabase entityDatabase;
};

TEST_F(AudioSystemTest,
       DoesNothingIfThereAreNoEntitiesWithAudioSourceComponents) {
  auto e1 = entityDatabase.create();
  audioSystem.output(entityDatabase);

  EXPECT_FALSE(entityDatabase.has<liquid::AudioStatus>(e1));

  entityDatabase.set<liquid::AudioStart>(e1, {});
  audioSystem.output(entityDatabase);

  EXPECT_FALSE(entityDatabase.has<liquid::AudioStatus>(e1));
}

TEST_F(AudioSystemTest,
       DoesNothingIfThereAreNoEntitiesWithAudioStartComponents) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();

  entityDatabase.set<liquid::AudioSource>(e1, {handle});
  audioSystem.output(entityDatabase);

  EXPECT_FALSE(entityDatabase.has<liquid::AudioStatus>(e1));
}

TEST_F(
    AudioSystemTest,
    CreatesAudioSourceComponentsForEntitiesThatHaveAudioSourceAndStartComponents) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<liquid::AudioStart>(e1, {});
  entityDatabase.set<liquid::AudioSource>(e1, {handle});

  EXPECT_FALSE(entityDatabase.has<liquid::AudioStatus>(e1));

  audioSystem.output(entityDatabase);

  EXPECT_TRUE(entityDatabase.has<liquid::AudioStatus>(e1));
}

TEST_F(AudioSystemTest, DoesNotPlaySoundIfAudioStatusComponentExistsForEntity) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<liquid::AudioStart>(e1, {});
  entityDatabase.set<liquid::AudioSource>(e1, {handle});

  audioSystem.output(entityDatabase);

  EXPECT_FALSE(entityDatabase.has<liquid::AudioStart>(e1));
  EXPECT_TRUE(entityDatabase.has<liquid::AudioStatus>(e1));

  auto *prevInstance = entityDatabase.get<liquid::AudioStatus>(e1).instance;

  entityDatabase.set<liquid::AudioStart>(e1, {});
  audioSystem.output(entityDatabase);

  EXPECT_FALSE(entityDatabase.has<liquid::AudioStart>(e1));

  auto *newInstance = entityDatabase.get<liquid::AudioStatus>(e1).instance;

  EXPECT_EQ(prevInstance, newInstance);
}

TEST_F(AudioSystemTest,
       RemovesAudioStartComponentFromAllEntitiesWithAudioStartComponents) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<liquid::AudioStart>(e1, {});
  entityDatabase.set<liquid::AudioSource>(e1, {handle});

  audioSystem.output(entityDatabase);

  EXPECT_FALSE(entityDatabase.has<liquid::AudioStart>(e1));
}

TEST_F(AudioSystemTest, DeletesAudioStatusComponentFromFinishedAudios) {
  auto &backend = audioSystem.getBackend();

  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<liquid::AudioStart>(e1, {});
  entityDatabase.set<liquid::AudioSource>(e1, {handle});

  audioSystem.output(entityDatabase);

  auto *sound = entityDatabase.get<liquid::AudioStatus>(e1).instance;
  EXPECT_TRUE(backend.hasSound(sound));

  backend.setStatus(sound, false);
  audioSystem.output(entityDatabase);

  EXPECT_FALSE(backend.hasSound(sound));
}

TEST_F(AudioSystemTest, CleanupDeletesAllAudioStatuses) {
  auto &backend = audioSystem.getBackend();

  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<liquid::AudioStart>(e1, {});
  entityDatabase.set<liquid::AudioSource>(e1, {handle});

  audioSystem.output(entityDatabase);

  auto *sound = entityDatabase.get<liquid::AudioStatus>(e1).instance;
  EXPECT_TRUE(backend.hasSound(sound));

  entityDatabase.set<liquid::AudioStart>(e1, {});
  EXPECT_TRUE(entityDatabase.has<liquid::AudioStart>(e1));

  audioSystem.cleanup(entityDatabase);

  EXPECT_FALSE(entityDatabase.has<liquid::AudioStart>(e1));
  EXPECT_FALSE(entityDatabase.has<liquid::AudioStatus>(e1));
  EXPECT_FALSE(backend.hasSound(sound));
}

TEST_F(AudioSystemTest, DeletesAudioStartComponentsWithDeleteComponents) {
  auto &backend = audioSystem.getBackend();

  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<liquid::AudioStart>(e1, {});
  entityDatabase.set<liquid::AudioSource>(e1, {handle});
  entityDatabase.set<liquid::Delete>(e1, {});

  audioSystem.output(entityDatabase);

  EXPECT_FALSE(entityDatabase.has<liquid::AudioStart>(e1));
  EXPECT_FALSE(entityDatabase.has<liquid::AudioStatus>(e1));
}

TEST_F(AudioSystemTest, DeletesAudioStatusComponentsWithDeleteComponents) {
  auto &backend = audioSystem.getBackend();

  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<liquid::AudioStart>(e1, {});
  entityDatabase.set<liquid::AudioSource>(e1, {handle});

  audioSystem.output(entityDatabase);

  auto *sound = entityDatabase.get<liquid::AudioStatus>(e1).instance;

  entityDatabase.set<liquid::Delete>(e1, {});

  audioSystem.output(entityDatabase);

  EXPECT_FALSE(entityDatabase.has<liquid::AudioStart>(e1));
  EXPECT_FALSE(entityDatabase.has<liquid::AudioStatus>(e1));
  EXPECT_FALSE(backend.hasSound(sound));
}
