#include "quoll/core/Base.h"
#include "ActionTestBase.h"

static const quoll::Path CachePath = std::filesystem::current_path() / "cache";

ActionTestBase::ActionTestBase() : assetCache(CachePath) {}
