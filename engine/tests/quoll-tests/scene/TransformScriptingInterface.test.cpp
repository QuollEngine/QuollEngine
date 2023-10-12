#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"

#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class TransformLuaScriptingInterfaceTest
    : public LuaScriptingInterfaceTestBase {};

using TransformLuaScriptingInterfaceDeathTest =
    TransformLuaScriptingInterfaceTest;

TEST_F(TransformLuaScriptingInterfaceTest,
       GetPositionReturnsNullIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "local_transform_position_get"), ".*");
}

TEST_F(TransformLuaScriptingInterfaceTest, GetsPositionValue) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity,
                                            {glm::vec3(2.5f, 0.2f, 0.5f)});

  auto state = call(entity, "local_transform_position_get");

  EXPECT_EQ(state["local_position_x"].get<float>(), 2.5f);
  EXPECT_EQ(state["local_position_y"].get<float>(), 0.2f);
  EXPECT_EQ(state["local_position_z"].get<float>(), 0.5f);
}

TEST_F(TransformLuaScriptingInterfaceDeathTest,
       SetPositionFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "local_transform_position_set"), ".*");
}

TEST_F(TransformLuaScriptingInterfaceTest, SetsPositionValue) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity,
                                            {glm::vec3(1.5f, 0.2f, 0.5f)});

  auto state = call(entity, "local_transform_position_set");

  EXPECT_EQ(entityDatabase.get<quoll::LocalTransform>(entity).localPosition,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

TEST_F(TransformLuaScriptingInterfaceDeathTest,
       GetScaleFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "local_transform_scale_get"), ".*");
}

TEST_F(TransformLuaScriptingInterfaceTest, GetsScaleValue) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(
      entity, {{}, {}, glm::vec3(2.5f, 0.2f, 0.5f)});

  auto state = call(entity, "local_transform_scale_get");

  EXPECT_EQ(state["local_scale_x"].get<float>(), 2.5f);
  EXPECT_EQ(state["local_scale_y"].get<float>(), 0.2f);
  EXPECT_EQ(state["local_scale_z"].get<float>(), 0.5f);
}

TEST_F(TransformLuaScriptingInterfaceDeathTest,
       SetScaleFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "local_transform_scale_set"), ".*");
}

TEST_F(TransformLuaScriptingInterfaceTest, SetsScaleValue) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(
      entity, {{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto state = call(entity, "local_transform_scale_set");

  EXPECT_EQ(entityDatabase.get<quoll::LocalTransform>(entity).localScale,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

// XYZ -> Roll-Pitch-Yaw
constexpr glm::quat TestQuat{0.835326f, 0.3502249f, 0.0783618f, 0.4164508f};
constexpr glm::vec3 TestEulerDegrees{45.0f, 25.0f, 35.0f};

TEST_F(TransformLuaScriptingInterfaceDeathTest,
       GetRotationFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "local_transform_rotation_get"), ".*");
}

TEST_F(TransformLuaScriptingInterfaceTest,
       GetRotationReturnsRotationInDegrees) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity, {{}, TestQuat, {}});

  auto state = call(entity, "local_transform_rotation_get");

  auto actual = glm::vec3{state["local_rotation_x"].get<float>(),
                          state["local_rotation_y"].get<float>(),
                          state["local_rotation_z"].get<float>()};
  auto expected = TestEulerDegrees;

  EXPECT_NEAR(actual.x, expected.x, 0.0001f);
  EXPECT_NEAR(actual.y, expected.y, 0.0001f);
  EXPECT_NEAR(actual.z, expected.z, 0.0001f);
}

TEST_F(TransformLuaScriptingInterfaceDeathTest,
       SetRotationFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_DEATH(call(entity, "local_transform_rotation_set"), ".*");
}

TEST_F(TransformLuaScriptingInterfaceTest,
       SetRotationConvertsProvidedEulerDegreeRotationsToQuat) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity, {{}, {}, {}});

  auto state = call(entity, "local_transform_rotation_set");

  auto actual = entityDatabase.get<quoll::LocalTransform>(entity).localRotation;
  auto expected = TestQuat;

  EXPECT_NEAR(actual.x, expected.x, 0.0001f);
  EXPECT_NEAR(actual.y, expected.y, 0.0001f);
  EXPECT_NEAR(actual.z, expected.z, 0.0001f);
  EXPECT_NEAR(actual.w, expected.w, 0.0001f);
}

TEST_F(TransformLuaScriptingInterfaceTest,
       DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "local_transform_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::LocalTransform>(entity));
}

TEST_F(TransformLuaScriptingInterfaceTest,
       DeleteRemovesLocalTransformComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::LocalTransform>(entity, {});

  call(entity, "local_transform_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::LocalTransform>(entity));
}
