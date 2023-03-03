#include "liquid/core/Base.h"
#include "liquid/entity/EntityStorageSparseSet.h"
#include "liquid/entity/EntityDatabase.h"

#include "liquid-tests/Testing.h"

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

static constexpr liquid::Entity DeadEntity{12};

/**
 * @brief Test entity storage
 *
 * Test class that allows quickly creating pools
 * by passing template parameters
 *
 * @tparam ...TComponentTypes Component types
 */
template <class... TComponentTypes>
class TestEntityStorage : public liquid::EntityStorageSparseSet {
public:
  /**
   * @brief Create test entity storage
   *
   * Create component pool from component types
   */
  TestEntityStorage() {
    if constexpr (sizeof...(TComponentTypes) > 0) {
      registerComponents<TComponentTypes...>();
    }
  }

  /**
   * @brief Register components
   *
   * Recursively register components
   *
   * @tparam TPick0 First component
   * @tparam ...TPickRest Rest components
   */
  template <class TPick0, class... TPickRest> void registerComponents() {
    reg<TPick0>();

    if constexpr (sizeof...(TPickRest) > 0) {
      registerComponents<TPickRest...>();
    }
  }
};

TEST(EntityStorageSparseSetTest, ReturnsFalseIfEntityDoesNotExist) {
  TestEntityStorage<Component1> storage;
  EXPECT_FALSE(storage.exists(liquid::Entity{12}));
}

TEST(EntityStorageSparseSetTest, ReturnsTrueIfEntityExists) {
  TestEntityStorage<Component1> storage;
  auto entity = storage.create();
  EXPECT_TRUE(storage.exists(entity));
}

TEST(EntityStorageSparseSetTest, ReturnsFalseIfComponentDoesNotExist) {
  TestEntityStorage<IntComponent, FloatComponent> storage;
  auto entity = storage.create();
  storage.set<IntComponent>(entity, {10});
  EXPECT_FALSE(storage.has<FloatComponent>(DeadEntity));
}

TEST(EntityStorageSparseSetTest, ReturnsTrueIfComponentExists) {
  TestEntityStorage<IntComponent> storage;
  auto entity = storage.create();
  storage.set<IntComponent>(entity, {10});
  EXPECT_TRUE(storage.has<IntComponent>(entity));
}

TEST(EntityStorageSparseSetDeathTest,
     GetThrowsErrorIfEntityDoesNotHaveComponent) {
  TestEntityStorage<IntComponent, FloatComponent> storage;
  auto entity = storage.create();
  storage.set<IntComponent>(entity, {1});

  EXPECT_DEATH({ storage.get<FloatComponent>(entity); }, ".*");
}

TEST(EntityStorageSparseSetDeathTest,
     GetThrowsErrorIfComponentIsNotRegistered) {
  TestEntityStorage storage;
  auto entity = storage.create();

  EXPECT_DEATH({ storage.get<FloatComponent>(entity); }, ".*");
}

TEST(EntityStorageSparseSetDeathTest,
     HasThrowsErrorIfComponentIsNotRegistered) {
  TestEntityStorage storage;
  auto entity = storage.create();

  EXPECT_DEATH({ storage.has<FloatComponent>(entity); }, ".*");
}

TEST(EntityStorageSparseSetDeathTest,
     SetThrowsErrorIfComponentIsNotRegistered) {
  TestEntityStorage storage;
  auto entity = storage.create();

  EXPECT_DEATH({ storage.set<FloatComponent>(entity, {}); }, ".*");
}

TEST(EntityStorageSparseSetTest, AddsComponentsIfDoesNotExist) {
  TestEntityStorage<Component1, Component2> storage;
  auto entity1 = storage.create();
  storage.set<Component1>(entity1, {5, 10.0f});
  storage.set<Component2>(entity1, {"My string", glm::vec2{1.0f, 2.5f}});
  auto entity2 = storage.create();
  storage.set<Component1>(entity2, {1, 2.5f});
  auto entity3 = storage.create();
  storage.set<Component2>(entity3, {"test string", glm::vec2{0.2f, 5.0f}});

  {
    const auto &comp1 = storage.get<Component1>(entity1);
    EXPECT_EQ(comp1.intValue, 5);
    EXPECT_EQ(comp1.realValue, 10.0f);

    const auto &comp2 = storage.get<Component2>(entity1);
    EXPECT_EQ(comp2.strValue, "My string");
    EXPECT_TRUE(comp2.vec2Value == glm::vec2(1.0f, 2.5f));
  }

  {
    const auto &comp1 = storage.get<Component1>(entity2);
    EXPECT_EQ(comp1.intValue, 1);
    EXPECT_EQ(comp1.realValue, 2.5f);
  }

  {
    const auto &comp1 = storage.get<Component2>(entity3);
    EXPECT_EQ(comp1.strValue, "test string");
    EXPECT_TRUE(comp1.vec2Value == glm::vec2(0.2f, 5.0f));
  }
}

TEST(EntityStorageSparseSetTest, UpdatesComponentIfExists) {
  TestEntityStorage<Component1> storage;
  auto entity = storage.create();
  storage.set<Component1>(entity, {5, 10.0f});

  {
    const auto &comp = storage.get<Component1>(entity);
    EXPECT_EQ(comp.intValue, 5);
    EXPECT_EQ(comp.realValue, 10.0f);
  }

  storage.set<Component1>(entity, {-2, -3.0f});

  {
    const auto &comp = storage.get<Component1>(entity);
    EXPECT_EQ(comp.intValue, -2);
    EXPECT_EQ(comp.realValue, -3.0f);
  }
}

TEST(EntityStorageSparseSetTest, CountsComponentsWithEntity) {
  TestEntityStorage<IntComponent> storage;
  auto e1 = storage.create();
  auto e2 = storage.create();
  auto e3 = storage.create();
  auto e4 = storage.create();

  storage.set<IntComponent>(e1, {});
  storage.set<IntComponent>(e2, {});
  storage.set<IntComponent>(e3, {});
  storage.set<IntComponent>(e4, {});

  EXPECT_EQ(storage.getEntityCountForComponent<IntComponent>(), 4);
}

TEST(EntityStorageSparseSetDeathTest,
     DeleteThrowsErrorIfEntityDoesNotHaveComponent) {
  TestEntityStorage<IntComponent> storage;
  auto entity = storage.create();
  EXPECT_DEATH({ storage.remove<IntComponent>(entity); }, ".*");
}

TEST(EntityStorageSparseSetTest, DeletesComponentIfExists) {
  TestEntityStorage<IntComponent, FloatComponent> storage;
  auto entity = storage.create();
  storage.set<IntComponent>(entity, {5});
  storage.set<FloatComponent>(entity, {5.0f});

  EXPECT_TRUE(storage.has<IntComponent>(entity));
  EXPECT_TRUE(storage.has<FloatComponent>(entity));

  storage.remove<IntComponent>(entity);

  EXPECT_FALSE(storage.has<IntComponent>(entity));
  EXPECT_TRUE(storage.has<FloatComponent>(entity));
}

TEST(EntityStorageSparseSetTest, UseRecyclesEntity) {
  TestEntityStorage<IntComponent, FloatComponent> storage;
  auto e1 = storage.create();
  auto e2 = storage.create();
  auto e3 = storage.create();
  storage.set<IntComponent>(e1, {5});
  storage.set<FloatComponent>(e1, {5.0f});
  storage.set<IntComponent>(e2, {1});
  storage.set<FloatComponent>(e2, {2.5f});
  storage.set<IntComponent>(e3, {0});
  storage.set<FloatComponent>(e3, {-3.5f});

  EXPECT_EQ(storage.getEntityCount(), 3);
  EXPECT_TRUE(storage.exists(e2));
  EXPECT_TRUE(storage.has<IntComponent>(e2));
  EXPECT_TRUE(storage.has<FloatComponent>(e2));

  storage.deleteEntity(e2);

  EXPECT_FALSE(storage.has<IntComponent>(e2));
  EXPECT_FALSE(storage.has<FloatComponent>(e2));
  EXPECT_FALSE(storage.exists(e2));
  EXPECT_EQ(storage.getEntityCount(), 2);

  auto recycledEntity = storage.create();
  EXPECT_EQ(storage.getEntityCount(), 3);
  EXPECT_TRUE(storage.exists(recycledEntity));
  EXPECT_EQ(recycledEntity, e2);
  EXPECT_FALSE(storage.has<IntComponent>(recycledEntity));
  EXPECT_FALSE(storage.has<FloatComponent>(recycledEntity));

  storage.set<IntComponent>(recycledEntity, {6});

  EXPECT_TRUE(storage.has<IntComponent>(recycledEntity));
  EXPECT_FALSE(storage.has<FloatComponent>(recycledEntity));
  EXPECT_EQ(storage.get<IntComponent>(recycledEntity).value, 6);
}

TEST(EntityStorageSparseSetTest, DoesNotDeleteNonExistentEntity) {
  TestEntityStorage<IntComponent, FloatComponent> storage;
  storage.deleteEntity(liquid::Entity::Null);

  auto e1 = storage.create();
  EXPECT_NE(e1, liquid::Entity::Null);
}

TEST(EntityStorageSparseSetTest, DeletesEntityAndItsComponentsIfExists) {
  TestEntityStorage<IntComponent, FloatComponent> storage;
  auto e1 = storage.create();
  auto e2 = storage.create();
  auto e3 = storage.create();
  storage.set<IntComponent>(e1, {5});
  storage.set<FloatComponent>(e1, {5.0f});
  storage.set<IntComponent>(e2, {1});
  storage.set<FloatComponent>(e2, {2.5f});
  storage.set<IntComponent>(e3, {0});
  storage.set<FloatComponent>(e3, {-3.5f});

  EXPECT_TRUE(storage.exists(e1));
  EXPECT_TRUE(storage.exists(e2));
  EXPECT_TRUE(storage.exists(e3));
  EXPECT_EQ(storage.getEntityCount(), 3);

  EXPECT_TRUE(storage.has<IntComponent>(e1));
  EXPECT_TRUE(storage.has<FloatComponent>(e1));
  EXPECT_TRUE(storage.has<IntComponent>(e2));
  EXPECT_TRUE(storage.has<FloatComponent>(e2));
  EXPECT_TRUE(storage.has<IntComponent>(e3));
  EXPECT_TRUE(storage.has<FloatComponent>(e3));

  storage.deleteEntity(e2);

  EXPECT_TRUE(storage.has<IntComponent>(e1));
  EXPECT_TRUE(storage.has<FloatComponent>(e1));
  EXPECT_FALSE(storage.has<IntComponent>(e2));
  EXPECT_FALSE(storage.has<FloatComponent>(e2));
  EXPECT_TRUE(storage.has<IntComponent>(e3));
  EXPECT_TRUE(storage.has<FloatComponent>(e3));

  EXPECT_TRUE(storage.exists(e1));
  EXPECT_FALSE(storage.exists(e2));
  EXPECT_TRUE(storage.exists(e3));
  EXPECT_EQ(storage.getEntityCount(), 2);
}

TEST(EntityStorageSparseSetTest, IterateEntities) {
  TestEntityStorage<IntComponent, FloatComponent, StringComponent, Component1>
      storage;
  auto e1 = storage.create(); // 0
  auto e2 = storage.create(); // 1
  auto e3 = storage.create(); // 2
  auto e4 = storage.create(); // 3
  auto e5 = storage.create(); // 4
  storage.set<IntComponent>(e1, {10});
  storage.set<FloatComponent>(e1, {10.2f});
  storage.set<StringComponent>(e1, {"Entity 1"});
  storage.set<IntComponent>(e2, {20});
  storage.set<FloatComponent>(e3, {30.2f});
  storage.set<IntComponent>(e3, {30});
  storage.set<StringComponent>(e3, {"Entity 3"});
  storage.set<IntComponent>(e4, {40});
  storage.set<Component1>(e4, {40, 40.4f});
  storage.set<IntComponent>(e5, {50});
  storage.set<StringComponent>(e5, {"Entity 5"});

  for (auto [entity, val] : storage.view<IntComponent>()) {
    EXPECT_TRUE(storage.exists(entity));
    EXPECT_EQ(storage.get<IntComponent>(entity).value, val.value);
  }

  for (auto [entity, val] : storage.view<FloatComponent>()) {
    EXPECT_TRUE(storage.exists(entity));
    EXPECT_EQ(storage.get<FloatComponent>(entity).value, val.value);
  }

  for (auto [entity, val] : storage.view<StringComponent>()) {
    EXPECT_TRUE(storage.exists(entity));
    EXPECT_EQ(storage.get<StringComponent>(entity).value, val.value);
  }

  for (auto [entity, val] : storage.view<Component1>()) {
    EXPECT_TRUE(storage.exists(entity));
    auto &comp = storage.get<Component1>(entity);
    EXPECT_EQ(comp.intValue, val.intValue);
    EXPECT_EQ(comp.realValue, val.realValue);
  }

  for (auto [entity, val1, val2] :
       storage.view<IntComponent, FloatComponent>()) {
    EXPECT_TRUE(storage.exists(entity));
    EXPECT_EQ(storage.get<IntComponent>(entity).value, val1.value);
    EXPECT_EQ(storage.get<FloatComponent>(entity).value, val2.value);
  }

  for (auto [entity, val1, val2, val3] :
       storage.view<IntComponent, FloatComponent, StringComponent>()) {
    EXPECT_TRUE(storage.exists(entity));
    EXPECT_EQ(storage.get<IntComponent>(entity).value, val1.value);
    EXPECT_EQ(storage.get<FloatComponent>(entity).value, val2.value);
    EXPECT_EQ(storage.get<StringComponent>(entity).value, val3.value);
  }

  for (auto [entity, _1, _2] : storage.view<FloatComponent, Component1>()) {
    EXPECT_TRUE(false) << "This function should not be called";
  }
}

TEST(EntityStorageSparseSetTest, DestroysOneComponent) {
  TestEntityStorage<IntComponent, FloatComponent, StringComponent> storage;
  auto e1 = storage.create(); // 0
  auto e2 = storage.create(); // 1
  storage.set<IntComponent>(e1, {10});
  storage.set<FloatComponent>(e1, {10.2f});
  storage.set<StringComponent>(e1, {"Entity 1"});
  storage.set<IntComponent>(e2, {20});
  storage.set<StringComponent>(e2, {"Entity 1"});

  EXPECT_TRUE(storage.has<IntComponent>(e1));
  EXPECT_TRUE(storage.has<FloatComponent>(e1));
  EXPECT_TRUE(storage.has<StringComponent>(e1));

  EXPECT_TRUE(storage.has<IntComponent>(e2));
  EXPECT_FALSE(storage.has<FloatComponent>(e2));
  EXPECT_TRUE(storage.has<StringComponent>(e1));

  storage.destroyComponents<IntComponent>();

  EXPECT_FALSE(storage.has<IntComponent>(e1));
  EXPECT_TRUE(storage.has<FloatComponent>(e1));
  EXPECT_TRUE(storage.has<StringComponent>(e1));

  EXPECT_FALSE(storage.has<IntComponent>(e2));
  EXPECT_FALSE(storage.has<FloatComponent>(e2));
  EXPECT_TRUE(storage.has<StringComponent>(e2));
}

TEST(EntityStorageSparseSetTest, DestroysAll) {
  TestEntityStorage<IntComponent, FloatComponent, StringComponent> storage;
  auto e1 = storage.create(); // 0
  auto e2 = storage.create(); // 1
  storage.set<IntComponent>(e1, {10});
  storage.set<FloatComponent>(e1, {10.2f});
  storage.set<StringComponent>(e1, {"Entity 1"});
  storage.set<IntComponent>(e2, {20});
  storage.set<StringComponent>(e2, {"Entity 1"});

  EXPECT_TRUE(storage.exists(e1));
  EXPECT_TRUE(storage.has<IntComponent>(e1));
  EXPECT_TRUE(storage.has<FloatComponent>(e1));
  EXPECT_TRUE(storage.has<StringComponent>(e1));

  EXPECT_TRUE(storage.exists(e2));
  EXPECT_TRUE(storage.has<IntComponent>(e2));
  EXPECT_FALSE(storage.has<FloatComponent>(e2));
  EXPECT_TRUE(storage.has<StringComponent>(e2));

  storage.destroy();

  EXPECT_FALSE(storage.exists(e1));
  EXPECT_FALSE(storage.has<IntComponent>(e1));
  EXPECT_FALSE(storage.has<FloatComponent>(e1));
  EXPECT_FALSE(storage.has<StringComponent>(e1));

  EXPECT_FALSE(storage.exists(e2));
  EXPECT_FALSE(storage.has<IntComponent>(e2));
  EXPECT_FALSE(storage.has<FloatComponent>(e2));
  EXPECT_FALSE(storage.has<StringComponent>(e1));
}

TEST(EntityStorageSparseSetTest, DeletesOnlyNeededComponents) {
  TestEntityStorage<IntComponent, StringComponent> storage;
  auto e1 = storage.create(); // 0
  auto e2 = storage.create(); // 1
  auto e3 = storage.create(); // 2
  storage.set<IntComponent>(e1, {10});
  storage.set<IntComponent>(e2, {20});
  storage.set<IntComponent>(e3, {30});

  storage.deleteEntity(e1);

  // This entity is going to fill up the space of old one
  auto newE1 = storage.create();
  EXPECT_EQ(e1, newE1);

  // Set component for the entity
  storage.set<StringComponent>(newE1, {"Hello World"});

  // When deleting entity, the int component's
  // entity index will point to DEAD_INDEX
  storage.deleteEntity(newE1);
}

TEST(EntityStorageSparseSetTest, DeletesMultipleComponents) {
  TestEntityStorage<IntComponent, StringComponent> storage;
  auto e1 = storage.create();
  auto e2 = storage.create();
  auto e3 = storage.create();
  auto e4 = storage.create();
  auto e5 = storage.create();
  auto e6 = storage.create();
  storage.set<IntComponent>(e1, {10});
  storage.set<IntComponent>(e2, {20});
  storage.set<IntComponent>(e3, {30});
  storage.set<IntComponent>(e4, {30});
  storage.set<IntComponent>(e5, {30});
  storage.set<IntComponent>(e6, {30});

  storage.remove<IntComponent>(e1);
  storage.remove<IntComponent>(e2);
  storage.remove<IntComponent>(e3);
  storage.remove<IntComponent>(e4);
  storage.remove<IntComponent>(e5);
  storage.remove<IntComponent>(e6);
}
