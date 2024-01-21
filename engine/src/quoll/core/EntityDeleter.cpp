#include "quoll/core/Base.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/Scene.h"
#include "Delete.h"
#include "EntityDeleter.h"

namespace quoll {

void addToDeleteList(Entity entity, EntityDatabase &entityDatabase,
                     std::vector<Entity> &deleteList) {
  if (entityDatabase.has<Children>(entity)) {
    for (auto &child : entityDatabase.get<Children>(entity).children) {
      addToDeleteList(child, entityDatabase, deleteList);
    }
  }

  deleteList.push_back(entity);
}

void EntityDeleter::update(Scene &scene) {
  auto &entityDatabase = scene.entityDatabase;
  auto activeCamera = scene.activeCamera;

  auto count = entityDatabase.getEntityCountForComponent<Delete>();
  if (count == 0) {
    return;
  }

  std::vector<Entity> deleteList;
  deleteList.reserve(count);

  bool cameraDeleted = false;

  for (auto [entity, _] : entityDatabase.view<Delete>()) {
    cameraDeleted = cameraDeleted || (activeCamera == entity);

    if (entityDatabase.has<Parent>(entity)) {
      auto parent = entityDatabase.get<Parent>(entity).parent;

      if (entityDatabase.has<Children>(parent)) {
        auto &children = entityDatabase.get<Children>(parent).children;

        auto it = std::find(children.begin(), children.end(), entity);
        if (it != children.end()) {
          children.erase(it);
        }
      }
    }

    addToDeleteList(entity, entityDatabase, deleteList);
  }

  if (cameraDeleted) {
    scene.activeCamera = scene.dummyCamera;
  }

  for (auto entity : deleteList) {
    entityDatabase.deleteEntity(entity);
  }
}

} // namespace quoll
