#include "quoll/core/Base.h"
#include "quoll/audio/AudioStart.h"
#include "quoll/audio/AudioStatus.h"
#include "quoll/audio/AudioSystem.h"
#include "quoll/system/SystemView.h"
#include "quoll-tests/Testing.h"

struct FakeAudioData {};

class TestAudioBackend : public quoll::AudioBackend {
  struct AudioStatus {
    bool playing = true;
  };

public:
  void *playSound(const quoll::AudioAsset &data) override {
    auto *sound = new FakeAudioData;
    mInstances.insert_or_assign(sound, AudioStatus());

    return sound;
  }

  bool isPlaying(void *sound) override {
    return mInstances.at(static_cast<FakeAudioData *>(sound)).playing;
  }

  void destroySound(void *sound) override {
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
  AudioSystemTest() : audioSystem(backend, assetRegistry) {
    audioSystem.createSystemViewData(view);
  }

  quoll::AudioAssetHandle createFakeAudio() {
    std::vector<char> bytes;
    quoll::AssetData<quoll::AudioAsset> asset{};
    asset.data.bytes = bytes;
    asset.data.format = quoll::AudioAssetFormat::Wav;
    return assetRegistry.getAudios().addAsset(asset);
  }

  TestAudioBackend *backend = new TestAudioBackend;
  quoll::AssetRegistry assetRegistry;
  quoll::AudioSystem audioSystem;
  quoll::Scene scene;
  quoll::SystemView view{&scene};
  quoll::EntityDatabase &entityDatabase = scene.entityDatabase;
};

TEST_F(AudioSystemTest,
       DoesNothingIfThereAreNoEntitiesWithAudioSourceComponents) {
  auto e1 = entityDatabase.entity();
  audioSystem.output(view);

  EXPECT_FALSE(e1.has<quoll::AudioStatus>());

  e1.add<quoll::AudioStart>();
  audioSystem.output(view);

  EXPECT_FALSE(e1.has<quoll::AudioStatus>());
}

TEST_F(AudioSystemTest,
       DoesNothingIfThereAreNoEntitiesWithAudioStartComponents) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.entity();

  e1.set<quoll::AudioSource>({handle});
  audioSystem.output(view);

  EXPECT_FALSE(e1.has<quoll::AudioStatus>());
}

TEST_F(
    AudioSystemTest,
    CreatesAudioSourceComponentsForEntitiesThatHaveAudioSourceAndStartComponents) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.entity();
  e1.add<quoll::AudioStart>();
  e1.set<quoll::AudioSource>({handle});

  EXPECT_FALSE(e1.has<quoll::AudioStatus>());

  audioSystem.output(view);

  EXPECT_TRUE(e1.has<quoll::AudioStatus>());
}

TEST_F(AudioSystemTest, DoesNotPlaySoundIfAudioStatusComponentExistsForEntity) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.entity();
  e1.add<quoll::AudioStart>();
  e1.set<quoll::AudioSource>({handle});

  audioSystem.output(view);

  EXPECT_FALSE(e1.has<quoll::AudioStart>());
  EXPECT_TRUE(e1.has<quoll::AudioStatus>());

  auto *prevInstance = e1.get_ref<quoll::AudioStatus>()->instance;

  e1.add<quoll::AudioStart>();
  audioSystem.output(view);

  EXPECT_FALSE(e1.has<quoll::AudioStart>());

  auto *newInstance = e1.get_ref<quoll::AudioStatus>()->instance;

  EXPECT_EQ(prevInstance, newInstance);
}

TEST_F(AudioSystemTest,
       RemovesAudioStartComponentFromAllEntitiesWithAudioStartComponents) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.entity();
  e1.add<quoll::AudioStart>();
  e1.set<quoll::AudioSource>({handle});

  audioSystem.output(view);

  EXPECT_FALSE(e1.has<quoll::AudioStart>());
}

TEST_F(AudioSystemTest, DeletesAudioStatusComponentFromFinishedAudios) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.entity();
  e1.add<quoll::AudioStart>();
  e1.set<quoll::AudioSource>({handle});

  audioSystem.output(view);

  auto *sound = e1.get_ref<quoll::AudioStatus>()->instance;
  EXPECT_TRUE(backend->hasSound(sound));

  backend->setStatus(sound, false);
  audioSystem.output(view);
  EXPECT_FALSE(backend->hasSound(sound));

  audioSystem.output(view);
  EXPECT_FALSE(backend->hasSound(sound));
}

TEST_F(AudioSystemTest, CleanupDeletesAllAudioStatuses) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.entity();
  e1.add<quoll::AudioStart>();
  e1.set<quoll::AudioSource>({handle});

  audioSystem.output(view);

  auto *sound = e1.get_ref<quoll::AudioStatus>()->instance;
  EXPECT_TRUE(backend->hasSound(sound));

  e1.add<quoll::AudioStart>();
  EXPECT_TRUE(e1.has<quoll::AudioStart>());

  audioSystem.cleanup(view);

  EXPECT_FALSE(e1.has<quoll::AudioStart>());
  EXPECT_FALSE(e1.has<quoll::AudioStatus>());
  EXPECT_FALSE(backend->hasSound(sound));
}

TEST_F(AudioSystemTest, DeletesAudioComponentsWhenAudioSourceIsDeleted) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.entity();
  e1.add<quoll::AudioStart>();
  e1.set<quoll::AudioSource>({handle});
  e1.remove<quoll::AudioSource>();

  auto e2 = entityDatabase.entity();
  e2.add<quoll::AudioStart>();
  e2.set<quoll::AudioSource>({handle});
  e2.destruct();

  audioSystem.output(view);

  EXPECT_FALSE(e1.has<quoll::AudioStart>());
  EXPECT_FALSE(e1.has<quoll::AudioStatus>());
}

TEST_F(AudioSystemTest, DeletesAudioDataWhenAudioSourceComponentsAreDeleted) {
  auto handle = createFakeAudio();

  auto e1 = entityDatabase.entity();
  e1.add<quoll::AudioStart>();
  e1.set<quoll::AudioSource>({handle});

  auto e2 = entityDatabase.entity();
  e2.add<quoll::AudioStart>();
  e2.set<quoll::AudioSource>({handle});

  audioSystem.output(view);
  auto *sound1 = e1.get_ref<quoll::AudioStatus>()->instance;
  auto *sound2 = e2.get_ref<quoll::AudioStatus>()->instance;

  e1.remove<quoll::AudioSource>();
  e2.destruct();

  audioSystem.output(view);

  EXPECT_FALSE(e1.has<quoll::AudioStart>());
  EXPECT_FALSE(e1.has<quoll::AudioStatus>());
  EXPECT_FALSE(backend->hasSound(sound1));
  EXPECT_FALSE(backend->hasSound(sound2));
}
