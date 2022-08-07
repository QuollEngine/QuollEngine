#include "liquid/core/Base.h"
#include "EntityDeleter.h"

namespace liquid {

void addToDeleteList(Entity entity, EntityDatabase &entityDatabase,
                     std::vector<Entity> &deleteList) {
  if (entityDatabase.hasComponent<ChildrenComponent>(entity)) {
    for (auto &child :
         entityDatabase.getComponent<ChildrenComponent>(entity).children) {
      addToDeleteList(child, entityDatabase, deleteList);
    }
  }

  deleteList.push_back(entity);
}

void EntityDeleter::update(EntityDatabase &entityDatabase) {
  auto count = entityDatabase.getEntityCountForComponent<DeleteComponent>();

  std::vector<Entity> deleteList;
  deleteList.reserve(count);

  entityDatabase.iterateEntities<DeleteComponent>(
      [&deleteList, &entityDatabase](auto entity, auto &) {
        if (entityDatabase.hasComponent<ParentComponent>(entity)) {
          auto parent =
              entityDatabase.getComponent<ParentComponent>(entity).parent;

          if (entityDatabase.hasComponent<ChildrenComponent>(parent)) {
            auto &children =
                entityDatabase.getComponent<ChildrenComponent>(parent).children;

            auto it = std::find(children.begin(), children.end(), entity);
            if (it != children.end()) {
              children.erase(it);
            }
          }
        }

        addToDeleteList(entity, entityDatabase, deleteList);
      });

  for (auto entity : deleteList) {
    entityDatabase.deleteEntity(entity);
  }
}

} // namespace liquid
