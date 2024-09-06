#include "quoll/core/Base.h"
#include "quoll/asset/AssetRef.h"
#include "quoll-tests/Testing.h"

struct Data {
  u32 value;
};

class AssetRefTest : public ::testing::Test {
public:
  quoll::AssetMap<Data> map;

  quoll::AssetHandle<Data>
  allocateAndStore(const Data &data,
                   quoll::Uuid uuid = quoll::Uuid::generate()) {
    auto handle = map.allocate({.uuid = uuid});
    map.store(handle, data);
    return handle;
  }
};

TEST_F(AssetRefTest, ConstructorSetsHandle) {
  auto uuid = quoll::Uuid::generate();
  auto handle = allocateAndStore({.value = 25}, uuid);
  quoll::AssetRef<Data> ref(map, handle);

  EXPECT_TRUE(ref);
  EXPECT_EQ(ref.handle(), handle);
  EXPECT_EQ(ref.meta().uuid, uuid);
  EXPECT_EQ(ref.get().value, 25);
  EXPECT_EQ(ref->value, 25);
}

TEST_F(AssetRefTest, DefaultConstructorSetsHandleToNull) {
  quoll::AssetRef<Data> ref;

  EXPECT_FALSE(ref);
  EXPECT_EQ(ref.handle(), quoll::AssetHandle<Data>());
}
TEST_F(AssetRefTest, ConstructorIncrementsRefCount) {
  auto handle = allocateAndStore({});

  quoll::AssetRef<Data> ref(map, handle);

  EXPECT_EQ(map.getRefCount(handle), 1);
}

TEST_F(AssetRefTest, DestructorDecrementsRefCount) {
  auto handle = allocateAndStore({});

  { quoll::AssetRef<Data> ref(map, handle); }

  EXPECT_EQ(map.getRefCount(handle), 0);
}

TEST_F(AssetRefTest, DestructorDoesNothingIfHandleIsNull) {
  quoll::AssetRef<Data> ref;
}

TEST_F(AssetRefTest, CopyConstructorSetsHandle) {
  auto uuid = quoll::Uuid::generate();
  auto handle = allocateAndStore({.value = 25}, uuid);
  quoll::AssetRef<Data> ref1(map, handle);
  auto ref2 = ref1;

  EXPECT_TRUE(ref2);
  EXPECT_TRUE(ref1);
  EXPECT_EQ(ref1.handle(), ref2.handle());
}

TEST_F(AssetRefTest, CopyConstructorIncrementsRefCount) {
  auto handle = allocateAndStore({});
  quoll::AssetRef<Data> ref1(map, handle);
  auto ref2 = ref1;

  EXPECT_EQ(map.getRefCount(handle), 2);
}

TEST_F(AssetRefTest, CopyConstructorDoesNothingIfHandleIsNull) {
  quoll::AssetRef<Data> ref1;
  auto ref2 = ref1;

  EXPECT_FALSE(ref1);
  EXPECT_FALSE(ref2);
}

TEST_F(AssetRefTest, CopyAssignmentSetsRefFromOther) {
  auto handle1 = allocateAndStore({});
  auto handle2 = allocateAndStore({});

  quoll::AssetRef<Data> ref1(map, handle1);
  quoll::AssetRef<Data> ref2(map, handle2);

  EXPECT_EQ(ref1.handle(), handle1);

  ref1 = ref2;

  EXPECT_EQ(ref1.handle(), handle2);
  EXPECT_EQ(ref1.handle(), ref2.handle());
}

TEST_F(AssetRefTest,
       CopyAssignmentDecrementsRefCountOfSelfAndIncrementsRefCountOfOther) {
  auto handle1 = allocateAndStore({});
  auto handle2 = allocateAndStore({});

  quoll::AssetRef<Data> ref1(map, handle1);
  quoll::AssetRef<Data> ref2(map, handle2);

  ref1 = ref2;

  EXPECT_EQ(map.getRefCount(handle1), 0);
  EXPECT_EQ(map.getRefCount(handle2), 2);
}

TEST_F(AssetRefTest,
       CopyAssignmentOnlyDecrementsRefCountOfSelfIfOtherRefHasNullHandle) {
  auto handle = allocateAndStore({});

  quoll::AssetRef<Data> ref1(map, handle);
  quoll::AssetRef<Data> ref2;

  ref1 = ref2;

  EXPECT_EQ(map.getRefCount(handle), 0);

  EXPECT_FALSE(ref1);
  EXPECT_EQ(ref1.handle(), ref2.handle());
}

TEST_F(AssetRefTest,
       CopyAssignmentOnlyIncrementsRefCountOfOtherIfSelfHasNullHandle) {
  auto handle = allocateAndStore({});

  quoll::AssetRef<Data> ref1;
  quoll::AssetRef<Data> ref2(map, handle);

  ref1 = ref2;

  EXPECT_EQ(map.getRefCount(handle), 2);
}

TEST_F(AssetRefTest, CopyAssignmentHasNoEffectIfBothSidesHaveNullHandles) {
  quoll::AssetRef<Data> ref1;
  quoll::AssetRef<Data> ref2;

  ref1 = ref2;
  EXPECT_FALSE(ref1);
  EXPECT_FALSE(ref2);
}

TEST_F(AssetRefTest, MoveConstructorSetsHandleOfSelfAndClearsHandleOfOther) {
  auto uuid = quoll::Uuid::generate();
  auto handle = allocateAndStore({.value = 25}, uuid);
  quoll::AssetRef<Data> ref1(map, handle);
  auto ref2 = std::move(ref1);

  EXPECT_TRUE(ref2);
  EXPECT_FALSE(ref1);
}

TEST_F(AssetRefTest, MoveConstructorDoesNotIncrementRefCount) {
  auto handle = allocateAndStore({});
  quoll::AssetRef<Data> ref1(map, handle);

  EXPECT_EQ(map.getRefCount(handle), 1);

  auto ref2 = std::move(ref1);

  EXPECT_EQ(map.getRefCount(handle), 1);
}

TEST_F(AssetRefTest, MoveConstructorDoesNothingIfHandleIsNull) {
  quoll::AssetRef<Data> ref1;
  auto ref2 = ref1;

  EXPECT_FALSE(ref1);
  EXPECT_FALSE(ref2);
}

TEST_F(AssetRefTest, MoveAssignmentSetsHandleOfSelfAndClearsHandleOfOther) {
  auto handle1 = allocateAndStore({});
  auto handle2 = allocateAndStore({});

  quoll::AssetRef<Data> ref1(map, handle1);
  quoll::AssetRef<Data> ref2(map, handle2);

  ref1 = std::move(ref2);

  EXPECT_EQ(ref1.handle(), handle2);
  EXPECT_FALSE(ref2);
}

TEST_F(
    AssetRefTest,
    MoveAssignmentDecrementsRefCountOfSelfButDoesNotIncrementRefCountOfOther) {
  auto handle1 = allocateAndStore({});
  auto handle2 = allocateAndStore({});

  quoll::AssetRef<Data> ref1(map, handle1);
  quoll::AssetRef<Data> ref2(map, handle2);

  EXPECT_EQ(map.getRefCount(handle1), 1);
  EXPECT_EQ(map.getRefCount(handle2), 1);

  ref1 = std::move(ref2);

  EXPECT_EQ(map.getRefCount(handle1), 0);
  EXPECT_EQ(map.getRefCount(handle2), 1);
}

TEST_F(AssetRefTest, MoveAssignmentDoesNotAffectRefCountIfSelfHasNullHandle) {
  auto handle = allocateAndStore({});

  quoll::AssetRef<Data> ref1;
  quoll::AssetRef<Data> ref2(map, handle);

  EXPECT_EQ(map.getRefCount(handle), 1);

  ref1 = std::move(ref2);

  EXPECT_EQ(map.getRefCount(handle), 1);
}

TEST_F(AssetRefTest,
       MoveAssignmentOnlyDecrementsRefCountOfSelfIfOtherRefHasNullHandle) {
  auto handle = allocateAndStore({});

  quoll::AssetRef<Data> ref1(map, handle);
  quoll::AssetRef<Data> ref2;

  EXPECT_EQ(map.getRefCount(handle), 1);

  ref1 = std::move(ref2);

  EXPECT_EQ(map.getRefCount(handle), 0);
}

TEST_F(AssetRefTest, MoveAssignmentHasNoEffectIfBothSidesHaveNullHandles) {
  quoll::AssetRef<Data> ref1;
  quoll::AssetRef<Data> ref2;

  ref1 = std::move(ref2);
  EXPECT_FALSE(ref1);
  EXPECT_FALSE(ref2);
}