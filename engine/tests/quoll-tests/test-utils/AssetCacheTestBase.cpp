#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"
#include "AssetCacheTestBase.h"

const quoll::Path AssetCacheTestBase::CachePath =
    std::filesystem::current_path() / "cache";

AssetCacheTestBase::AssetCacheTestBase() : cache(CachePath) {}

void AssetCacheTestBase::SetUp() {
  TearDown();
  std::filesystem::create_directory(CachePath);
}

void AssetCacheTestBase::TearDown() { std::filesystem::remove_all(CachePath); }
