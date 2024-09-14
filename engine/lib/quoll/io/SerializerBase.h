#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/yaml/Yaml.h"

namespace quoll {

class AssetCache;

using EntityIdCache = std::unordered_map<u64, Entity>;

} // namespace quoll
