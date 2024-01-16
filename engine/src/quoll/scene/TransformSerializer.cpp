#include "quoll/core/Base.h"
#include "quoll/core/Id.h"

#include "LocalTransform.h"
#include "Parent.h"
#include "Children.h"
#include "WorldTransform.h"
#include "TransformSerializer.h"

namespace quoll {

void TransformSerializer::serialize(YAML::Node &components,
                                    EntityDatabase &entityDatabase,
                                    Entity entity) {
  if (!entityDatabase.has<LocalTransform>(entity)) {
    entityDatabase.set<LocalTransform>(entity, {});
  }
  const auto &component = entityDatabase.get<LocalTransform>(entity);

  components["transform"]["position"] = component.localPosition;
  components["transform"]["rotation"] = component.localRotation;
  components["transform"]["scale"] = component.localScale;

  if (entityDatabase.has<Parent>(entity)) {
    auto parent = entityDatabase.get<Parent>(entity).parent;

    if (entityDatabase.exists(parent) && entityDatabase.has<Id>(parent)) {
      components["transform"]["parent"] = entityDatabase.get<Id>(parent).id;
    }
  }
}

void TransformSerializer::deserialize(const YAML::Node &node,
                                      EntityDatabase &entityDatabase,
                                      Entity entity,
                                      const EntityIdCache &entityIdCache) {

  LocalTransform transform{};
  if (node["transform"] && node["transform"].IsMap()) {
    transform.localPosition =
        node["transform"]["position"].as<glm::vec3>(transform.localPosition);

    transform.localRotation =
        node["transform"]["rotation"].as<glm::quat>(transform.localRotation);

    transform.localScale =
        node["transform"]["scale"].as<glm::vec3>(transform.localScale);

    if (node["transform"]["parent"]) {
      auto parentId = node["transform"]["parent"].as<u64>(0);

      auto it = entityIdCache.find(parentId);
      Entity parentEntity =
          it != entityIdCache.end() ? it->second : Entity::Null;

      if (parentEntity != Entity::Null) {
        entityDatabase.set<Parent>(entity, {parentEntity});

        if (entityDatabase.has<Children>(parentEntity)) {
          entityDatabase.get<Children>(parentEntity).children.push_back(entity);
        } else {
          entityDatabase.set<Children>(parentEntity, {{entity}});
        }
      }
    }
  }

  entityDatabase.set(entity, transform);
  entityDatabase.set<WorldTransform>(entity, {});
}

} // namespace quoll
