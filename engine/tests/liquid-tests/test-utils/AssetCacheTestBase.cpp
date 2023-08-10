#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "AssetCacheTestBase.h"

const liquid::Path AssetCacheTestBase::CachePath =
    std::filesystem::current_path() / "cache";

AssetCacheTestBase::AssetCacheTestBase() : cache(CachePath) {}

void AssetCacheTestBase::SetUp() {
  std::filesystem::create_directory(CachePath);
}

void AssetCacheTestBase::TearDown() { std::filesystem::remove_all(CachePath); }
