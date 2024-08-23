#include "quoll/core/Base.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/Scene.h"
#include "quoll/system/SystemView.h"
#include "Delete.h"
#include "EntityDeleter.h"

namespace quoll {

void addToDeleteList(Entity entity, EntityDatabase &entityDatabase,
                     std::vector<Entity> &deleteList) {
  if (entity.has<Children>()) {
    for (auto &child : entity.get_ref<Children>()->children) {
      addToDeleteList(child, entityDatabase, deleteList);
    }
  }

  deleteList.push_back(entity);
}

void EntityDeleter::update(SystemView &view) {
  auto *scene = view.scene;
  auto &entityDatabase = scene->entityDatabase;
  auto activeCamera = scene->activeCamera;

  auto count = entityDatabase.count<Delete>();
  if (count == 0) {
    return;
  }

  std::vector<Entity> deleteList;
  deleteList.reserve(count);

  bool cameraDeleted = false;

  auto query =
      view.scene->entityDatabase.query_builder().with<Delete>().build();

  query.each([&](flecs::entity entity) {
    cameraDeleted = cameraDeleted || (activeCamera == entity);

    if (entity.has<Parent>()) {
      auto parent = entity.get_ref<Parent>()->parent;

      if (parent.has<Children>()) {
        auto &children = parent.get_ref<Children>()->children;

        auto it = std::find(children.begin(), children.end(), entity);
        if (it != children.end()) {
          children.erase(it);
        }
      }
    }

    addToDeleteList(entity, entityDatabase, deleteList);
  });

  if (cameraDeleted) {
    scene->activeCamera = scene->dummyCamera;
  }

  for (auto entity : deleteList) {
    entity.destruct();
  }
}

} // namespace quoll
