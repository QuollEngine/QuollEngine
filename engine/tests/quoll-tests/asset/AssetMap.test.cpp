#include "quoll/core/Base.h"
#include "quoll/asset/AssetHandle.h"
#include "quoll/asset/AssetMap.h"
#include "quoll/asset/AssetMeta.h"
#include "quoll-tests/Testing.h"

struct TestAsset {
  u32 data = 0;
};

class AssetMapTest : public ::testing::Test {
public:
  quoll::AssetMap<TestAsset> map;

  quoll::AssetMeta createMeta(quoll::String name) {
    return {.type = quoll::AssetType::InputMap,
            .name = name,
            .uuid = quoll::Uuid::generate()};
  }
};

TEST_F(AssetMapTest, GetFailsIfAssetIsNotAllocated) {
  quoll::AssetHandle<TestAsset> handle(25);
  EXPECT_DEATH(map.get(handle), ".*");
}

TEST_F(AssetMapTest, AllocateFailsIfInvalidUuid) {
  EXPECT_DEATH(map.allocate({}), ".*");
}

TEST_F(AssetMapTest, AllocateStoresAssetMetadata) {
  auto meta = createMeta("test");
  auto handle = map.allocate(meta);

  EXPECT_TRUE(handle);
  EXPECT_TRUE(map.contains(handle));
  EXPECT_EQ(map.getMeta(handle).name, "test");
  EXPECT_EQ(map.getMeta(handle).uuid, meta.uuid);
  EXPECT_EQ(map.getMeta(handle).type, meta.type);
}

TEST_F(AssetMapTest, AllocateUpdatesExistingMetadata) {
  auto meta = createMeta("test");
  auto handle1 = map.allocate(meta);

  meta.name = "hello";
  meta.type = quoll::AssetType::Animator;
  auto handle2 = map.allocate(meta);

  EXPECT_EQ(handle1, handle2);

  EXPECT_TRUE(handle2);
  EXPECT_TRUE(map.contains(handle2));
  EXPECT_EQ(map.getMeta(handle2).name, "hello");
  EXPECT_EQ(map.getMeta(handle2).uuid, meta.uuid);
  EXPECT_EQ(map.getMeta(handle2).type, meta.type);
}

TEST_F(AssetMapTest, AllocateDoesNotStoreAssetData) {
  auto meta = createMeta("test");
  auto handle = map.allocate(meta);

  EXPECT_FALSE(map.hasData(handle));
  EXPECT_DEATH(map.get(handle), ".*");
}

TEST_F(AssetMapTest, StoreFailsIfProvidedAssetDoesNotExist) {
  quoll::AssetHandle<TestAsset> handle(25);
  EXPECT_DEATH(map.store(handle, {}), ".*");
}

TEST_F(AssetMapTest, StoresCreatesAssetDataForAllocatedAsset) {
  auto meta = createMeta("test");
  auto handle = map.allocate(meta);

  map.store(handle, {.data = 25});
  EXPECT_TRUE(map.hasData(handle));
  EXPECT_EQ(map.get(handle).data, 25);
  EXPECT_EQ(map.getRefCount(handle), 0);
}

TEST_F(AssetMapTest, StoreUpdatesExistingDataForAllocatedAsset) {
  auto meta = createMeta("test");
  auto handle = map.allocate(meta);

  map.store(handle, {.data = 25});

  map.store(handle, {.data = 35});
  EXPECT_TRUE(map.hasData(handle));
  EXPECT_EQ(map.get(handle).data, 35);
  EXPECT_EQ(map.getRefCount(handle), 0);
}

TEST_F(AssetMapTest, TaksFailsIfAssetIsNotAllocated) {
  quoll::AssetHandle<TestAsset> handle(25);
  EXPECT_DEATH(map.take(handle), ".*");
}

TEST_F(AssetMapTest, TakeFailsIfAssetHasNoData) {
  auto meta = createMeta("test");
  auto handle = map.allocate(meta);

  EXPECT_DEATH(map.take(handle), ".*");
}

TEST_F(AssetMapTest, TakeIncreasesReferenceCountOfAsset) {
  auto meta = createMeta("test");
  auto handle = map.allocate(meta);
  map.store(handle, {.data = 25});
  map.take(handle);

  EXPECT_EQ(map.getRefCount(handle), 1);
}

TEST_F(AssetMapTest, ReleaseFailsIfAssetIsNotAllocated) {
  quoll::AssetHandle<TestAsset> handle(25);
  EXPECT_DEATH(map.release(handle), ".*");
}

TEST_F(AssetMapTest, ReleaseFailsIfAssetHasNoData) {
  auto meta = createMeta("test");
  auto handle = map.allocate(meta);

  EXPECT_DEATH(map.release(handle), ".*");
}

TEST_F(AssetMapTest, ReleaseFailsIfAssetReferenceCountIsAlreadyZero) {
  auto meta = createMeta("test");
  auto handle = map.allocate(meta);
  map.store(handle, {.data = 25});

  EXPECT_DEATH(map.release(handle), ".*");
}

TEST_F(AssetMapTest, ReleaseDecreasesReferenceCountOfAsset) {
  auto meta = createMeta("test");
  auto handle = map.allocate(meta);
  map.store(handle, {.data = 25});

  map.take(handle);
  map.take(handle);
  EXPECT_EQ(map.getRefCount(handle), 2);

  map.release(handle);
  EXPECT_EQ(map.getRefCount(handle), 1);
}

TEST_F(AssetMapTest, StoreDoesNotAffectReferenceCountOfAsset) {
  auto meta = createMeta("test");
  auto handle = map.allocate(meta);
  map.store(handle, {.data = 25});

  map.take(handle);
  map.take(handle);
  EXPECT_EQ(map.getRefCount(handle), 2);

  map.store(handle, {.data = 35});
  EXPECT_EQ(map.getRefCount(handle), 2);
}

TEST_F(AssetMapTest, ReallocateDoesNotAffectExistingDataOrReferenceCount) {
  auto meta = createMeta("test");
  auto handle1 = map.allocate(meta);
  map.store(handle1, {.data = 25});

  map.take(handle1);
  map.take(handle1);
  EXPECT_EQ(map.getRefCount(handle1), 2);

  meta.name = "hello";
  meta.type = quoll::AssetType::Animator;
  auto handle2 = map.allocate(meta);

  EXPECT_EQ(handle1, handle2);

  EXPECT_EQ(map.getRefCount(handle1), 2);
  EXPECT_EQ(map.get(handle1).data, 25);
}
