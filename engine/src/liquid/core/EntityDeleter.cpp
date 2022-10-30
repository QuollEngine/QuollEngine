#include "liquid/core/Base.h"
#include "EntityDeleter.h"

namespace liquid {

void addToDeleteList(Entity entity, EntityDatabase &entityDatabase,
                     std::vector<Entity> &deleteList) {
  if (entityDatabase.has<ChildrenComponent>(entity)) {
    for (auto &child : entityDatabase.get<ChildrenComponent>(entity).children) {
      addToDeleteList(child, entityDatabase, deleteList);
    }
  }

  deleteList.push_back(entity);
}

void EntityDeleter::update(Scene &scene) {
  auto &entityDatabase = scene.entityDatabase;
  auto activeCamera = scene.activeCamera;

  auto count = entityDatabase.getEntityCountForComponent<DeleteComponent>();

  std::vector<Entity> deleteList;
  deleteList.reserve(count);

  bool cameraDeleted = false;

  entityDatabase.iterateEntities<DeleteComponent>(
      [&deleteList, &entityDatabase, &activeCamera,
       &cameraDeleted](auto entity, auto &) mutable {
        cameraDeleted = cameraDeleted || (activeCamera == entity);

        if (entityDatabase.has<ParentComponent>(entity)) {
          auto parent = entityDatabase.get<ParentComponent>(entity).parent;

          if (entityDatabase.has<ChildrenComponent>(parent)) {
            auto &children =
                entityDatabase.get<ChildrenComponent>(parent).children;

            auto it = std::find(children.begin(), children.end(), entity);
            if (it != children.end()) {
              children.erase(it);
            }
          }
        }

        addToDeleteList(entity, entityDatabase, deleteList);
      });

  if (cameraDeleted) {
    scene.activeCamera = scene.dummyCamera;
  }

  for (auto entity : deleteList) {
    entityDatabase.deleteEntity(entity);
  }
}

} // namespace liquid
