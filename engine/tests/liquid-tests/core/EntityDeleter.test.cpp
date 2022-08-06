#include "liquid/core/Base.h"
#include "liquid/core/EntityDeleter.h"

#include "liquid-tests/Testing.h"

class EntityDeleterTest : public ::testing::Test {
public:
  liquid::EntityDatabase entityDatabase;
  liquid::EntityDeleter entityDeleter;
};

TEST_F(EntityDeleterTest, DeleteEntitiesThatHaveDeleteComponents) {
  static constexpr size_t NumEntities = 20;

  std::vector<liquid::Entity> entities(NumEntities, liquid::EntityNull);
  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entityDatabase.createEntity();
    entities.at(i) = entity;

    if ((i % 2) == 0) {
      entityDatabase.setComponent<liquid::DeleteComponent>(entity, {});
    }
  }

  for (auto entity : entities) {
    EXPECT_TRUE(entityDatabase.hasEntity(entity));
  }

  entityDeleter.update(entityDatabase);

  for (size_t i = 0; i < entities.size(); ++i) {
    auto entity = entities.at(i);
    EXPECT_NE(entityDatabase.hasEntity(entity), (i % 2) == 0);
  }
}
