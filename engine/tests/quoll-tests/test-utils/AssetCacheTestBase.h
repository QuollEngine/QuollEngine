#pragma once

#include "quoll/asset/AssetCache.h"
#include "quoll-tests/Testing.h"

class AssetCacheTestBase : public ::testing::Test {
public:
  static const quoll::Path CachePath;

public:
  AssetCacheTestBase();

  void SetUp() override;

  void TearDown() override;

public:
  quoll::AssetCache cache;
};
