#include "quoll/core/Base.h"
#include "quoll/scene/LocalTransform.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class TransformLuaTableTest : public LuaScriptingInterfaceTestBase {};

using TransformLuaTableDeathTest = TransformLuaTableTest;

TEST_F(TransformLuaTableTest, GetPositionReturnsNullIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "localTransformPositionGet"), ".*");
}

TEST_F(TransformLuaTableTest, GetsPositionValue) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity,
                                            {glm::vec3(2.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformPositionGet");

  EXPECT_EQ(state["localPositionX"].get<f32>(), 2.5f);
  EXPECT_EQ(state["localPositionY"].get<f32>(), 0.2f);
  EXPECT_EQ(state["localPositionZ"].get<f32>(), 0.5f);
}

TEST_F(TransformLuaTableDeathTest, SetPositionFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "localTransformPositionSet"), ".*");
}

TEST_F(TransformLuaTableTest, SetsPositionValue) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity,
                                            {glm::vec3(1.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformPositionSet");

  EXPECT_EQ(entityDatabase.get<quoll::LocalTransform>(entity).localPosition,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

TEST_F(TransformLuaTableDeathTest, GetScaleFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "localTransformScaleGet"), ".*");
}

TEST_F(TransformLuaTableTest, GetsScaleValue) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(
      entity, {{}, {}, glm::vec3(2.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformScaleGet");

  EXPECT_EQ(state["localScaleX"].get<f32>(), 2.5f);
  EXPECT_EQ(state["localScaleY"].get<f32>(), 0.2f);
  EXPECT_EQ(state["localScaleZ"].get<f32>(), 0.5f);
}

TEST_F(TransformLuaTableDeathTest, SetScaleFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "localTransformScaleSet"), ".*");
}

TEST_F(TransformLuaTableTest, SetsScaleValue) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(
      entity, {{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformScaleSet");

  EXPECT_EQ(entityDatabase.get<quoll::LocalTransform>(entity).localScale,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

// XYZ -> Roll-Pitch-Yaw
constexpr glm::quat TestQuat{0.835326f, 0.3502249f, 0.0783618f, 0.4164508f};
constexpr glm::vec3 TestEulerDegrees{45.0f, 25.0f, 35.0f};

TEST_F(TransformLuaTableDeathTest, GetRotationFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "localTransformRotationGet"), ".*");
}

TEST_F(TransformLuaTableTest, GetRotationReturnsRotationInDegrees) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity, {{}, TestQuat, {}});

  auto state = call(entity, "localTransformRotationGet");

  auto actual = glm::vec3{state["localRotationX"].get<f32>(),
                          state["localRotationY"].get<f32>(),
                          state["localRotationZ"].get<f32>()};
  auto expected = TestEulerDegrees;

  EXPECT_NEAR(actual.x, expected.x, 0.0001f);
  EXPECT_NEAR(actual.y, expected.y, 0.0001f);
  EXPECT_NEAR(actual.z, expected.z, 0.0001f);
}

TEST_F(TransformLuaTableDeathTest, SetRotationFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "localTransformRotationSet"), ".*");
}

TEST_F(TransformLuaTableTest,
       SetRotationConvertsProvidedEulerDegreeRotationsToQuat) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity, {{}, {}, {}});

  auto state = call(entity, "localTransformRotationSet");

  auto actual = entityDatabase.get<quoll::LocalTransform>(entity).localRotation;
  auto expected = TestQuat;

  EXPECT_NEAR(actual.x, expected.x, 0.0001f);
  EXPECT_NEAR(actual.y, expected.y, 0.0001f);
  EXPECT_NEAR(actual.z, expected.z, 0.0001f);
  EXPECT_NEAR(actual.w, expected.w, 0.0001f);
}

TEST_F(TransformLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "localTransformDelete");
  EXPECT_FALSE(entityDatabase.has<quoll::LocalTransform>(entity));
}

TEST_F(TransformLuaTableTest, DeleteRemovesLocalTransformComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity, {});

  call(entity, "localTransformDelete");
  EXPECT_FALSE(entityDatabase.has<quoll::LocalTransform>(entity));
}
