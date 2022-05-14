#include "liquid/core/Base.h"
#include "liquid/entity/EntityStorageSparseSet.h"
#include "liquid/entity/EntityContext.h"

#include <gtest/gtest.h>

struct Component1 {
  int intValue;
  float realValue;
};

struct Component2 {
  std::string strValue;
  glm::vec2 vec2Value;
};

struct IntComponent {
  int value;
};

struct FloatComponent {
  float value;
};

struct StringComponent {
  std::string value;
};

TEST(EntityStorageSparseSetTests, ReturnsFalseIfEntityDoesNotExist) {
  liquid::EntityStorageSparseSet<Component1> storage;
  EXPECT_FALSE(storage.hasEntity(12));
}

TEST(EntityStorageSparseSetTests, ReturnsTrueIfEntityExists) {
  liquid::EntityStorageSparseSet<Component1> storage;
  auto entity = storage.createEntity();
  EXPECT_TRUE(storage.hasEntity(entity));
}

TEST(EntityStorageSparseSetTests, ReturnsFalseIfComponentDoesNotExist) {
  liquid::EntityStorageSparseSet<IntComponent, FloatComponent> storage;
  auto entity = storage.createEntity();
  storage.setComponent<IntComponent>(entity, {10});
  EXPECT_FALSE(storage.hasComponent<FloatComponent>(12));
}

TEST(EntityStorageSparseSetTests, ReturnsTrueIfComponentExists) {
  liquid::EntityStorageSparseSet<IntComponent> storage;
  auto entity = storage.createEntity();
  storage.setComponent<IntComponent>(entity, {10});
  EXPECT_TRUE(storage.hasComponent<IntComponent>(entity));
}

TEST(EntityStorageSparseSetDeathTest,
     GetThrowsErrorIfEntityDoesNotHaveComponent) {
  liquid::EntityStorageSparseSet<IntComponent, FloatComponent> storage;
  auto entity = storage.createEntity();
  storage.setComponent<IntComponent>(entity, {1});

  EXPECT_DEATH({ storage.getComponent<FloatComponent>(entity); }, ".*");
}

TEST(EntityStorageSparseSetTests, AddsComponentsIfDoesNotExist) {
  liquid::EntityStorageSparseSet<Component1, Component2> storage;
  auto entity1 = storage.createEntity();
  storage.setComponent<Component1>(entity1, {5, 10.0f});
  storage.setComponent<Component2>(entity1,
                                   {"My string", glm::vec2{1.0f, 2.5f}});
  auto entity2 = storage.createEntity();
  storage.setComponent<Component1>(entity2, {1, 2.5f});
  auto entity3 = storage.createEntity();
  storage.setComponent<Component2>(entity3,
                                   {"test string", glm::vec2{0.2f, 5.0f}});

  {
    const auto &comp1 = storage.getComponent<Component1>(entity1);
    EXPECT_EQ(comp1.intValue, 5);
    EXPECT_EQ(comp1.realValue, 10.0f);

    const auto &comp2 = storage.getComponent<Component2>(entity1);
    EXPECT_EQ(comp2.strValue, "My string");
    EXPECT_TRUE(comp2.vec2Value == glm::vec2(1.0f, 2.5f));
  }

  {
    const auto &comp1 = storage.getComponent<Component1>(entity2);
    EXPECT_EQ(comp1.intValue, 1);
    EXPECT_EQ(comp1.realValue, 2.5f);
  }

  {
    const auto &comp1 = storage.getComponent<Component2>(entity3);
    EXPECT_EQ(comp1.strValue, "test string");
    EXPECT_TRUE(comp1.vec2Value == glm::vec2(0.2f, 5.0f));
  }
}

TEST(EntityStorageSparseSetTests, UpdatesComponentIfExists) {
  liquid::EntityStorageSparseSet<Component1> storage;
  auto entity = storage.createEntity();
  storage.setComponent<Component1>(entity, {5, 10.0f});

  {
    const auto &comp = storage.getComponent<Component1>(entity);
    EXPECT_EQ(comp.intValue, 5);
    EXPECT_EQ(comp.realValue, 10.0f);
  }

  storage.setComponent<Component1>(entity, {-2, -3.0f});

  {
    const auto &comp = storage.getComponent<Component1>(entity);
    EXPECT_EQ(comp.intValue, -2);
    EXPECT_EQ(comp.realValue, -3.0f);
  }
}

TEST(EntityStorageSparseSetTests, CountsComponentsWithEntity) {
  liquid::EntityStorageSparseSet<IntComponent> storage;
  auto e1 = storage.createEntity();
  auto e2 = storage.createEntity();
  auto e3 = storage.createEntity();
  auto e4 = storage.createEntity();

  storage.setComponent<IntComponent>(e1, {});
  storage.setComponent<IntComponent>(e2, {});
  storage.setComponent<IntComponent>(e3, {});
  storage.setComponent<IntComponent>(e4, {});

  EXPECT_EQ(storage.getEntityCountForComponent<IntComponent>(), 4);
}

TEST(EntityStorageSparseSetDeathTest,
     DeleteThrowsErrorIfEntityDoesNotHaveComponent) {
  liquid::EntityStorageSparseSet<IntComponent> storage;
  auto entity = storage.createEntity();
  EXPECT_DEATH({ storage.deleteComponent<IntComponent>(entity); }, ".*");
}

TEST(EntityStorageSparseSetTests, DeletesComponentIfExists) {
  liquid::EntityStorageSparseSet<IntComponent, FloatComponent> storage;
  auto entity = storage.createEntity();
  storage.setComponent<IntComponent>(entity, {5});
  storage.setComponent<FloatComponent>(entity, {5.0f});

  EXPECT_TRUE(storage.hasComponent<IntComponent>(entity));
  EXPECT_TRUE(storage.hasComponent<FloatComponent>(entity));

  storage.deleteComponent<IntComponent>(entity);

  EXPECT_FALSE(storage.hasComponent<IntComponent>(entity));
  EXPECT_TRUE(storage.hasComponent<FloatComponent>(entity));
}

TEST(EntityStorageSparseSetTests, UseRecyclesEntity) {
  liquid::EntityStorageSparseSet<IntComponent, FloatComponent> storage;
  auto e1 = storage.createEntity();
  auto e2 = storage.createEntity();
  auto e3 = storage.createEntity();
  storage.setComponent<IntComponent>(e1, {5});
  storage.setComponent<FloatComponent>(e1, {5.0f});
  storage.setComponent<IntComponent>(e2, {1});
  storage.setComponent<FloatComponent>(e2, {2.5f});
  storage.setComponent<IntComponent>(e3, {0});
  storage.setComponent<FloatComponent>(e3, {-3.5f});

  EXPECT_EQ(storage.getEntityCount(), 3);
  EXPECT_TRUE(storage.hasEntity(e2));
  EXPECT_TRUE(storage.hasComponent<IntComponent>(e2));
  EXPECT_TRUE(storage.hasComponent<FloatComponent>(e2));

  storage.deleteEntity(e2);

  EXPECT_FALSE(storage.hasComponent<IntComponent>(e2));
  EXPECT_FALSE(storage.hasComponent<FloatComponent>(e2));
  EXPECT_FALSE(storage.hasEntity(e2));
  EXPECT_EQ(storage.getEntityCount(), 2);

  auto recycledEntity = storage.createEntity();
  EXPECT_EQ(storage.getEntityCount(), 3);
  EXPECT_TRUE(storage.hasEntity(recycledEntity));
  EXPECT_EQ(recycledEntity, e2);
  EXPECT_FALSE(storage.hasComponent<IntComponent>(recycledEntity));
  EXPECT_FALSE(storage.hasComponent<FloatComponent>(recycledEntity));

  storage.setComponent<IntComponent>(recycledEntity, {6});

  EXPECT_TRUE(storage.hasComponent<IntComponent>(recycledEntity));
  EXPECT_FALSE(storage.hasComponent<FloatComponent>(recycledEntity));
  EXPECT_EQ(storage.getComponent<IntComponent>(recycledEntity).value, 6);
}

TEST(EntityStorageSparseSetTests, DoesNotDeleteNonExistentEntity) {
  liquid::EntityStorageSparseSet<IntComponent, FloatComponent> storage;
  storage.deleteEntity(liquid::EntityNull);

  auto e1 = storage.createEntity();
  EXPECT_NE(e1, liquid::EntityNull);
}

TEST(EntityStorageSparseSetTests, DeletesEntityAndItsComponentsIfExists) {
  liquid::EntityStorageSparseSet<IntComponent, FloatComponent> storage;
  auto e1 = storage.createEntity();
  auto e2 = storage.createEntity();
  auto e3 = storage.createEntity();
  storage.setComponent<IntComponent>(e1, {5});
  storage.setComponent<FloatComponent>(e1, {5.0f});
  storage.setComponent<IntComponent>(e2, {1});
  storage.setComponent<FloatComponent>(e2, {2.5f});
  storage.setComponent<IntComponent>(e3, {0});
  storage.setComponent<FloatComponent>(e3, {-3.5f});

  EXPECT_TRUE(storage.hasEntity(e1));
  EXPECT_TRUE(storage.hasEntity(e2));
  EXPECT_TRUE(storage.hasEntity(e3));
  EXPECT_EQ(storage.getEntityCount(), 3);

  EXPECT_TRUE(storage.hasComponent<IntComponent>(e1));
  EXPECT_TRUE(storage.hasComponent<FloatComponent>(e1));
  EXPECT_TRUE(storage.hasComponent<IntComponent>(e2));
  EXPECT_TRUE(storage.hasComponent<FloatComponent>(e2));
  EXPECT_TRUE(storage.hasComponent<IntComponent>(e3));
  EXPECT_TRUE(storage.hasComponent<FloatComponent>(e3));

  storage.deleteEntity(e2);

  EXPECT_TRUE(storage.hasComponent<IntComponent>(e1));
  EXPECT_TRUE(storage.hasComponent<FloatComponent>(e1));
  EXPECT_FALSE(storage.hasComponent<IntComponent>(e2));
  EXPECT_FALSE(storage.hasComponent<FloatComponent>(e2));
  EXPECT_TRUE(storage.hasComponent<IntComponent>(e3));
  EXPECT_TRUE(storage.hasComponent<FloatComponent>(e3));

  EXPECT_TRUE(storage.hasEntity(e1));
  EXPECT_FALSE(storage.hasEntity(e2));
  EXPECT_TRUE(storage.hasEntity(e3));
  EXPECT_EQ(storage.getEntityCount(), 2);
}

TEST(EntityStorageSparseSetTests, IterateEntities) {
  liquid::EntityStorageSparseSet<IntComponent, FloatComponent, StringComponent,
                                 Component1>
      storage;
  auto e1 = storage.createEntity(); // 0
  auto e2 = storage.createEntity(); // 1
  auto e3 = storage.createEntity(); // 2
  auto e4 = storage.createEntity(); // 3
  auto e5 = storage.createEntity(); // 4
  storage.setComponent<IntComponent>(e1, {10});
  storage.setComponent<FloatComponent>(e1, {10.2f});
  storage.setComponent<StringComponent>(e1, {"Entity 1"});
  storage.setComponent<IntComponent>(e2, {20});
  storage.setComponent<FloatComponent>(e3, {30.2f});
  storage.setComponent<IntComponent>(e3, {30});
  storage.setComponent<StringComponent>(e3, {"Entity 3"});
  storage.setComponent<IntComponent>(e4, {40});
  storage.setComponent<Component1>(e4, {40, 40.4f});
  storage.setComponent<IntComponent>(e5, {50});
  storage.setComponent<StringComponent>(e5, {"Entity 5"});

  storage.iterateEntities<IntComponent>(
      [&storage](liquid::Entity entity, const auto &val) {
        EXPECT_TRUE(storage.hasEntity(entity));
        EXPECT_EQ(storage.getComponent<IntComponent>(entity).value, val.value);
      });

  storage.iterateEntities<FloatComponent>([&storage](liquid::Entity entity,
                                                     const auto &val) {
    EXPECT_TRUE(storage.hasEntity(entity));
    EXPECT_EQ(storage.getComponent<FloatComponent>(entity).value, val.value);
  });

  storage.iterateEntities<StringComponent>([&storage](liquid::Entity entity,
                                                      const auto &val) {
    EXPECT_TRUE(storage.hasEntity(entity));
    EXPECT_EQ(storage.getComponent<StringComponent>(entity).value, val.value);
  });

  storage.iterateEntities<Component1>(
      [&storage](liquid::Entity entity, const auto &val) {
        EXPECT_TRUE(storage.hasEntity(entity));
        auto &comp = storage.getComponent<Component1>(entity);
        EXPECT_EQ(comp.intValue, val.intValue);
        EXPECT_EQ(comp.realValue, val.realValue);
      });

  storage.iterateEntities<IntComponent, FloatComponent>(
      [&storage](liquid::Entity entity, const IntComponent &val1,
                 const FloatComponent &val2) {
        EXPECT_TRUE(storage.hasEntity(entity));
        EXPECT_EQ(storage.getComponent<IntComponent>(entity).value, val1.value);
        EXPECT_EQ(storage.getComponent<FloatComponent>(entity).value,
                  val2.value);
      });

  storage.iterateEntities<IntComponent, FloatComponent, StringComponent>(
      [&storage](liquid::Entity entity, const auto &val1, const auto &val2,
                 const auto &val3) {
        EXPECT_TRUE(storage.hasEntity(entity));
        EXPECT_EQ(storage.getComponent<IntComponent>(entity).value, val1.value);
        EXPECT_EQ(storage.getComponent<FloatComponent>(entity).value,
                  val2.value);
        EXPECT_EQ(storage.getComponent<StringComponent>(entity).value,
                  val3.value);
      });

  storage.iterateEntities<FloatComponent, Component1>(
      [](liquid::Entity entity, const auto &, const auto &) {
        EXPECT_TRUE(false) << "This function should not be called";
      });
}

TEST(EntityStorageSparseSetTests, DestroysOneComponent) {
  liquid::EntityStorageSparseSet<IntComponent, FloatComponent, StringComponent>
      storage;
  auto e1 = storage.createEntity(); // 0
  auto e2 = storage.createEntity(); // 1
  storage.setComponent<IntComponent>(e1, {10});
  storage.setComponent<FloatComponent>(e1, {10.2f});
  storage.setComponent<StringComponent>(e1, {"Entity 1"});
  storage.setComponent<IntComponent>(e2, {20});
  storage.setComponent<StringComponent>(e2, {"Entity 1"});

  EXPECT_TRUE(storage.hasComponent<IntComponent>(e1));
  EXPECT_TRUE(storage.hasComponent<FloatComponent>(e1));
  EXPECT_TRUE(storage.hasComponent<StringComponent>(e1));

  EXPECT_TRUE(storage.hasComponent<IntComponent>(e2));
  EXPECT_FALSE(storage.hasComponent<FloatComponent>(e2));
  EXPECT_TRUE(storage.hasComponent<StringComponent>(e1));

  storage.destroyComponents<IntComponent>();

  EXPECT_FALSE(storage.hasComponent<IntComponent>(e1));
  EXPECT_TRUE(storage.hasComponent<FloatComponent>(e1));
  EXPECT_TRUE(storage.hasComponent<StringComponent>(e1));

  EXPECT_FALSE(storage.hasComponent<IntComponent>(e2));
  EXPECT_FALSE(storage.hasComponent<FloatComponent>(e2));
  EXPECT_TRUE(storage.hasComponent<StringComponent>(e2));
}

TEST(EntityStorageSparseSetTests, DestroysAll) {
  liquid::EntityStorageSparseSet<IntComponent, FloatComponent, StringComponent>
      storage;
  auto e1 = storage.createEntity(); // 0
  auto e2 = storage.createEntity(); // 1
  storage.setComponent<IntComponent>(e1, {10});
  storage.setComponent<FloatComponent>(e1, {10.2f});
  storage.setComponent<StringComponent>(e1, {"Entity 1"});
  storage.setComponent<IntComponent>(e2, {20});
  storage.setComponent<StringComponent>(e2, {"Entity 1"});

  EXPECT_TRUE(storage.hasEntity(e1));
  EXPECT_TRUE(storage.hasComponent<IntComponent>(e1));
  EXPECT_TRUE(storage.hasComponent<FloatComponent>(e1));
  EXPECT_TRUE(storage.hasComponent<StringComponent>(e1));

  EXPECT_TRUE(storage.hasEntity(e2));
  EXPECT_TRUE(storage.hasComponent<IntComponent>(e2));
  EXPECT_FALSE(storage.hasComponent<FloatComponent>(e2));
  EXPECT_TRUE(storage.hasComponent<StringComponent>(e2));

  storage.destroy();

  EXPECT_FALSE(storage.hasEntity(e1));
  EXPECT_FALSE(storage.hasComponent<IntComponent>(e1));
  EXPECT_FALSE(storage.hasComponent<FloatComponent>(e1));
  EXPECT_FALSE(storage.hasComponent<StringComponent>(e1));

  EXPECT_FALSE(storage.hasEntity(e2));
  EXPECT_FALSE(storage.hasComponent<IntComponent>(e2));
  EXPECT_FALSE(storage.hasComponent<FloatComponent>(e2));
  EXPECT_FALSE(storage.hasComponent<StringComponent>(e1));
}

TEST(EntityStorageSparseSetTests, DeletesOnlyNeededComponents) {
  liquid::EntityStorageSparseSet<IntComponent, StringComponent> storage;
  auto e1 = storage.createEntity(); // 0
  auto e2 = storage.createEntity(); // 1
  auto e3 = storage.createEntity(); // 2
  storage.setComponent<IntComponent>(e1, {10});
  storage.setComponent<IntComponent>(e2, {20});
  storage.setComponent<IntComponent>(e3, {30});

  storage.deleteEntity(e1);

  // This entity is going to fill up the space of old one
  auto newE1 = storage.createEntity();
  EXPECT_EQ(e1, newE1);

  // Set component for the entity
  storage.setComponent<StringComponent>(newE1, {"Hello World"});

  // When deleting entity, the int component's
  // entity index will point to DEAD_INDEX
  storage.deleteEntity(newE1);
}

TEST(EntityStorageSparseSetTests, DeletesMultipleComponents) {
  liquid::EntityStorageSparseSet<IntComponent, StringComponent> storage;
  auto e1 = storage.createEntity();
  auto e2 = storage.createEntity();
  auto e3 = storage.createEntity();
  auto e4 = storage.createEntity();
  auto e5 = storage.createEntity();
  auto e6 = storage.createEntity();
  storage.setComponent<IntComponent>(e1, {10});
  storage.setComponent<IntComponent>(e2, {20});
  storage.setComponent<IntComponent>(e3, {30});
  storage.setComponent<IntComponent>(e4, {30});
  storage.setComponent<IntComponent>(e5, {30});
  storage.setComponent<IntComponent>(e6, {30});

  storage.deleteComponent<IntComponent>(e1);
  storage.deleteComponent<IntComponent>(e2);
  storage.deleteComponent<IntComponent>(e3);
  storage.deleteComponent<IntComponent>(e4);
  storage.deleteComponent<IntComponent>(e5);
  storage.deleteComponent<IntComponent>(e6);
}
