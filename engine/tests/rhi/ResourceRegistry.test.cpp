#include "liquid/core/Base.h"
#include "liquid/rhi/ResourceRegistry.h"

#include <gtest/gtest.h>

struct TestDesc {
  uint32_t value = 0;
};

using TestMap = liquid::rhi::ResourceRegistryMap<uint32_t, TestDesc>;

TEST(ResourceRegistryMapTest, AddsNewResource) {
  TestMap map;
  auto id = map.addDescription({1});

  EXPECT_EQ(map.getDescription(id).value, 1);
}

TEST(ResourceRegistryMapTest, AddingNewResourceAddsNewIdToCreateList) {
  TestMap map;
  auto id = map.addDescription({1});
  EXPECT_EQ(map.getDirtyCreates().size(), 1);
  EXPECT_EQ(map.getDirtyCreates().at(0), id);
}

TEST(ResourceRegistryMapTest, ClearsCreateListOnCall) {
  TestMap map;
  map.addDescription({1});
  map.clearDirtyCreates();
  EXPECT_EQ(map.getDirtyCreates().size(), 0);
}

TEST(ResourceRegistryMapTest, UpdatesResource) {
  TestMap map;
  auto id = map.addDescription({1});
  map.updateDescription(id, {2});

  EXPECT_EQ(map.getDescription(id).value, 2);
}

TEST(ResourceRegistryMapTest, UpdatingResourceAddsIdToUpdateList) {
  TestMap map;
  auto id = map.addDescription({1});
  map.updateDescription(id, {2});
  EXPECT_EQ(map.getDirtyUpdates().size(), 1);
  EXPECT_EQ(map.getDirtyUpdates().at(0), id);
}

TEST(ResourceRegistryMapTest, ClearsUpdateListOnCall) {
  TestMap map;
  auto id = map.addDescription({1});
  map.updateDescription(id, {2});

  map.clearDirtyUpdates();
  EXPECT_EQ(map.getDirtyUpdates().size(), 0);
}

TEST(ResourceRegistryMapTest, DeletesResource) {
  TestMap map;
  auto id = map.addDescription({1});
  map.deleteDescription(id);
  EXPECT_FALSE(map.hasDescription(id));
}

TEST(ResourceRegistryMapTest, DeletingResourceAddsIdToDeleteList) {
  TestMap map;
  auto id = map.addDescription({1});
  map.deleteDescription(id);
  EXPECT_EQ(map.getDirtyDeletes().at(0), id);
}

TEST(ResourceRegistryMapTest, ClearsDeleteListOnCall) {
  TestMap map;
  auto id = map.addDescription({1});
  map.deleteDescription(id);

  map.clearDirtyDeletes();
  EXPECT_EQ(map.getDirtyDeletes().size(), 0);
}
