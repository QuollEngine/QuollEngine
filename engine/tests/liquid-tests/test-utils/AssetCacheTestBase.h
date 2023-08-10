#pragma once

#include "liquid/asset/AssetCache.h"

/**
 * @brief Test base for asset cache tests
 */
class AssetCacheTestBase : public ::testing::Test {
public:
  /**
   * @brief Cache path
   */
  static const liquid::Path CachePath;

public:
  /**
   * @brief Create asset cache test base
   */
  AssetCacheTestBase();

  /**
   * @brief Set up test
   */
  void SetUp() override;

  /**
   * @brief Tear down test
   */
  void TearDown() override;

public:
  /**
   * Asset cache
   */
  liquid::AssetCache cache;
};
