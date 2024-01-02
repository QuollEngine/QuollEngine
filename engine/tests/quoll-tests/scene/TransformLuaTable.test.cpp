#include "quoll/core/Base.h"
#include "quoll/scene/LocalTransform.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class TransformLuaTableTest : public LuaScriptingInterfaceTestBase {};

using TransformLuaTableDeathTest = TransformLuaTableTest;

// Position
TEST_F(TransformLuaTableTest, GetPositionFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "localTransformPositionGet"), ".*");
}

TEST_F(TransformLuaTableTest, GetsPositionValue) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity,
                                            {glm::vec3(2.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformPositionGet");

  EXPECT_EQ(state["localTransformPosition"].get<glm::vec3>(),
            glm::vec3(2.5f, 0.2f, 0.5f));
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

TEST_F(TransformLuaTableTest, SetPositionValuesInvidiually) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity,
                                            {glm::vec3(1.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformPositionSetIndividual");

  EXPECT_EQ(entityDatabase.get<quoll::LocalTransform>(entity).localPosition,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

// Scale
TEST_F(TransformLuaTableDeathTest, GetScaleFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "localTransformScaleGet"), ".*");
}

TEST_F(TransformLuaTableTest, GetsScaleValue) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(
      entity, {{}, {}, glm::vec3(2.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformScaleGet");

  EXPECT_EQ(state["localTransformScale"].get<glm::vec3>(),
            glm::vec3(2.5f, 0.2f, 0.5f));
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

TEST_F(TransformLuaTableTest, SetsScaleValuesIndividually) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(
      entity, {{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformScaleSetIndividual");

  EXPECT_EQ(entityDatabase.get<quoll::LocalTransform>(entity).localScale,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

// XYZ -> Roll-Pitch-Yaw
constexpr glm::quat TestQuat{0.835327f, 0.350225f, 0.078362f, 0.416451f};

TEST_F(TransformLuaTableDeathTest, GetRotationFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "localTransformRotationGet"), ".*");
}

TEST_F(TransformLuaTableTest, GetRotationReturnsRotationQuaternion) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity, {{}, TestQuat, {}});

  auto state = call(entity, "localTransformRotationGet");

  auto actual = state["localTransformRotation"].get<glm::quat>();
  auto expected = TestQuat;

  EXPECT_NEAR(actual.x, expected.x, 0.0001f);
  EXPECT_NEAR(actual.y, expected.y, 0.0001f);
  EXPECT_NEAR(actual.z, expected.z, 0.0001f);
}

TEST_F(TransformLuaTableDeathTest, SetRotationFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "localTransformRotationSet"), ".*");
}

TEST_F(TransformLuaTableTest, SetRotationValue) {
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

TEST_F(TransformLuaTableTest, SetRotationValueIndividually) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity, {{}, {}, {}});

  auto state = call(entity, "localTransformRotationSetIndividual");

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
