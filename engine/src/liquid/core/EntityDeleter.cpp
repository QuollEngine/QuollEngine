#include "liquid/core/Base.h"
#include "EntityDeleter.h"

namespace liquid {

void EntityDeleter::update(EntityContext &entityContext) {

  auto count = entityContext.getEntityCountForComponent<DeleteComponent>();

  std::vector<Entity> deleteList;
  deleteList.reserve(count);

  entityContext.iterateEntities<liquid::DeleteComponent>(
      [&deleteList](auto entity, auto &) { deleteList.push_back(entity); });

  for (auto entity : deleteList) {
    entityContext.deleteEntity(entity);
  }
}

} // namespace liquid
