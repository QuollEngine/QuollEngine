#include "quoll/core/Base.h"
#include "Id.h"
#include "Name.h"
#include "NameSerializer.h"

namespace quoll {

void NameSerializer::serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                               Entity entity) {
  // Set name component if name is empty
  if (!entityDatabase.has<Name>(entity) ||
      entityDatabase.get<Name>(entity).name.empty()) {

    if (entityDatabase.has<Id>(entity)) {
      auto id = entityDatabase.get<Id>(entity).id;
      entityDatabase.set<Name>(entity, {"Untitled " + std::to_string(id)});
    } else {
      entityDatabase.set<Name>(entity, {"Untitled"});
    }
  }
  node["name"] = entityDatabase.get<Name>(entity).name;
}

void NameSerializer::deserialize(const YAML::Node &node,
                                 EntityDatabase &entityDatabase, Entity entity,
                                 const EntityIdCache &cache) {
  if (node["name"] && node["name"].IsScalar()) {
    auto name = node["name"].as<quoll::String>();
    entityDatabase.set<Name>(entity, {name});
  } else {
    auto name = "Untitled " + node["id"].as<String>();
    entityDatabase.set<Name>(entity, {name});
  }
}

} // namespace quoll
