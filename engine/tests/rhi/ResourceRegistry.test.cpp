#include "liquid/core/Base.h"
#include "liquid/rhi/ResourceRegistry.h"

#include <gtest/gtest.h>

struct TestDesc {
  uint32_t value = 0;
};

using TestHandle = liquid::rhi::ShaderHandle;

using TestMap = liquid::rhi::ResourceRegistryMap<TestHandle, TestDesc>;

TEST(ResourceRegistryMapTest,
     SettingDescriptionToNonExistentResourceCreatesResource) {
  TestMap map;
  auto id = map.setDescription({1}, TestHandle::Invalid);
  EXPECT_EQ(map.getDescription(id).value, 1);
}

TEST(ResourceRegistryMapTest, FailsToSetResourceIfValidResourceDoesNotExist) {
  TestMap map;
  EXPECT_DEATH(map.setDescription({1}, TestHandle{5}), ".*");
}

TEST(ResourceRegistryMapTest,
     SettingDescriptionToValidResourceUpdatesResource) {
  TestMap map;
  auto id = map.setDescription({1}, TestHandle::Invalid);
  auto updateId = map.setDescription({2}, id);
  EXPECT_EQ(updateId, id);
  EXPECT_EQ(map.getDescription(id).value, 2);
}

TEST(ResourceRegistryMapTest,
     UpdatingDescriptionAddsResourceToStagedWithSetFlag) {
  TestMap map;
  auto id = map.setDescription({1}, TestHandle::Invalid);
  EXPECT_EQ(map.getStagedResources().at(id),
            liquid::rhi::ResourceRegistryState::Set);
}

TEST(ResourceRegistryMapTest, DeletesDescription) {
  TestMap map;
  auto id = map.setDescription({1}, TestHandle::Invalid);
  map.deleteDescription(id);
  EXPECT_FALSE(map.hasDescription(id));
}

TEST(ResourceRegistryMapTest,
     DeletingDescriptionAddsResourceToStagedWithDeleteFlag) {
  TestMap map;
  auto id = map.setDescription({1}, TestHandle::Invalid);
  map.deleteDescription(id);
  EXPECT_EQ(map.getStagedResources().at(id),
            liquid::rhi::ResourceRegistryState::Delete);
}

TEST(ResourceRegistryMapTest, ClearsStagedResources) {
  TestMap map;
  auto id = map.setDescription({1}, TestHandle::Invalid);
  map.deleteDescription(id);
  map.clearStagedResources();
  EXPECT_EQ(map.getStagedResources().size(), 0);
}
