#include "liquid/core/Base.h"
#include "EntityDeleter.h"

namespace liquid {

void EntityDeleter::update(EntityDatabase &entityDatabase) {

  auto count = entityDatabase.getEntityCountForComponent<DeleteComponent>();

  std::vector<Entity> deleteList;
  deleteList.reserve(count);

  entityDatabase.iterateEntities<liquid::DeleteComponent>(
      [&deleteList](auto entity, auto &) { deleteList.push_back(entity); });

  for (auto entity : deleteList) {
    entityDatabase.deleteEntity(entity);
  }
}

} // namespace liquid
