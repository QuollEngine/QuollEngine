#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
#include "Children.h"
#include "LocalTransform.h"
#include "Parent.h"
#include "TransformSerializer.h"
#include "WorldTransform.h"

namespace quoll {

void TransformSerializer::serialize(YAML::Node &components,
                                    EntityDatabase &entityDatabase,
                                    Entity entity) {
  if (!entity.has<LocalTransform>()) {
    entity.set<LocalTransform>({});
  }
  auto component = entity.get_ref<LocalTransform>();

  components["transform"]["position"] = component->localPosition;
  components["transform"]["rotation"] = component->localRotation;
  components["transform"]["scale"] = component->localScale;

  if (entity.has<Parent>()) {
    auto parent = entity.get_ref<Parent>()->parent;

    if (parent.is_valid() && parent.has<Id>()) {
      components["transform"]["parent"] = parent.get_ref<Id>()->id;
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
      Entity parentEntity;

      if (it != entityIdCache.end()) {
        parentEntity = it->second;
      }

      if (parentEntity) {
        entity.set<Parent>({parentEntity});

        if (parentEntity.has<Children>()) {
          parentEntity.get_ref<Children>()->children.push_back(entity);
        } else {
          parentEntity.set<Children>({{entity}});
        }
      }
    }
  }

  entity.set(transform);
  entity.set<WorldTransform>({});
}

} // namespace quoll
