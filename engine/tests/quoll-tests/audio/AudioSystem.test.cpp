#include "quoll/core/Base.h"
#include "quoll/audio/AudioSystem.h"
#include "quoll/system/SystemView.h"
#include "quoll-tests/Testing.h"

struct FakeAudioData {};

class TestAudioBackend {
  struct AudioStatus {
    bool playing = true;
  };

public:
  void *playSound(const quoll::AudioAsset &data) {
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
  AudioSystemTest() : audioSystem(assetRegistry) {
    audioSystem.createSystemViewData(view);
  }

  quoll::AudioAssetHandle createFakeAudio() {
    std::vector<char> bytes;
    quoll::AssetData<quoll::AudioAsset> asset{};
    asset.data.bytes = bytes;
    asset.data.format = quoll::AudioAssetFormat::Wav;
    return assetRegistry.getAudios().addAsset(asset);
  }

  quoll::AssetRegistry assetRegistry;
  quoll::AudioSystem<TestAudioBackend> audioSystem;
  quoll::Scene scene;
  quoll::SystemView view{&scene};
  quoll::EntityDatabase &entityDatabase = scene.entityDatabase;
};

TEST_F(AudioSystemTest,
       DoesNothingIfThereAreNoEntitiesWithAudioSourceComponents) {
  auto e1 = entityDatabase.create();
  audioSystem.output(view);

  EXPECT_FALSE(entityDatabase.has<quoll::AudioStatus>(e1));

  entityDatabase.set<quoll::AudioStart>(e1, {});
  audioSystem.output(view);

  EXPECT_FALSE(entityDatabase.has<quoll::AudioStatus>(e1));
}

TEST_F(AudioSystemTest,
       DoesNothingIfThereAreNoEntitiesWithAudioStartComponents) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();

  entityDatabase.set<quoll::AudioSource>(e1, {handle});
  audioSystem.output(view);

  EXPECT_FALSE(entityDatabase.has<quoll::AudioStatus>(e1));
}

TEST_F(
    AudioSystemTest,
    CreatesAudioSourceComponentsForEntitiesThatHaveAudioSourceAndStartComponents) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<quoll::AudioStart>(e1, {});
  entityDatabase.set<quoll::AudioSource>(e1, {handle});

  EXPECT_FALSE(entityDatabase.has<quoll::AudioStatus>(e1));

  audioSystem.output(view);

  EXPECT_TRUE(entityDatabase.has<quoll::AudioStatus>(e1));
}

TEST_F(AudioSystemTest, DoesNotPlaySoundIfAudioStatusComponentExistsForEntity) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<quoll::AudioStart>(e1, {});
  entityDatabase.set<quoll::AudioSource>(e1, {handle});

  audioSystem.output(view);

  EXPECT_FALSE(entityDatabase.has<quoll::AudioStart>(e1));
  EXPECT_TRUE(entityDatabase.has<quoll::AudioStatus>(e1));

  auto *prevInstance = entityDatabase.get<quoll::AudioStatus>(e1).instance;

  entityDatabase.set<quoll::AudioStart>(e1, {});
  audioSystem.output(view);

  EXPECT_FALSE(entityDatabase.has<quoll::AudioStart>(e1));

  auto *newInstance = entityDatabase.get<quoll::AudioStatus>(e1).instance;

  EXPECT_EQ(prevInstance, newInstance);
}

TEST_F(AudioSystemTest,
       RemovesAudioStartComponentFromAllEntitiesWithAudioStartComponents) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<quoll::AudioStart>(e1, {});
  entityDatabase.set<quoll::AudioSource>(e1, {handle});

  audioSystem.output(view);

  EXPECT_FALSE(entityDatabase.has<quoll::AudioStart>(e1));
}

TEST_F(AudioSystemTest, DeletesAudioStatusComponentFromFinishedAudios) {
  auto &backend = audioSystem.getBackend();

  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<quoll::AudioStart>(e1, {});
  entityDatabase.set<quoll::AudioSource>(e1, {handle});

  audioSystem.output(view);

  auto *sound = entityDatabase.get<quoll::AudioStatus>(e1).instance;
  EXPECT_TRUE(backend.hasSound(sound));

  backend.setStatus(sound, false);
  audioSystem.output(view);
  EXPECT_TRUE(backend.hasSound(sound));

  audioSystem.output(view);
  EXPECT_FALSE(backend.hasSound(sound));
}

TEST_F(AudioSystemTest, CleanupDeletesAllAudioStatuses) {
  auto &backend = audioSystem.getBackend();

  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<quoll::AudioStart>(e1, {});
  entityDatabase.set<quoll::AudioSource>(e1, {handle});

  audioSystem.output(view);

  auto *sound = entityDatabase.get<quoll::AudioStatus>(e1).instance;
  EXPECT_TRUE(backend.hasSound(sound));

  entityDatabase.set<quoll::AudioStart>(e1, {});
  EXPECT_TRUE(entityDatabase.has<quoll::AudioStart>(e1));

  audioSystem.cleanup(view);

  EXPECT_FALSE(entityDatabase.has<quoll::AudioStart>(e1));
  EXPECT_FALSE(entityDatabase.has<quoll::AudioStatus>(e1));
  EXPECT_FALSE(backend.hasSound(sound));
}

TEST_F(AudioSystemTest, DeletesAudioComponentsWhenAudioSourceIsDeleted) {
  auto &backend = audioSystem.getBackend();

  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<quoll::AudioStart>(e1, {});
  entityDatabase.set<quoll::AudioSource>(e1, {handle});
  entityDatabase.remove<quoll::AudioSource>(e1);

  auto e2 = entityDatabase.create();
  entityDatabase.set<quoll::AudioStart>(e2, {});
  entityDatabase.set<quoll::AudioSource>(e2, {handle});
  entityDatabase.deleteEntity(e2);

  audioSystem.output(view);

  EXPECT_FALSE(entityDatabase.has<quoll::AudioStart>(e1));
  EXPECT_FALSE(entityDatabase.has<quoll::AudioStatus>(e1));
}

TEST_F(AudioSystemTest, DeletesAudioDataWhenAudioSourceComponentsAreDeleted) {
  auto &backend = audioSystem.getBackend();

  auto handle = createFakeAudio();

  auto e1 = entityDatabase.create();
  entityDatabase.set<quoll::AudioStart>(e1, {});
  entityDatabase.set<quoll::AudioSource>(e1, {handle});

  auto e2 = entityDatabase.create();
  entityDatabase.set<quoll::AudioStart>(e2, {});
  entityDatabase.set<quoll::AudioSource>(e2, {handle});

  audioSystem.output(view);
  auto *sound1 = entityDatabase.get<quoll::AudioStatus>(e1).instance;
  auto *sound2 = entityDatabase.get<quoll::AudioStatus>(e2).instance;

  entityDatabase.remove<quoll::AudioSource>(e1);
  entityDatabase.deleteEntity(e2);

  audioSystem.output(view);

  EXPECT_FALSE(entityDatabase.has<quoll::AudioStart>(e1));
  EXPECT_FALSE(entityDatabase.has<quoll::AudioStatus>(e1));
  EXPECT_FALSE(backend.hasSound(sound1));
  EXPECT_FALSE(backend.hasSound(sound2));
}
