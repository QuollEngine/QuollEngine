#include "quoll/core/Base.h"
#include "Id.h"
#include "Name.h"
#include "NameSerializer.h"

namespace quoll {

void NameSerializer::serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                               Entity entity) {
  // Set name component if name is empty
  if (!entity.has<Name>() || entity.get_ref<Name>()->name.empty()) {
    if (entity.has<Id>()) {
      auto id = entity.get_ref<Id>()->id;
      entity.set<Name>({"Untitled " + std::to_string(id)});
    } else {
      entity.set<Name>({"Untitled"});
    }
  }
  node["name"] = entity.get_ref<Name>()->name;
}

void NameSerializer::deserialize(const YAML::Node &node,
                                 EntityDatabase &entityDatabase, Entity entity,
                                 const EntityIdCache &cache) {
  if (node["name"] && node["name"].IsScalar()) {
    auto name = node["name"].as<quoll::String>();
    entity.set<Name>({name});
  } else {
    auto name = "Untitled " + node["id"].as<String>();
    entity.set<Name>({name});
  }
}

} // namespace quoll
