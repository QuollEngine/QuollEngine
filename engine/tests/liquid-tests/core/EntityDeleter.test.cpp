#include "liquid/core/Base.h"
#include "liquid/core/EntityDeleter.h"

#include "liquid-tests/Testing.h"

class EntityDeleterTest : public ::testing::Test {
public:
  liquid::EntityContext entityContext;
  liquid::EntityDeleter entityDeleter;
};

TEST_F(EntityDeleterTest, DeleteEntitiesThatHaveDeleteComponents) {
  static constexpr size_t NUM_ENTITIES = 20;

  std::vector<liquid::Entity> entities(NUM_ENTITIES, liquid::EntityNull);
  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entityContext.createEntity();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      entityContext.setComponent<liquid::DeleteComponent>(entity, {});
    }
  }

  for (auto entity : entities) {
    EXPECT_TRUE(entityContext.hasEntity(entity));
  }

  entityDeleter.update(entityContext);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    EXPECT_NE(entityContext.hasEntity(entity), (i % 2) == 0);
  }
}
